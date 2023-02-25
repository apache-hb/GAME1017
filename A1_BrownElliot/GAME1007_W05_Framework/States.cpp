#include "States.h"
#include "StateManager.h"
#include "Engine.h"

#include "imgui/imgui.h"

#include <algorithm>
#include <iostream>
#include <random>

auto random = [] {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
}();

static void HelpMarker(const char *text, const char* desc)
{
	ImGui::TextDisabled(text);
	if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

static void FullScreenWindow(const char* title)
{
	auto* pView = ImGui::GetMainViewport();
	constexpr ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;

	ImGui::SetNextWindowPos(pView->Pos);
	ImGui::SetNextWindowSize(pView->WorkSize);

	ImGui::Begin(title, nullptr, flags);
}

// TitleState

void TitleState::Enter()
{
	Mix_PlayMusic(Engine::m_music["title"], INT_MAX);
}

void TitleState::Exit()
{
	
}

void TitleState::Update()
{
	
}

void TitleState::Render()
{
	FullScreenWindow("title");

	ImGui::Text("The hat game!");
	ImGui::Image((ImTextureID)Engine::m_textures["title"].pTexture, {256, 256});

	if (ImGui::Button("Begin")) {
		StateManager::ChangeState(new GameState());
	}

	HelpMarker("About", 
		"This is rendered using Dear ImGui (https://github.com/ocornut/imgui)\n"
		"Dear ImGui is A FACADE ON TOP OF SDL, IT DOES NOT USE A DIFFERENT API\n"
		"I am using it because writing an entire layout toolkit for a school project is excessive\n"
		"Not to say i couldn't though\n"
		"- https://github.com/apache-hb/cthulhu\n"
		"- https://github.com/apache-hb/engine\n"
		"I wrote all the relevant code for this assignment"
	);

	HelpMarker("Credits", "Elliot Brown (101443805)");

	ImGui::End();
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
	if (Engine::KeyDown(SDL_SCANCODE_ESCAPE))
	{
		StateManager::PushState(new PauseState(this));
	}
	else if (Engine::KeyDown(SDL_SCANCODE_1))
	{
		int chan = Mix_PlayChannel(-1, Engine::m_sfx["pop"], 0);
		cout << "Playing clip 1 on channel: " << chan << endl;
	}
	else if (Engine::KeyDown(SDL_SCANCODE_2))
	{
		int chan = Mix_PlayChannel(-1, Engine::m_sfx["bang"], 0);
		cout << "Playing clip 2 on channel: " << chan << endl;
	}

	if (Engine::KeyDown(SDL_SCANCODE_LEFT))
	{
		player.x -= 5;
	}
	else if (Engine::KeyDown(SDL_SCANCODE_RIGHT))
	{
		player.x += 5;
	}

	if (Engine::KeyDown(SDL_SCANCODE_UP))
	{
		player.y -= 5;
	}
	if (Engine::KeyDown(SDL_SCANCODE_DOWN))
	{
		player.y += 5;
	}

	player.x = std::clamp(player.x, window.x, window.x + window.w);
	player.y = std::clamp(player.y, window.y, window.y + window.h);

	std::erase_if(playerBullets, [&](auto& it) {
		return SDL_HasIntersection(&window, &it.position) == SDL_FALSE;
	});

	std::erase_if(enemyBullets, [&](auto& it) {
		return SDL_HasIntersection(&window, &it.position) == SDL_FALSE;
	});

	std::erase_if(enemies, [&](auto& it) {
		return std::any_of(playerBullets.begin(), playerBullets.end(), [&](auto& bullet) {
			return SDL_HasIntersection(&it.position, &bullet.position);
		});
	});

	for (auto& [_, velocity, position] : playerBullets) {
		position.x += velocity.x;
		position.y += velocity.y;
	}

	for (auto& [_, velocity, position] : enemyBullets) {
		position.x += velocity.x;
		position.y += velocity.y;
	}

	std::uniform_int_distribution<> dist(-3, 3);

	for (auto& enemy : enemies) {
		if (enemy.timer++ > 60 * 5) {
			enemy.timer = 0;
		} else { continue; }

		enemyBullets.push_back({
			0,
			{ dist(random), dist(random) },
			{ enemy.position.x, enemy.position.y, 25, 25 }
		});
	}

	if (enemySpawnTimer++ > 60 * 5) {
		std::uniform_int_distribution x(0, kWidth);
		std::uniform_int_distribution y(0, kHeight);

		enemySpawnTimer = 0;
		enemies.push_back({
			0,
			{ 0, 0 },
			{ x(random), y(random), 50, 50 }
		});
	}

	int x, y;
	auto state = SDL_GetMouseState(&x, &y);

	if (state & SDL_BUTTON_LEFT)
	{
		playerBullets.push_back(
			{
				0,
				{ std::clamp(int((x - player.x) * 0.1), -3, 3), std::clamp(int((y - player.y) * 0.1), -3, 3) },
				{ player.x, player.y, 25, 25 }
			});
	}

	bool collide = std::any_of(enemyBullets.begin(), enemyBullets.end(), [&](auto& bullet) {
		return SDL_HasIntersection(&player, &bullet.position);
		});


	collide |= std::any_of(enemies.begin(), enemies.end(), [&](auto& bullet) {
		return SDL_HasIntersection(&player, &bullet.position);
		});

	if (collide) {
		StateManager::ChangeState(new EndState());
	}
}

void GameState::Render()
{
	const ImGuiWindowFlags kFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;
	const auto* pView = ImGui::GetMainViewport();
	const float pad = 10.f;
	ImVec2 workPos = pView->WorkPos;
	ImVec2 workSize = pView->WorkSize;

	// bottom left
	ImVec2 window = { workPos.x + pad, workPos.y + workSize.y - pad };

	ImVec2 pivot = { 0.f, 1.f };

	ImGui::SetNextWindowPos(window, ImGuiCond_Always, pivot);
	ImGui::SetNextWindowBgAlpha(0.35f);


	ImGui::Begin("Health", nullptr, kFlags);
	ImGui::Text("Health: %d/%d", currentHealth, maxHealth);
	ImGui::ProgressBar(float(currentHealth) / float(maxHealth));
	ImGui::End();

	const auto& [playerTexture, playerSize] = Engine::m_textures["player"];
	SDL_RenderCopy(Engine::GetRenderer(), playerTexture, &playerSize, &player);

	for (auto& it : playerBullets) {
		const auto& [texture, size] = Engine::m_textures["bullet"];
		SDL_RenderCopy(Engine::GetRenderer(), texture, &size, &it.position);
	}

	for (auto& it : enemyBullets) {
		const auto& [texture, size] = Engine::m_textures["bullet"];
		SDL_RenderCopy(Engine::GetRenderer(), texture, &size, &it.position);
	}

	for (auto& it : enemies) {
		const auto& [texture, size] = Engine::m_textures["enemy"];
		SDL_RenderCopy(Engine::GetRenderer(), texture, &size, &it.position);
	}
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
	if (Engine::KeyDown(SDL_SCANCODE_ESCAPE))
	{
		StateManager::PopState();
	}
}

void PauseState::Render()
{
	pGame->Render();
	ImGui::SetNextWindowBgAlpha(0.35f);
	FullScreenWindow("pause");

	if (ImGui::Button("Resume")) {
		StateManager::PopState();
	}

	if (ImGui::Button("Exit")) {
		StateManager::ChangeState(new EndState());
	}

	ImGui::End();
}

void EndState::Enter()
{
	Mix_PlayMusic(Engine::m_music["end"], INT_MAX);
}

void EndState::Exit()
{
}

void EndState::Update()
{
}

void EndState::Render()
{
	FullScreenWindow("game over");

	// draw game over text
	auto avail = ImGui::GetContentRegionAvail();
	auto size = ImGui::CalcTextSize("Game Over");

	ImGui::SetCursorPos({ (avail.x - size.x) / 2, (avail.y - size.y) / 2 });
	ImGui::Text("Game Over");

	if (ImGui::Button("Back to title screen")) {
		StateManager::ChangeState(new TitleState());
	}

	ImGui::SameLine();
	if (ImGui::Button("Play again?")) {
		StateManager::ChangeState(new GameState());
	}

	ImGui::End();
}
