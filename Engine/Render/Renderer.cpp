#include "Renderer.h"
#include "ScreenBuffer.h"
#include "Util/Util.h"

namespace Wanted
{
	Renderer::Frame::Frame(int bufferCount)
	{
		charInfoArray = new CHAR_INFO[bufferCount];
		memset(charInfoArray, 0, sizeof(CHAR_INFO) * bufferCount);
		//ZeroMemory(charInfoArray, sizeof(CHAR_INFO) * bufferCount) macro, 결국은 memset 호출

		sortingOrderArray = new int[bufferCount];
		memset(sortingOrderArray, 0, sizeof(int) * bufferCount); // 배열 동적할당 후엔 memset으로 제대로 초기화해주는게 좋음
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
			for (int x = 0; x < height; ++x)
			{
				// 2차원 배열처럼 사용하는 1차원 배열에서 인덱스 구하기
				const int index = (y * width) + x;

				// 글자 값 및 속성 설정
				CHAR_INFO& info = charInfoArray[index];
				info.Char.AsciiChar = ' ';
				info.Attributes = 0;

				// 그리기 우선순위 초기화
				sortingOrderArray[index] = -1;
			}
		}
	}

	// ----------- Frame ------------------- //
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
		for (ScreenBuffer*& buffer : screenBuffers)
		{
			SafeDelete(buffer);
		}
	}

	void Renderer::Draw()
	{
		// 화면 지우기
		Clear();

		// 렌더큐 순회하면서 프레임 채우기
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
			const int length = static_cast<int>(strlen(command.text));

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
				frame->charInfoArray[index].Char.AsciiChar = command.text[sourceIndex];
				frame->charInfoArray[index].Attributes = (WORD)command.color;

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

	void Renderer::Clear()
	{
		// 화면 지우기
		// 1. 프레임(2차원 배열) 지우기
		frame->Clear(screenSize);

		// 2. 콘솔 버퍼 지우기
		GetCurrentBuffer()->Clear();
	}

	void Renderer::Submit(const char* text, const Vector2& position, Color color, int sortingOrder)
	{
		// 렌더 데이터 생성 후 큐에 추가
		RenderCommand command = {}; // 스택변수로 썼지만 크기가 더 커지면 힙 영역을 쓰도록 바꿔줘도 됨
		command.text = text; // q. 직접 할당할 수밖에 없는 이유?
		command.position = position;
		command.color = color;
		command.sortingOrder = sortingOrder;

		renderQueue.emplace_back(command);
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

}