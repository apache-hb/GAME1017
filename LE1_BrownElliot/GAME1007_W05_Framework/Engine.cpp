#include "Engine.h"

#include <iostream>
#include <string>
#include <filesystem>

#include "StateManager.h"

using namespace std;

Engine::Engine():m_pWindow(nullptr), m_pRenderer(nullptr), m_isRunning(false)
{
	cout << "Constructing Engine object!" << endl;
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
	
	if (TTF_Init() != 0)
	{
		cout << "Error during TTF Init" << endl;
		return 1;
	}

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
	m_isRunning = true; // Start your engine.

	auto here = std::filesystem::current_path().parent_path().string();
	cout << here << endl;

	m_sfx["pop"] = Mix_LoadWAV((here + "\\Resources\\pop.mp3").c_str());
	m_sfx["bang"] = Mix_LoadWAV((here + "\\Resources\\bang.mp3").c_str());

	m_music["title"] = Mix_LoadMUS((here + "\\Resources\\title.mp3").c_str());
	m_music["end"] = Mix_LoadMUS((here + "\\Resources\\end.mp3").c_str());

	cout << "loaded sfx " << (void*)m_sfx["pop"] << " " << (void*)m_sfx["bang"] << endl;
	cout << "loaded music " << (void*)m_music["title"] << " " << (void*)m_music["end"] << endl;
	return 0;
}

void Engine::HandleEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
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

Engine& Engine::Instance()
{
	static Engine instance; // Creating Engine object.
	return instance;
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
	StateManager::Render();
}

void Engine::Clean()
{
	cout << "Cleaning up..." << endl;
	SDL_DestroyRenderer(m_pRenderer);
	SDL_DestroyWindow(m_pWindow);
	StateManager::Quit();
	SDL_Quit();
}