#include <iostream>
#include "States.h"
#include "StateManager.h"
#include "CollisionManager.h"
#include "EventManager.h"
#include "RenderManager.h"
#include "TextureManager.h"
using namespace std;

void State::Render()
{
	SDL_RenderPresent(REMA::Instance().GetRenderer());
}

TitleState::TitleState(){}

void TitleState::Enter()
{
	
}

void TitleState::Update()
{
	if (EVMA::KeyPressed(SDL_SCANCODE_N))
	{
		STMA::ChangeState( new GameState() );
	}
}

void TitleState::Render()
{
	SDL_SetRenderDrawColor(REMA::Instance().GetRenderer(), 0, 0, 255, 255);
	SDL_RenderClear(REMA::Instance().GetRenderer());
	State::Render();
}

void TitleState::Exit()
{
	
}

void GameState::ClearTurrets()
{
	for (unsigned i = 0; i < m_turrets.size(); i++)
	{
		delete m_turrets[i];
		m_turrets[i] = nullptr;
	}
	m_turrets.clear();
	m_turrets.shrink_to_fit();
	// You can assimilate some parts of this code for deleting bullets and enemies.
}

GameState::GameState():m_spawnCtr(1) {}

void GameState::Enter()
{
	TEMA::Load("../Assets/img/Turret.png", "turret");
	TEMA::Load("../Assets/img/Enemies.png", "enemy");
	s_enemies.push_back(new Enemy({ 80,0,40,57 }, { 512.0f, -57.0f, 40.0f, 57.0f }));
	// Create the DOM and load the XML file.

	tinyxml2::XMLDocument doc;
	if (doc.LoadFile("save.xml") != 0) {
		std::cout << "failed to load save" << std::endl;
		return;
	}
	
	auto state = doc.FirstChildElement();

	auto turrets = state->FirstChildElement("turrets")->FirstChildElement("data");
	
	while (turrets != nullptr)
	{
		float x = 0.f, y = 0.f;
		turrets->FindAttribute("x")->QueryFloatValue(&x);
		turrets->FindAttribute("y")->QueryFloatValue(&y);

		m_turrets.push_back(new Turret({ 0,0,100,100 },
			{ x, y, 100.0f,100.0f }));

		turrets = turrets->NextSiblingElement();
	}

	auto enemies = state->FirstChildElement("enemies")->FirstChildElement("data");

	while (enemies != nullptr)
	{
		float x = 0.f, y = 0.f;

		enemies->FindAttribute("x")->QueryFloatValue(&x);
		enemies->FindAttribute("y")->QueryFloatValue(&y);

		s_enemies.push_back(new Enemy({ 80,0,40,57 },
			{ x, y, 40.0f, 57.0f }));
		enemies = enemies->NextSiblingElement();
	}

	// Iterate through the Turret elements in the file and push_back new Turrets into the m_turrets vector.
		// Keep the width and height as 100.
	// Look at the last two XML examples from Week 3
}

void GameState::Update()
{
	// Parse T and C events.
	if (EVMA::KeyPressed(SDL_SCANCODE_T))
	{
		m_turrets.push_back(new Turret({0,0,100,100},
			{(float)(rand() % (1024 - 40)), (float)(rand() % (768 - 40)), 100.0f,100.0f}));
	}
	if (EVMA::KeyPressed(SDL_SCANCODE_C))
	{
		ClearTurrets();
	}
	// Update all GameObjects individually. Spawn enemies. Update turrets. Update enemies. Update bullets.
	if (m_spawnCtr++ % 180 == 0)
	{
		s_enemies.push_back(new Enemy({ 80,0,40,57 },
			{ (float)(rand() % (1024 - 40)), -57.0f, 40.0f, 57.0f }));
	}
	for (auto turret : m_turrets)
		turret->Update();
	for (auto enemy : s_enemies)
		enemy->Update();
	for (auto bullet : s_bullets)
		bullet->Update();
	// Cleanup bullets and enemies that go off screen.

		// for all bullets
			// if bullet goes off screen (four bounds checks)
			// or
			// if deleteMe of bullet is true
				// delete s_bullets[i]
				// set s_bullets[i] to nullptr
	
		// for all enemies, similar to above

	SDL_Rect window = { 0, 0, 1024, 768 };

	for (unsigned i = 0; i < s_bullets.size(); i++)
	{
		const auto [x, y, w, h] = s_bullets[i]->m_dst;
		SDL_Rect rect = { int(x), int(y), int(w), int(h) };
		// delete off screen bullets
		if (!SDL_HasIntersection(&window, &rect))
		{
			delete s_bullets[i];
			s_bullets[i] = nullptr;
			s_bullets.erase(s_bullets.begin() + i);
			i--;
		}
	}

	// also check enemies

	for (unsigned i = 0; i < s_enemies.size(); i++)
	{
		const auto [x, y, w, h] = s_enemies[i]->m_dst;
		SDL_Rect rect = { int(x), int(y), int(w), int(h) };
		// delete off screen bullets
		if (!SDL_HasIntersection(&window, &rect))
		{
			delete s_enemies[i];
			s_enemies[i] = nullptr;
			s_enemies.erase(s_enemies.begin() + i);
			i--;
		}
	}

	// Check for collisions with bullets and enemies.
	
		// for all bullets
			// for all enemies
				// check collision (use AABB check with SDL_FRect and SDL_FRect)
				// if (COMA::AABBCheck(/*SDL_FRect*/, /*SDL_FRect*/));
					// Then colliding and do the thing

	for (unsigned i = 0; i < s_bullets.size(); i++)
	{
		for (unsigned j = 0; j < s_enemies.size(); j++)
		{
			if (COMA::AABBCheck(s_bullets[i]->m_dst, s_enemies[j]->m_dst))
			{
				delete s_bullets[i];
				s_bullets[i] = nullptr;
				s_bullets.erase(s_bullets.begin() + i);
				i--;

				delete s_enemies[j];
				s_enemies[j] = nullptr;
				s_enemies.erase(s_enemies.begin() + j);
				j--;
			}
		}
	}
}

void GameState::Render()
{
	SDL_SetRenderDrawColor(REMA::Instance().GetRenderer(), 0, 0, 0, 255);
	SDL_RenderClear(REMA::Instance().GetRenderer());

	for (unsigned i = 0; i < m_turrets.size(); i++)
		m_turrets[i]->Render();
	for (unsigned i = 0; i < s_enemies.size(); i++)
		s_enemies[i]->Render();
	for (unsigned i = 0; i < s_bullets.size(); i++)
		s_bullets[i]->Render();

	SDL_Rect spawnBox = { 50, 618, 100, 100 };
	SDL_SetRenderDrawColor(REMA::Instance().GetRenderer(), 255, 255, 255, 255);
	SDL_RenderDrawRect(REMA::Instance().GetRenderer(), &spawnBox);

	// This code below prevents SDL_RenderPresent from running twice in one frame.
	if ( dynamic_cast<GameState*>( STMA::GetStates().back() ) ) // If current state is GameState.
		State::Render();
}

void GameState::Exit()
{
	// You can clear all children of the root node by calling .DeleteChildren(); and this will essentially clear the DOM.

	// Iterate through all the turrets and save their positions as child elements of the root node in the DOM.

	tinyxml2::XMLDocument doc;
	auto state = doc.NewElement("state");
	auto turrets = doc.NewElement("turrets");
	auto bullets = doc.NewElement("bullets");
	auto enemies = doc.NewElement("enemies");

	// <turrets>
	//     <data/>
	//     ...
	// </turrets>
	// <bullets>
	//	   <data/>
	//	   ...
	// </bullets>
	//

	for (const auto* pTurret : m_turrets)
	{
		auto elem = doc.NewElement("data");
		elem->SetAttribute("x", pTurret->m_dst.x);
		elem->SetAttribute("y", pTurret->m_dst.y);
		turrets->InsertEndChild(elem);
	}

	for (const auto* pBullet : s_bullets)
	{
		auto elem = doc.NewElement("data");
		elem->SetAttribute("dx", pBullet->m_dx);
		elem->SetAttribute("dy", pBullet->m_dy);
		elem->SetAttribute("x", pBullet->GetCenter().x);
		elem->SetAttribute("y", pBullet->GetCenter().y);
		bullets->InsertEndChild(elem);
	}

	for (const auto* pEnemy : s_enemies)
	{
		auto elem = doc.NewElement("data");
		elem->SetAttribute("x", pEnemy->m_dst.x);
		elem->SetAttribute("y", pEnemy->m_dst.y);
		enemies->InsertEndChild(elem);
	}

	state->InsertEndChild(turrets);
	state->InsertEndChild(bullets);
	state->InsertEndChild(enemies);

	doc.InsertFirstChild(state);

	SDL_assert(doc.SaveFile("save.xml") == 0);

	// Make sure to save to the XML file.
		// xmlDoc.SaveFile("Turrets.xml");
	ClearTurrets(); // Deallocate all turrets, then all other objects.
	for (unsigned i = 0; i < s_enemies.size(); i++)
	{
		delete s_enemies[i];
		s_enemies[i] = nullptr;
	}
	s_enemies.clear();
	s_enemies.shrink_to_fit();
	for (unsigned i = 0; i < s_bullets.size(); i++)
	{
		delete s_bullets[i];
		s_bullets[i] = nullptr;
	}
	s_bullets.clear();
	s_bullets.shrink_to_fit();
}

void GameState::Resume()
{
	
}

// This is how static properties are allocated.
std::vector<Bullet*> GameState::s_bullets;
std::vector<Enemy*> GameState::s_enemies;