#pragma once

#include "Common/Common.h"

namespace engine
{
	struct SpriteAsset;

	// .spr 텍스트 파일에서 SpriteAsset을 로드하는 유틸리티
	//
	// 파일 형식 (.spr):
	//   첫 줄: width height
	//   이후 height 줄: width 개의 색상 코드 (공백 구분)
	//
	// 색상 코드:
	//   . = 투명   K = Black   B = Blue    G = Green
	//   R = Red    W = White   S = Skyblue Y = Yellow   O = Orange
	//
	// 예시 (4x4):
	//   4 4
	//   . R R .
	//   R R R R
	//   R R R R
	//   . R R .
	class ENGINE_API SpriteLoader
	{
	public:
		// 파일에서 스프라이트를 로드한다.
		// 반환된 SpriteAsset* 의 수명은 호출자가 관리한다 (delete).
		// 로드 실패 시 nullptr 반환.
		static SpriteAsset* Load(const char* filePath);
	};
}
