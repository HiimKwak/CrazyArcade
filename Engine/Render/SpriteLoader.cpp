#include "SpriteLoader.h"
#include "SpriteAsset.h"

#include <cstdio>
#include <iostream>

namespace engine
{
	SpriteAsset::SpriteAsset(int width, int height)
		: width(width), height(height)
	{
		const int count = width * height;
		pixels = new Color[count];
		mask = new bool[count];
		for (int i = 0; i < count; ++i)
		{
			pixels[i] = Color::Black;
			mask[i] = false;
		}
	}

	SpriteAsset::~SpriteAsset()
	{
		delete[] pixels;
		delete[] mask;
		pixels = nullptr;
		mask = nullptr;
	}

	// -------------------------------------------------------

	static Color CharToColor(char c)
	{
		switch (c)
		{
		case 'K': return Color::Black;
		case 'B': return Color::Blue;
		case 'G': return Color::Green;
		case 'R': return Color::Red;
		case 'W': return Color::White;
		case 'S': return Color::Skyblue;
		case 'Y': return Color::Yellow;
		case 'O': return Color::Orange;
		default:  return Color::Black;
		}
	}

	SpriteAsset* SpriteLoader::Load(const char* filePath)
	{
		FILE* file = nullptr;
		fopen_s(&file, filePath, "rt");

		if (!file)
		{
			std::cout << "SpriteLoader: failed to open " << filePath << "\n";
			return nullptr;
		}

		int width = 0, height = 0;
		if (fscanf_s(file, "%d %d", &width, &height) != 2 || width <= 0 || height <= 0)
		{
			std::cout << "SpriteLoader: invalid header in " << filePath << "\n";
			fclose(file);
			return nullptr;
		}

		SpriteAsset* asset = new SpriteAsset(width, height);

		for (int y = 0; y < height; ++y)
		{
			for (int x = 0; x < width; ++x)
			{
				char code[4] = {};
				if (fscanf_s(file, "%3s", code, (unsigned)sizeof(code)) != 1)
				{
					std::cout << "SpriteLoader: unexpected end of data in " << filePath << "\n";
					fclose(file);
					return asset; // 부분 로드 반환
				}

				const int idx = y * width + x;
				if (code[0] == '.')
				{
					asset->mask[idx] = false;
					asset->pixels[idx] = Color::Black;
				}
				else
				{
					asset->mask[idx] = true;
					asset->pixels[idx] = CharToColor(code[0]);
				}
			}
		}

		fclose(file);
		return asset;
	}
}
