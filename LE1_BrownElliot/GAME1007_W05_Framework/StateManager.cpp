#include "StateManager.h"

namespace {
    std::vector<std::unique_ptr<State>> states; // Manage current state.

    State* CurrentState()
    {
        return states.empty() ? nullptr : states.back().get();
    }
}

void StateManager::Update()
{
    if (State* pState = CurrentState(); pState != nullptr)
    {
        pState->Update();
    }
}

void StateManager::Render()
{
    if (State* pState = CurrentState(); pState != nullptr)
    {
        pState->Render();
    }
}

void StateManager::PushState(State* pState)
{
    states.emplace_back(pState);
    pState->Enter();
}

void StateManager::PopState()
{
    if (State* pState = CurrentState(); pState == nullptr)
    {
        return;
    }
    else
    {
        pState->Exit();
        states.pop_back();
    }

    if (State* pState = CurrentState(); pState != nullptr) 
    {
        CurrentState()->Resume();
    }
}

void StateManager::ChangeState(State* pState)
{
    if (State* pState = CurrentState(); pState != nullptr)
    {
        pState->Exit();
        states.pop_back();
    }

    PushState(pState);
}

void StateManager::Quit()
{
    for (auto& state : states)
    {
        state->Exit();
    }
    states.clear();
}
