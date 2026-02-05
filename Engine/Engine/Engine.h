#pragma once

#include "Common/Common.h"
#include "Util/Util.h"

namespace  Wanted
{
	// 싱글톤 관계없이 명시적 전방선언은 필수
	class Input;
	class Renderer;
	class Level;

	class WANTED_API Engine
	{
		struct EngineSettings
		{
			float frameRate = 0.0f;

			// 화면 크기
			int width = 0;
			int height = 0;
		};

	public:
		Engine();
		virtual ~Engine(); // 자손 객체 소멸자도 누락안되고 실행됨

		// game loop
		void Run();
		void Quit();

		// add new level
		void SetNewLevel(Level* newLevel);

		static Engine& Get();

	protected:
		// 정리 함수
		void Shutdown()
		{
			std::cout << "Engine has been shutdown..." << std::endl;

			Util::TurnOnCursor();
		}

		// load setting file
		void LoadSettings();
		// begin game play
		void BeginPlay();

		// update
		void Tick(float deltaTime);

		// draw/render
		void Draw();

	protected:
		// quit flag
		bool isQuit = false;
		// engine settings
		EngineSettings settings;
		// 렌더러 객체
		Renderer* renderer = nullptr;
		// input manager
		Input* input = nullptr;
		// main level
		Level* mainLevel = nullptr;

		static Engine* instance;
	};
}
