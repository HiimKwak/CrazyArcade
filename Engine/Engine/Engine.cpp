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
		instance = this;

		input = new Input();

		LoadSettings();
		DetectConsoleSize();

		renderer = new Renderer(
			Vector2(settings.viewportWidth, settings.viewportHeight),
			settings.tileSize, settings.displayTileSize);

		Util::TurnOffCursor();
		Util::SetRandomSeed();
	}
	Engine::~Engine()
	{
		if (input)
		{
			delete input; 
			input = nullptr;
		}
		SafeDelete(mainLevel);
		SafeDelete(renderer);
	}
	void Engine::Run()
	{
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
	
		__int64 currentTime = 0;
		__int64 previousTime = 0;

		LARGE_INTEGER time;
		QueryPerformanceCounter(&time);

		currentTime = time.QuadPart;
		previousTime = currentTime;

		settings.frameRate = settings.frameRate == 0.0f ? 60.0f : settings.frameRate;
		float oneFrameTime = 1.0f / settings.frameRate;

		while (!isQuit)
		{
			QueryPerformanceCounter(&time);
			currentTime = time.QuadPart;

			float deltaTime = static_cast<float>(currentTime - previousTime);

			deltaTime /= static_cast<float>(frequency.QuadPart);

			if (deltaTime >= oneFrameTime)
			{
				input->ProcessInput();

				BeginPlay();
				Tick(deltaTime);
				Draw();

				previousTime = currentTime;

				input->SavePreviousInputStates();
	
				if (mainLevel)
					mainLevel->ProcessAddAndDestoryActors();
				
				if (nextLevel)
				{
					SafeDelete(mainLevel);
					mainLevel = nextLevel;
					nextLevel = nullptr;
				}
			}
		}	
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
		std::cout << "Engine is being shutdown...\n";

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

		char* context = nullptr;
		char* token = nullptr;
		token = strtok_s(buffer, "\n", &context);

		while (token)
		{
			char header[16] = {};
			sscanf_s(token, "%s", header, 16);
			if (strcmp(header, "frameRate") == 0)
				sscanf_s(token, "frameRate = %f", &settings.frameRate);
			else if (strcmp(header, "tileSize") == 0)
				sscanf_s(token, "tileSize = %d", &settings.tileSize);

			token = strtok_s(nullptr, "\n", &context);
		}

		fclose(file);
	}

	void Engine::DetectConsoleSize()
	{
		HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if (GetConsoleScreenBufferInfo(hStdout, &csbi))
		{
			settings.viewportWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
			settings.viewportHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
		}
		else
		{
			settings.viewportWidth = 80;
			settings.viewportHeight = 30;
		}
		settings.gameScreenWidth = settings.viewportWidth;
		settings.gameScreenHeight = settings.viewportHeight - settings.hudHeight;
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

		mainLevel->Draw();		
		renderer->Draw();
	}
}
