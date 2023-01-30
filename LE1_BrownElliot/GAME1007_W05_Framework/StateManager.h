#pragma once

#include "States.h"

#include <vector>
#include <memory>

namespace StateManager
{
	// Lifecycle/passthrough methods:
	void Update();
	void Render();

	// Unique to StateManager:
	void PushState(State* pState);
	void PopState();
	void ChangeState(State* pState);
	void Quit();
};
