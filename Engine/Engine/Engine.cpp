#include <iostream>
#include <Windows.h>

#include "Engine.h"
#include "Level/Level.h"
#include "Core/Input.h"
#include "Util/Util.h"
#include "Render/Renderer.h"

namespace engine
{
	Engine* Engine::instance = nullptr;
	Engine::Engine()
	{
		// 전역변수 초기화
		instance = this;

		// input manager 생성
		input = new Input();

		// 설정파일 로드
		LoadSettings();

		// 렌더러 객체 생성
		renderer = new Renderer(Vector2(settings.viewportWidth, settings.viewportHeight));

		// 커서 끄기
		Util::TurnOffCursor();

		// 타이머 시드 설정
		Util::SetRandomSeed();
	}
	Engine::~Engine()
	{
		if (input)
		{
			delete input; // delete 연산자 안에서 소멸자 호출하므로 Engine 안에선 됨
			input = nullptr;
		}
		SafeDelete(mainLevel);
		SafeDelete(renderer);
	}
	void Engine::Run()
	{
		// Accuracy of Timer
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);

		//int64_t = STL 타입 재정의된 타입, __int64와 동일함
		__int64 currentTime = 0;
		__int64 previousTime = 0;

		// Hardware Timer //참고: timeGetTime(); milliseconds, 게임 외 소프트웨어에서 쓰이는 타이머 
		LARGE_INTEGER time;
		QueryPerformanceCounter(&time);

		// initialize previous time with currTime once at the start of the loop
		currentTime = time.QuadPart;
		previousTime = currentTime;

		// target frame rate
		//float targetFrameRate = 120.0f;
		settings.frameRate = settings.frameRate == 0.0f ? 60.0f : settings.frameRate;
		float oneFrameTime = 1.0f / settings.frameRate;

		while (!isQuit)
		{
			// current time
			QueryPerformanceCounter(&time);
			currentTime = time.QuadPart;

			// calculate delta time
			float deltaTime = static_cast<float>(currentTime - previousTime);

			// transform to seconds
			deltaTime /= static_cast<float>(frequency.QuadPart);

			// frame limiting
			if (deltaTime >= oneFrameTime)
			{
				input->ProcessInput();

				// processing one frame
				BeginPlay();
				Tick(deltaTime);
				Draw();


				// update previous time
				previousTime = currentTime;

				input->SavePreviousInputStates();

				// 레벨에 요청된 추가/제거 처리
				if (mainLevel)
				{
					mainLevel->ProcessAddAndDestoryActors();
				}

				// 레벨 전환 처리.
				if (nextLevel)
				{
					// 기존 레벨 제거.
					SafeDelete(mainLevel);

					// 전환할 레벨을 메인 레벨로 지정.
					mainLevel = nextLevel;

					// 포인터 정리.
					nextLevel = nullptr;
				}
			}
		}

		// 정리
		Shutdown();
	}
	void Engine::Quit()
	{
		isQuit = true;
	}
	void Engine::SetNewLevel(Level* newLevel)
	{
		mainLevel = newLevel;
	}

	void Engine::Shutdown()
	{
		std::cout << "Engine is being shutdown ..." << std::endl;

		Util::TurnOnCursor();
	}

	Engine& Engine::Get()
	{
		if (!instance)
		{
			std::cout << "Error: Engine::Get(). instance is null\n";
			__debugbreak();
		}
		return *instance;
	}

	void Engine::LoadSettings()
	{
		FILE* file = nullptr;

		fopen_s(&file, "../Config/Settings.txt", "rt");

		if (!file)
		{
			std::cout << "Failed to open engine settings file.\n";
			__debugbreak();
			return;
		}

		char buffer[2048] = {};

		size_t readSize = fread(buffer, sizeof(char), 2048, file);

		// 파싱
		// 첫번째 문자열 분리할 때는 첫 파라미터 전달
		char* context = nullptr;
		char* token = nullptr;
		token = strtok_s(buffer, "\n", &context);
		// 반복해서 자르기
		while (token)
		{
			// 설정 텍스트에서 파라미터 이름만 읽기
			char header[10] = {};
			// 문자열 읽기 함수 활용 // %s 포맷은 띄워쓰기 만나면 중단
			sscanf_s(token, "%s", header, 10);
			// 문자열 비교, 값 읽기
			if (strcmp(header, "frameRate") == 0)
			{
				sscanf_s(token, "frameRate = %f", &settings.frameRate);
			}
			else if (strcmp(header, "width") == 0)
			{
				sscanf_s(token, "width = %d", &settings.gameScreenWidth);
			}
			else if (strcmp(header, "height") == 0)
			{
				sscanf_s(token, "height = %d", &settings.gameScreenHeight);
			}
			// 개행문자로 문자열 분리
			token = strtok_s(nullptr, "\n", &context);
		}

		settings.viewportWidth = settings.gameScreenWidth;
		settings.viewportHeight = settings.gameScreenHeight + settings.hudHeight;

		fclose(file);
	}

	void Engine::BeginPlay()
	{
		if (!mainLevel) return;

		mainLevel->BeginPlay();
	}
	void Engine::Tick(float deltaTime)
	{
		if (!mainLevel) return;

		mainLevel->Tick(deltaTime);
	}
	void Engine::Draw()
	{
		if (!mainLevel) return;

		// 레벨의 모든 액터가 렌더 데이터 제출(Submit)
		mainLevel->Draw();

		// 렌더러에 그리기 명령 전달
		renderer->Draw();
	}
}
