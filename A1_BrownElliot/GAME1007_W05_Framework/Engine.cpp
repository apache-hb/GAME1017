#include "Engine.h"

#include <iostream>
#include <string>
#include <filesystem>
#include <chrono>

#include "StateManager.h"

#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_sdl.h"
#include "imgui/backends/imgui_impl_sdlrenderer.h"

using namespace std;

namespace {
	const Uint8* m_pKeystates;
	SDL_Window* m_pWindow; // Pointers are normal variables that hold addresses.
	SDL_Renderer* m_pRenderer; // Pointer to "back buffer"
	bool m_isRunning;

	// For fixed timestep.
	chrono::time_point<chrono::steady_clock> m_start, m_end;
	chrono::duration<double> m_diff;
	double m_fps; // Changed to double.

	double deltaTime;

	chrono::time_point<chrono::steady_clock> lastFrameTime, thisFrameTime; // Cleaned this up.
	chrono::duration<double> lastFrameDuration;

	auto here = std::filesystem::current_path().parent_path().string();
}

namespace Engine {
	unordered_map<string, Mix_Chunk*> m_sfx = {};
	unordered_map<string, Mix_Music*> m_music = {};
	unordered_map<string, Texture> m_textures = {};

	void addTexture(std::string name, std::string path) {
		SDL_Surface* pSurface = IMG_Load((here + path).c_str());
		SDL_Texture* pTexture = SDL_CreateTextureFromSurface(m_pRenderer, pSurface);

		m_textures[name] = Engine::Texture{ pTexture, { 0, 0, pSurface->w, pSurface->h } };

		SDL_FreeSurface(pSurface);
	}
}

int Engine::Run()
{
	if (m_isRunning)
	{
		return 1; // 1 arbitrarily means that engine is already running.
	}
	if (Init("GAME1017 Framework", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, kWidth, kHeight, NULL))
	{
		return 2; // 2 arbitrarily means that something went wrong in init.
	}

	StateManager::PushState(new TitleState());

	while (m_isRunning) // Main game loop. Run while isRunning = true.
	{
		ImGui_ImplSDLRenderer_NewFrame();
		ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		Wake();
		HandleEvents(); // Input
		Update();       // Processing
		Render();       // Output
		if (m_isRunning == true)
			Sleep();
	}
	Clean(); // Deinitialize SDL and free up memory.
	return 0;
}

int Engine::Init(const char* title, const int xPos, const int yPos, 
	const int width, const int height, const int flags)
{
	cout << "Initializing framework..." << endl;
	SDL_Init(SDL_INIT_EVERYTHING);
	m_pWindow = SDL_CreateWindow(title,	xPos, yPos, width, height, flags);
	if (m_pWindow == nullptr)
	{
		cout << "Error during window creation!" << endl;
		return 1;
	}
	m_pRenderer = SDL_CreateRenderer(m_pWindow, -1, 0);
	if (m_pRenderer == nullptr)
	{
		cout << "Error during renderer creation!" << endl;
		return 1;
	}
	// Initialize SDL sublibraries.

	if (IMG_Init(IMG_INIT_PNG) == 0)
	{
		cout << "Error during IMG Init" << endl;
		return 1;
	}

	if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 512) < 0)
	{
		cout << "Error during Mix Init" << endl;
		return 1;
	}

	if (Mix_AllocateChannels(4) < 0)
	{
		cout << "Error during allocchannels" << endl;
		return 1;
	}

	// Example-specific initialization.
	// STMA::ChangeState(new TitleState());
	// Initialize rest of framework.
	m_fps = 1.0 / kTargetFps; // Converts FPS into a fraction of seconds.
	m_pKeystates = SDL_GetKeyboardState(nullptr);
	lastFrameTime = chrono::high_resolution_clock::now();
	m_isRunning = true; // Start your Engine::

	cout << here << endl;

	m_sfx["pop"] = Mix_LoadWAV((here + "\\Resources\\pop.mp3").c_str());
	m_sfx["bang"] = Mix_LoadWAV((here + "\\Resources\\bang.mp3").c_str());

	m_music["title"] = Mix_LoadMUS((here + "\\Resources\\title.mp3").c_str());
	m_music["end"] = Mix_LoadMUS((here + "\\Resources\\end.mp3").c_str());

	addTexture("title", "\\Resources\\hat.jpeg");
	addTexture("player", "\\Resources\\player.png");
	addTexture("enemy", "\\Resources\\enemy.png");
	addTexture("bullet", "\\Resources\\bullet.png");

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplSDL2_InitForSDLRenderer(m_pWindow, m_pRenderer);
	ImGui_ImplSDLRenderer_Init(m_pRenderer);

	return 0;
}

void Engine::HandleEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		switch (event.type)
		{
		case SDL_QUIT: // Pressing 'X' icon in SDL window.
			m_isRunning = false; // Tell Run() we're done.
			break;
		}
	}
}

void Engine::Wake()
{
	thisFrameTime = chrono::high_resolution_clock::now(); // New snapshot of number of seconds.
	lastFrameDuration = thisFrameTime - lastFrameTime;
	deltaTime = lastFrameDuration.count(); // Now we have our deltaTime multiplier.
	lastFrameTime = thisFrameTime;

	m_start = thisFrameTime; // Comment this out to just use deltaTime.
}

bool Engine::KeyDown(SDL_Scancode c)
{
	if (m_pKeystates != nullptr)
	{
		if (m_pKeystates[c] == 1)
			return true;
	}
	return false;
}

SDL_Renderer* Engine::GetRenderer()
{
	return m_pRenderer;
}

void Engine::Update()
{
	StateManager::Update();
}

void Engine::Sleep() 
{
	// Note: Not really better, but you can decide to not limit frameRate and just use deltaTime.
	// Comment all this out to just use deltaTime.
	m_end = chrono::high_resolution_clock::now();
	m_diff = m_end - m_start; // Similar to before, but now chrono and double.
	if (m_diff.count() < m_fps)
		SDL_Delay((Uint32)((m_fps - m_diff.count()) * 1000.0)); // Sleep for number of ms.
}

void Engine::Render()
{
	SDL_SetRenderDrawColor(m_pRenderer, 0, 0, 0, 255);
	SDL_RenderClear(m_pRenderer);

	StateManager::Render();

	ImGui::Render();
	ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
	SDL_RenderPresent(m_pRenderer);
}

void Engine::Clean()
{
	cout << "Cleaning up..." << endl;
	
	ImGui_ImplSDLRenderer_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	SDL_DestroyRenderer(m_pRenderer);
	SDL_DestroyWindow(m_pWindow);
	StateManager::Quit();
	SDL_Quit();
}