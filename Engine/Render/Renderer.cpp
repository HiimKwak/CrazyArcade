#include "Renderer.h"
#include "ScreenBuffer.h"
#include "Util/Util.h"

namespace engine
{
	Renderer::Frame::Frame(int bufferCount)
	{
		charInfoArray = new CHAR_INFO[bufferCount];
		memset(charInfoArray, 0, sizeof(CHAR_INFO) * bufferCount);

		sortingOrderArray = new int[bufferCount];
		memset(sortingOrderArray, 0, sizeof(int) * bufferCount); // '정적' 배열 동적할당 후엔 memset으로 제대로 초기화해주는게 좋음
	}
	Renderer::Frame::~Frame()
	{
		SafeDeleteArray(charInfoArray);
		SafeDeleteArray(sortingOrderArray);
	}
	void Renderer::Frame::Clear(const Vector2& screenSize)
	{
		// 2차원 배열로 다루는 1차원 배열을 순회하면서
		// 빈 문자(' ')로 설정
		const int width = screenSize.x;
		const int height = screenSize.y;

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				// 2차원 배열처럼 사용하는 1차원 배열에서 인덱스 구하기
				const int index = (y * width) + x;

				// 글자 값 및 속성 설정
				CHAR_INFO& info = charInfoArray[index];
				info.Char.UnicodeChar = L' ';
				info.Attributes = 0;

				// 그리기 우선순위 초기화
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

	// 정적 변수 초기화
	Renderer* Renderer::instance = nullptr;

	Renderer::Renderer(const Vector2& screenSize)
		: screenSize(screenSize)
	{
		instance = this;

		const int bufferCount = screenSize.x * screenSize.y;
		frame = new Frame(bufferCount);

		// 프레임 초기화
		frame->Clear(screenSize);

		// 픽셀 버퍼 초기화 (세로 해상도 2배)
		pixelWidth = screenSize.x;
		pixelHeight = screenSize.y * 2;
		pixelBuffer = new PixelBuffer(pixelWidth * pixelHeight);

		// 이중 버퍼 객체 생성 및 초기화
		screenBuffers[0] = new ScreenBuffer(screenSize);
		screenBuffers[0]->Clear();

		screenBuffers[1] = new ScreenBuffer(screenSize);
		screenBuffers[1]->Clear();

		// 활성화 버퍼 설정
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
		// 화면 지우기
		Clear();

		// 픽셀 버퍼 → CHAR_INFO 프레임 변환 (half-block)
		ConvertPixelBufferToFrame();

		// 렌더큐 순회하면서 프레임 채우기 (텍스트/HUD는 픽셀 위에 덮어쓰기)
		// 전제조건: 레벨의 모든 액터가 렌더러에 렌더러에 Submit이 완료돼있어야 함
		for (const RenderCommand& command : renderQueue)
		{
			// 화면에 그릴 텍스트 없으면 건너뛰기
			if (!command.text)
				continue;

			// 세로 기준 화면 벗어났는지 확인
			if (command.position.y < 0 || command.position.y >= screenSize.y)
				continue;

			// 화면에 그릴 문자열 길이
			const int length = static_cast<int>(wcslen(command.text));

			// 안그려도 되면 건너뜀
			if (length <= 0)
				continue;

			// x좌표 기준 화면 벗어났는지 확인
			const int startX = command.position.x; // 왼쪽 첫글짜 시작
			const int endX = command.position.x + length - 1;
			if (endX < 0 || startX >= screenSize.x)
				continue;

			// 시작 위치
			const int visibleStart = startX < 0 ? 0 : startX; // e.g. 'ab|cde'
			const int visibleEnd = endX >= screenSize.x ? screenSize.x - 1 : endX;

			// 문자열 설정
			for (int x = visibleStart; x <= visibleEnd; ++x)
			{
				// 문자열 안의 문자 인덱스
				const int sourceIndex = x - startX; // e.g. 'ab|cde'의 c -> 0 - (-2) = 2('abcde'의 2번째 인덱스)

				// 프레임(2차원 문자 배열) 인덱스
				const int index = (command.position.y * screenSize.x) + x;

				// 그리기 우선순위 비교
				if (frame->sortingOrderArray[index] > command.sortingOrder)
					continue;

				// 데이터 기록
				frame->charInfoArray[index].Char.UnicodeChar = command.text[sourceIndex];
				frame->charInfoArray[index].Attributes = static_cast<WORD>(command.color);

				// 우선순위 업데이트
				frame->sortingOrderArray[index] = command.sortingOrder;
			}
		}

		// 그리기
		GetCurrentBuffer()->Draw(frame->charInfoArray);

		// 버퍼 교환
		Present();

		// 렌더 큐 비우기
		renderQueue.clear();

		// 픽셀 버퍼 초기화 (다음 프레임을 위해)
		pixelBuffer->Clear(pixelWidth * pixelHeight);
	}

	void Renderer::Clear()
	{
		// 화면 지우기
		// 1. 프레임(2차원 배열) 지우기
		frame->Clear(screenSize);

		// 2. 콘솔 버퍼 지우기
		GetCurrentBuffer()->Clear();
	}

	void Renderer::Submit(const wchar_t* text, const Vector2& position, Color color, int sortingOrder)
	{
		// 렌더 데이터 생성 후 큐에 추가
		RenderCommand command = {};
		command.text = text;
		command.position = position;
		command.color = color;
		command.sortingOrder = sortingOrder;

		renderQueue.emplace_back(command);
	}

	void Renderer::SubmitPixel(int x, int y, Color color, int sortingOrder)
	{
		if (x < 0 || x >= pixelWidth || y < 0 || y >= pixelHeight)
			return;

		const int index = y * pixelWidth + x;

		if (pixelBuffer->sortingOrderArray[index] > sortingOrder)
			return;

		pixelBuffer->colorArray[index] = color;
		pixelBuffer->sortingOrderArray[index] = sortingOrder;
	}

	void Renderer::SubmitRect(int x, int y, int w, int h, Color color, int sortingOrder)
	{
		for (int py = y; py < y + h; ++py)
		{
			for (int px = x; px < x + w; ++px)
			{
				SubmitPixel(px, py, color, sortingOrder);
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

				// 두 픽셀 모두 미설정이면 건너뛰기 (텍스트 렌더링에 맡김)
				if (topOrder < 0 && bottomOrder < 0)
					continue;

				const int frameIndex = cy * screenSize.x + x;
				const int pixelOrder = topOrder > bottomOrder ? topOrder : bottomOrder;

				if (frame->sortingOrderArray[frameIndex] > pixelOrder)
					continue;

				Color topColor = topOrder >= 0 ? pixelBuffer->colorArray[topIndex] : Color::Black;
				Color bottomColor = bottomOrder >= 0 ? pixelBuffer->colorArray[bottomIndex] : Color::Black;

				frame->charInfoArray[frameIndex].Char.UnicodeChar = L'\u2580'; // ?
				frame->charInfoArray[frameIndex].Attributes = MakeHalfBlockAttr(topColor, bottomColor);
				frame->sortingOrderArray[frameIndex] = pixelOrder;
			}
		}
	}

	void Renderer::Present()
	{
		// 버퍼 교환
		SetConsoleActiveScreenBuffer(GetCurrentBuffer()->GetBuffer());

		// 인덱스 교체
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