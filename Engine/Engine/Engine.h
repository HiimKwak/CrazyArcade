#pragma once

#include "Common/Common.h"
#include "Util/Util.h"

namespace engine
{
	// 싱글톤 관계없이 명시적 전방선언은 필수
	class Input;
	class Renderer;
	class Level;

	class ENGINE_API Engine
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

		// 화면 너비 반환 함수
		inline int GetWidth() const { return settings.width; }
		// 화면 높이 반환 함수
		inline int GetHeight() const { return settings.height; }

	protected:
		// 정리 함수
		void Shutdown();

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
		// 전환할 레벨 임시 저장
		Level* nextLevel = nullptr;

		static Engine* instance;
	};
}
