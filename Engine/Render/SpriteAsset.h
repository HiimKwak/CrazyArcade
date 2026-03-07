#pragma once

#include "Common/Common.h"
#include "Math/Color.h"

namespace engine
{
	// 픽셀 스프라이트 에셋: Color 배열 + 투명 마스크
	struct ENGINE_API SpriteAsset
	{
		SpriteAsset(int width, int height);
		~SpriteAsset();

		int width = 0;
		int height = 0;
		Color* pixels = nullptr;	// [width * height] 색상 배열
		bool* mask = nullptr;		// [width * height] true = 불투명, false = 투명

	private:
		SpriteAsset(const SpriteAsset&) = delete;
		SpriteAsset& operator=(const SpriteAsset&) = delete;
	};
}
