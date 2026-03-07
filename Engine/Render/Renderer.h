#pragma once

#include <vector>
#include <Windows.h>

#include "Math/Vector2.h"
#include "Math/Color.h"

namespace engine
{
	class ScreenBuffer;

	class ENGINE_API Renderer
	{
		struct Frame
		{
			Frame(int bufferCount);
			~Frame();

			void Clear(const Vector2& screenSize);

			CHAR_INFO* charInfoArray = nullptr;
			int* sortingOrderArray = nullptr;
		};

		struct PixelBuffer
		{
			PixelBuffer(int pixelCount);
			~PixelBuffer();

			void Clear(int pixelCount);

			Color* colorArray = nullptr;
			int* sortingOrderArray = nullptr;
		};

		struct RenderCommand
		{
			const wchar_t* text = nullptr;
			Vector2 position;
			Color color = Color::White;
			int sortingOrder = 0;
		};

	public:
		Renderer(const Vector2& screenSize);
		~Renderer();

		static Renderer& Get();

		void Draw();

		void Submit(
			const wchar_t* text,
			const Vector2& position,
			Color color = Color::White,
			int sortingOrder = 0
		);

		// «»ºø ¥‹¿ß ∑ª¥ı ∏Ì∑… ¡¶√‚ (half-block ∑ª¥ı∏µ ∞Ê∑Œ)
		void SubmitPixel(int x, int y, Color color, int sortingOrder = 0);
		void SubmitRect(int x, int y, int w, int h, Color color, int sortingOrder = 0);

		inline int GetPixelWidth() const { return pixelWidth; }
		inline int GetPixelHeight() const { return pixelHeight; }

		void PresentImmediately();

	private:
		void Clear();
		void Present();
		void ConvertPixelBufferToFrame();

		ScreenBuffer* GetCurrentBuffer();

	private:
		Vector2 screenSize;
		Frame* frame = nullptr;
		ScreenBuffer* screenBuffers[2] = {};
		int currentBufferIndex = 0;
		std::vector<RenderCommand> renderQueue;

		PixelBuffer* pixelBuffer = nullptr;
		int pixelWidth = 0;
		int pixelHeight = 0;

		static Renderer* instance;
	};
}