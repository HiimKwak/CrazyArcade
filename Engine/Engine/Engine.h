#pragma once

#include "Common/Common.h"
#include "Util/Util.h"

namespace engine
{
	class Input;
	class Renderer;
	class Level;

	class ENGINE_API Engine
	{
		struct EngineSettings
		{
			float frameRate = 0.0f;

			int gameScreenWidth = 0;
			int gameScreenHeight = 0;
			int hudHeight = 5;
			int viewportWidth = 0;
			int viewportHeight = 0;
		};

	public:
		Engine();
		virtual ~Engine();

		void Run();
		void Quit();

		void SetNewLevel(Level* newLevel);

		static Engine& Get();

		inline int GetWidth() const { return settings.viewportWidth; }
		inline int GetHeight() const { return settings.viewportHeight; }

		inline int GetGameWidth() const { return settings.gameScreenWidth; }
		inline int GetGameHeight() const { return settings.gameScreenHeight; }
		inline int GetHudHeight() const { return settings.hudHeight; }

	protected:
		void Shutdown();
		void LoadSettings();
		void BeginPlay();
		void Tick(float deltaTime);
		void Draw();

	protected:
		bool isQuit = false;

		EngineSettings settings;

		Renderer* renderer = nullptr;

		Input* input = nullptr;

		Level* mainLevel = nullptr;
		Level* nextLevel = nullptr;

		static Engine* instance;
	};
}
