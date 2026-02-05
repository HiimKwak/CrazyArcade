#pragma once

#include <vector>
#include <Windows.h>

#include "Math/Vector2.h"
#include "Math/Color.h"

namespace Wanted
{
	// 콘솔 버퍼 전방선언
	class ScreenBuffer;

	// 더블 버퍼링 지원 렌더러
	class WANTED_API Renderer
	{
		// 프레임 구조체 - 2차원 문자열의 항목이 될 가상의 이미지
		struct Frame
		{
			Frame(int bufferCount);
			~Frame();

			// 지우기 함수
			void Clear(const Vector2& screenSize);

			// 글자 값과 색상을 갖는 타입
			CHAR_INFO* charInfoArray = nullptr;
			// 그리기 우선순위 배열
			int* sortingOrderArray = nullptr;
		};

		// 렌더할 데이터
		struct RenderCommand
		{
			// 화면에 보여줄 문자열 값
			const char* text = nullptr;
			// 좌표
			Vector2 position;
			// 색상
			Color color = Color::White;
			// 그리기 우선순위
			int sortingOrder = 0;
		};

	public:
		Renderer(const Vector2& screenSize);
		~Renderer();

		void Draw();
		// 그리는데 필요한 데이터 제출 함수
		void Submit(
			const char* text,
			const Vector2& position,
			Color color = Color::White,
			int sortingOrder = 0
		);

		// 싱글톤 접근 함수
		static Renderer& Get();

	private:
		void Clear();
		// 더블 버퍼링을 활용해 활성화 버퍼를 교환하는 함수
		void Present();
		// 사용할 버퍼 반환 Getter
		ScreenBuffer* GetCurrentBuffer();

	private:
		Vector2 screenSize;
		// 관리할 프레임 객체
		Frame* frame = nullptr;
		// 이중 버퍼 배열
		ScreenBuffer* screenBuffers[2] = {};
		// 현재 활성화된 버퍼 인덱스
		int currentBufferIndex = 0;
		// 렌더 큐(scene의 모든 그리기 명령을 모아두는 배열)
		std::vector<RenderCommand> renderQueue;
		// 싱글톤 구현을 위한 정적 변수
		static Renderer* instance;
	};
}