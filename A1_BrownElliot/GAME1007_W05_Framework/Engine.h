#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <unordered_map>
#include <string>

using namespace std;

constexpr auto kWidth = 1024;
constexpr auto kHeight = 768;
constexpr double kTargetFps = 60.0;

namespace Engine
{
	int Run();
	bool KeyDown(SDL_Scancode);
	SDL_Renderer* GetRenderer();

	struct Texture {
		SDL_Texture* pTexture;
		SDL_Rect rect;
	};

	extern unordered_map<string, Mix_Chunk*> m_sfx;
	extern unordered_map<string, Mix_Music*> m_music;
	extern unordered_map<string, Texture> m_textures;

	int Init(const char*, int, int, int, int, int);
	void HandleEvents();
	void Wake();
	void Update();
	void Sleep();
	void Render();
	void Clean();	
};
