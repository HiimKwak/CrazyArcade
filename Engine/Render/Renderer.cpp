#include "Renderer.h"
#include "ScreenBuffer.h"
#include "SpriteAsset.h"
#include "Util/Util.h"

namespace engine
{
	Renderer::Frame::Frame(int bufferCount)
	{
		charInfoArray = new CHAR_INFO[bufferCount];
		memset(charInfoArray, 0, sizeof(CHAR_INFO) * bufferCount);

		sortingOrderArray = new int[bufferCount];
		memset(sortingOrderArray, 0, sizeof(int) * bufferCount);
	}
	Renderer::Frame::~Frame()
	{
		SafeDeleteArray(charInfoArray);
		SafeDeleteArray(sortingOrderArray);
	}
	void Renderer::Frame::Clear(const Vector2& screenSize)
	{
		const int width = screenSize.x;
		const int height = screenSize.y;

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				const int index = (y * width) + x;

				CHAR_INFO& info = charInfoArray[index];
				info.Char.UnicodeChar = L' ';
				info.Attributes = 0;

				sortingOrderArray[index] = -1;
			}
		}
	}
	// ------------------ Frame ------------------- //

	// ------------------ PixelBuffer -------------- //
	Renderer::PixelBuffer::PixelBuffer(int pixelCount)
	{
		colorArray = new Color[pixelCount];
		sortingOrderArray = new int[pixelCount];
		Clear(pixelCount);
	}
	Renderer::PixelBuffer::~PixelBuffer()
	{
		SafeDeleteArray(colorArray);
		SafeDeleteArray(sortingOrderArray);
	}
	void Renderer::PixelBuffer::Clear(int pixelCount)
	{
		for (int i = 0; i < pixelCount; ++i)
		{
			colorArray[i] = Color::Black;
			sortingOrderArray[i] = -1;
		}
	}
	// ------------------ PixelBuffer -------------- //

	Renderer* Renderer::instance = nullptr;

	Renderer::Renderer(const Vector2& screenSize, int tileSize, int displayTileSize)
		: screenSize(screenSize), tileSize(tileSize), displayTileSize(displayTileSize)
	{
		instance = this;

		const int bufferCount = screenSize.x * screenSize.y;
		frame = new Frame(bufferCount);

		frame->Clear(screenSize);

		pixelWidth = screenSize.x;
		pixelHeight = screenSize.y * 2;
		pixelBuffer = new PixelBuffer(pixelWidth * pixelHeight);

		virtualWidth = pixelWidth * tileSize / displayTileSize;
		virtualHeight = pixelHeight * tileSize / displayTileSize;

		screenBuffers[0] = new ScreenBuffer(screenSize);
		screenBuffers[0]->Clear();

		screenBuffers[1] = new ScreenBuffer(screenSize);
		screenBuffers[1]->Clear();

		Present();
	}

	Renderer::~Renderer()
	{
		SafeDelete(frame);
		SafeDelete(pixelBuffer);
		for (ScreenBuffer*& buffer : screenBuffers)
		{
			SafeDelete(buffer);
		}
	}

	Renderer& Renderer::Get()
	{
		if (!instance)
		{
			MessageBoxA(nullptr, "Renderer::Get() - instance is null", "Error", MB_OK);

			__debugbreak();
		}

		return *instance;
	}

	void Renderer::Draw()
	{
		Clear();

		ConvertPixelBufferToFrame();

		for (const RenderCommand& command : renderQueue)
		{
			if (!command.text)
				continue;

			if (command.position.y < 0 || command.position.y >= screenSize.y)
				continue;

			const int length = static_cast<int>(wcslen(command.text));

			if (length <= 0)
				continue;

			const int startX = command.position.x;
			const int endX = command.position.x + length - 1;
			if (endX < 0 || startX >= screenSize.x)
				continue;

			const int visibleStart = startX < 0 ? 0 : startX; // e.g. 'ab|cde'
			const int visibleEnd = endX >= screenSize.x ? screenSize.x - 1 : endX;

			for (int x = visibleStart; x <= visibleEnd; ++x)
			{
				const int sourceIndex = x - startX; // e.g. 'ab|cde' c -> 0 - (-2) = 2

				const int index = (command.position.y * screenSize.x) + x;

				if (frame->sortingOrderArray[index] > command.sortingOrder)
					continue;

				frame->charInfoArray[index].Char.UnicodeChar = command.text[sourceIndex];
				frame->charInfoArray[index].Attributes = static_cast<WORD>(command.color);

				frame->sortingOrderArray[index] = command.sortingOrder;
			}
		}

		GetCurrentBuffer()->Draw(frame->charInfoArray);

		Present();

		renderQueue.clear();
		pixelBuffer->Clear(pixelWidth * pixelHeight);
	}

	void Renderer::Clear()
	{
		frame->Clear(screenSize);
		GetCurrentBuffer()->Clear();
	}

	void Renderer::Submit(const wchar_t* text, const Vector2& position, Color color, int sortingOrder)
	{
		RenderCommand command = {};
		command.text = text;
		command.position = position;
		command.color = color;
		command.sortingOrder = sortingOrder;

		renderQueue.emplace_back(command);
	}

	void Renderer::SubmitPixel(int x, int y, Color color, int sortingOrder)
	{
		const int sx = x * displayTileSize / tileSize;
		const int sy = y * displayTileSize / tileSize;

		if (sx < 0 || sx >= pixelWidth || sy < 0 || sy >= pixelHeight)
			return;

		const int index = sy * pixelWidth + sx;

		if (pixelBuffer->sortingOrderArray[index] > sortingOrder)
			return;

		pixelBuffer->colorArray[index] = color;
		pixelBuffer->sortingOrderArray[index] = sortingOrder;
	}

	void Renderer::SubmitRect(int x, int y, int w, int h, Color color, int sortingOrder)
	{
		int sx1 = x * displayTileSize / tileSize;
		int sy1 = y * displayTileSize / tileSize;
		int sx2 = (x + w) * displayTileSize / tileSize;
		int sy2 = (y + h) * displayTileSize / tileSize;

		if (sx2 <= sx1) sx2 = sx1 + 1;
		if (sy2 <= sy1) sy2 = sy1 + 1;

		for (int py = sy1; py < sy2; ++py)
		{
			for (int px = sx1; px < sx2; ++px)
			{
				if (px < 0 || px >= pixelWidth || py < 0 || py >= pixelHeight)
					continue;

				const int index = py * pixelWidth + px;

				if (pixelBuffer->sortingOrderArray[index] > sortingOrder)
					continue;

				pixelBuffer->colorArray[index] = color;
				pixelBuffer->sortingOrderArray[index] = sortingOrder;
			}
		}
	}

	void Renderer::SubmitSprite(int x, int y, const SpriteAsset* sprite, int sortingOrder)
	{
		if (!sprite)
			return;

		for (int py = 0; py < sprite->height; ++py)
		{
			for (int px = 0; px < sprite->width; ++px)
			{
				const int idx = py * sprite->width + px;
				if (!sprite->mask[idx])
					continue;

				SubmitPixel(x + px, y + py, sprite->pixels[idx], sortingOrder);
			}
		}
	}

	void Renderer::ConvertPixelBufferToFrame()
	{
		for (int cy = 0; cy < screenSize.y; ++cy)
		{
			for (int x = 0; x < screenSize.x; ++x)
			{
				const int topIndex = (cy * 2) * pixelWidth + x;
				const int bottomIndex = (cy * 2 + 1) * pixelWidth + x;

				const int topOrder = pixelBuffer->sortingOrderArray[topIndex];
				const int bottomOrder = pixelBuffer->sortingOrderArray[bottomIndex];

				if (topOrder < 0 && bottomOrder < 0)
					continue;

				const int frameIndex = cy * screenSize.x + x;
				const int pixelOrder = topOrder > bottomOrder ? topOrder : bottomOrder;

				if (frame->sortingOrderArray[frameIndex] > pixelOrder)
					continue;

				Color topColor = topOrder >= 0 ? pixelBuffer->colorArray[topIndex] : Color::Black;
				Color bottomColor = bottomOrder >= 0 ? pixelBuffer->colorArray[bottomIndex] : Color::Black;

				frame->charInfoArray[frameIndex].Char.UnicodeChar = L'\u2580';
				frame->charInfoArray[frameIndex].Attributes = MakeHalfBlockAttr(topColor, bottomColor);
				frame->sortingOrderArray[frameIndex] = pixelOrder;
			}
		}
	}

	void Renderer::Present()
	{
		SetConsoleActiveScreenBuffer(GetCurrentBuffer()->GetBuffer());
		currentBufferIndex = 1 - currentBufferIndex;
	}

	ScreenBuffer* Renderer::GetCurrentBuffer()
	{
		return screenBuffers[currentBufferIndex];
	}

	void Renderer::PresentImmediately()
	{
		Draw();
		GetCurrentBuffer()->Draw(frame->charInfoArray);
		Present();
	}
}