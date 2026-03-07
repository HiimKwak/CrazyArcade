#pragma once

#include <vector>
#include <Windows.h>

#include "Math/Vector2.h"
#include "Math/Color.h"

namespace engine
{
	class ScreenBuffer;
	struct SpriteAsset;

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
		Renderer(const Vector2& screenSize, int tileSize = 3, int displayTileSize = 3);
		~Renderer();

		static Renderer& Get();

		void Draw();

		void Submit(
			const wchar_t* text,
			const Vector2& position,
			Color color = Color::White,
			int sortingOrder = 0
		);

		// 픽셀 단위 렌더 명령 제출 (half-block 렌더링 경로)
		void SubmitPixel(int x, int y, Color color, int sortingOrder = 0);
		void SubmitRect(int x, int y, int w, int h, Color color, int sortingOrder = 0);
		void SubmitSprite(int x, int y, const SpriteAsset* sprite, int sortingOrder = 0);

		inline int GetPixelWidth() const { return virtualWidth; }
		inline int GetPixelHeight() const { return virtualHeight; }

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

		// 가상 해상도 (게임 코드가 사용하는 좌표계)
		int virtualWidth = 0;
		int virtualHeight = 0;
		int tileSize = 3;
		int displayTileSize = 3;

		static Renderer* instance;
	};
}