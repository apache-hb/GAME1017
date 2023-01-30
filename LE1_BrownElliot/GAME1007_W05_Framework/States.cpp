#include "States.h"
#include "StateManager.h"
#include "Engine.h"

#include <iostream>

void State::Render()
{
	SDL_RenderPresent(Engine::Instance().GetRenderer());
}

// TitleState

void TitleState::Enter()
{
	auto& engine = Engine::Instance();
	Mix_PlayMusic(engine.m_music["title"], INT_MAX);
}

void TitleState::Exit()
{
	
}

void TitleState::Update()
{
	if (Engine::Instance().KeyDown(SDL_SCANCODE_SPACE))
	{
		StateManager::ChangeState(new GameState());
	}
}

void TitleState::Render()
{
	SDL_Renderer* pRender = Engine::Instance().GetRenderer();
	SDL_SetRenderDrawColor(pRender, 0, 255, 255, 255);
	SDL_RenderClear(pRender);

	State::Render();
}

// GameState

void GameState::Enter()
{
}

void GameState::Exit()
{
}

void GameState::Update()
{
	auto& engine = Engine::Instance();
	if (engine.KeyDown(SDL_SCANCODE_ESCAPE))
	{
		StateManager::ChangeState(new PauseState());
	}
	else if (engine.KeyDown(SDL_SCANCODE_X))
	{
		StateManager::ChangeState(new EndState());
	}
	else if (engine.KeyDown(SDL_SCANCODE_1))
	{
		int chan = Mix_PlayChannel(-1, engine.m_sfx["pop"], 0);
		cout << "Playing clip 1 on channel: " << chan << endl;
	}
	else if (engine.KeyDown(SDL_SCANCODE_2))
	{
		int chan = Mix_PlayChannel(-1, engine.m_sfx["bang"], 0);
		cout << "Playing clip 2 on channel: " << chan << endl;
	}
}

void GameState::Render()
{
	SDL_Renderer* pRender = Engine::Instance().GetRenderer();
	SDL_SetRenderDrawColor(pRender, 255, 0, 255, 255);
	SDL_RenderClear(pRender);

	State::Render();
}

// PauseState

void PauseState::Enter()
{
}

void PauseState::Exit()
{
}

void PauseState::Update()
{
	if (Engine::Instance().KeyDown(SDL_SCANCODE_SPACE))
	{
		StateManager::ChangeState(new GameState());
	}
}

void PauseState::Render()
{
	SDL_Renderer* pRender = Engine::Instance().GetRenderer();
	SDL_SetRenderDrawColor(pRender, 0, 255, 0, 255);
	SDL_RenderClear(pRender);

	State::Render();
}

void EndState::Enter()
{
	auto& engine = Engine::Instance();
	Mix_PlayMusic(engine.m_music["end"], INT_MAX);
}

void EndState::Exit()
{
}

void EndState::Update()
{
}

void EndState::Render()
{
}
