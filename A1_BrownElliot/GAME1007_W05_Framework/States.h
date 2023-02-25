#pragma once


#include <vector>
#include "Engine.h"

struct State // Abstract base class.
{ // No objects of State can be created.
	virtual void Enter() = 0; // Pure virtual function. No definition.
	virtual void Exit() = 0;
	virtual void Resume() {} // Considered defined for all subclasses.

	virtual void Update() = 0;
	virtual void Render() = 0; // Will be defined in State.
};

struct TitleState : public State
{
	void Enter() override;
	void Exit() override;

	void Update() override;
	void Render() override;
};

struct GameState : public State
{
	void Enter() override;
	void Exit() override;

	void Update() override;
	void Render() override;

private:
	int currentHealth = 25;
	int maxHealth = 100;

	struct Entity {
		int timer = 0;
		SDL_Point velocity;
		SDL_Rect position;
	};

	SDL_Rect window{ 0, 0, kWidth, kHeight };
	SDL_Rect player{ 0, 0, 100, 100 };

	int enemySpawnTimer = 0;
	std::vector<Entity> enemies;
	std::vector<Entity> playerBullets;
	std::vector<Entity> enemyBullets;
};

struct PauseState : public State
{
	PauseState(GameState* pGame) : pGame(pGame) { }

	void Enter() override;
	void Exit() override;

	void Update() override;
	void Render() override;

	GameState* pGame;
};

struct EndState : public State
{
	void Enter() override;
	void Exit() override;

	void Update() override;
	void Render() override;
};
