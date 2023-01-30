#pragma once

struct State // Abstract base class.
{ // No objects of State can be created.
	virtual void Enter() = 0; // Pure virtual function. No definition.
	virtual void Exit() = 0;
	virtual void Resume() {} // Considered defined for all subclasses.

	virtual void Update() = 0;
	virtual void Render(); // Will be defined in State.
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
};

struct PauseState : public State
{
	void Enter() override;
	void Exit() override;

	void Update() override;
	void Render() override;
};

struct EndState : public State
{
	void Enter() override;
	void Exit() override;

	void Update() override;
	void Render() override;
};
