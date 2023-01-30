#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>

#include <chrono>
#include <unordered_map>

using namespace std;

constexpr auto kWidth = 1024;
constexpr auto kHeight = 768;
constexpr double kTargetFps = 60.0;

struct Engine
{
	int Run();
	bool KeyDown(SDL_Scancode);
	SDL_Renderer* GetRenderer();
	static Engine& Instance(); // Accessor for Engine object.
	
	double deltaTime;

	unordered_map<string, Mix_Chunk*> m_sfx;
	unordered_map<string, Mix_Music*> m_music;

	TTF_Font* m_pFont;

private:
	Engine();
	// For fixed timestep.
	chrono::time_point<chrono::steady_clock> m_start, m_end;
	chrono::duration<double> m_diff;
	double m_fps; // Changed to double.

	const Uint8* m_pKeystates;
	SDL_Window* m_pWindow; // Pointers are normal variables that hold addresses.
	SDL_Renderer* m_pRenderer; // Pointer to "back buffer"
	bool m_isRunning;

	chrono::time_point<chrono::steady_clock> lastFrameTime, thisFrameTime; // Cleaned this up.
	chrono::duration<double> lastFrameDuration;

	int Init(const char*, int, int, int, int, int);
	void HandleEvents();
	void Wake();
	void Update();
	void Sleep();
	void Render();
	void Clean();	
};
