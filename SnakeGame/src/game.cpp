#include "game.h"
#include <iostream>
#include "SDL.h"
#include <chrono>


using namespace std;
using namespace std;
Game::Game(std::size_t grid_width, std::size_t grid_height, int enemies_count)
	: snake_player(grid_width, grid_height),
	engine(dev()),
	random_w(0, static_cast<int>(grid_width - 1)),
	random_h(0, static_cast<int>(grid_height - 1))
{
	PlaceFood();
	CreateEnemies(enemies_count, grid_width, grid_height);
}

void Game::Run(Controller const& controller, Renderer& renderer, std::size_t target_frame_duration) {
	Uint32 title_timestamp = SDL_GetTicks();
	Uint32 frame_start;
	Uint32 frame_end;
	Uint32 frame_duration;
	int frame_count = 0;
	auto begin = std::chrono::high_resolution_clock::now();
	while (running)
	{
		RefreshScreen(controller, renderer, target_frame_duration, title_timestamp, frame_start, frame_end,
			frame_duration, frame_count, running);
		auto end = std::chrono::high_resolution_clock::now();
		auto diff = chrono::duration_cast<chrono::seconds>(end - begin);
		if (diff.count() >= 4)
		{
			unique_lock<mutex> lock(mutex_player);
			is_player_turn = false;
			cv.notify_all();
			cv.wait(lock, [&] { return  is_player_turn; });
			begin = std::chrono::high_resolution_clock::now();
		}
	}



}

void Game::RefreshScreen(Controller const& controller, Renderer& renderer, std::size_t& target_frame_duration,
	Uint32& title_timestamp, Uint32& frame_start, Uint32& frame_end, Uint32& frame_duration, int& frame_count, bool& running)
{
	frame_start = SDL_GetTicks();

	// Input, Update, Render - the main game loop.
	controller.HandleInput(running, snake_player);
	Update();
	renderer.ClearScreen();

	for (auto& sn : snake_enemies)
		renderer.Render(sn);
	renderer.RenderFood(food);
	renderer.Render(snake_player);
	renderer.UpdateScreen();

	frame_end = SDL_GetTicks();

	// Keep track of how long each loop through the input/update/render cycle
	// takes.
	frame_count++;
	frame_duration = frame_end - frame_start;

	// After every second, update the window title.
	if (frame_end - title_timestamp >= 1000) {
		renderer.UpdateWindowTitle(score, frame_count);
		frame_count = 0;
		title_timestamp = frame_end;
	}

	// If the time for this frame is too small (i.e. frame_duration is
	// smaller than the target ms_per_frame), delay the loop to
	// achieve the correct frame rate.
	if (frame_duration < target_frame_duration) {
		SDL_Delay(target_frame_duration - frame_duration);
	}

}

void Game::PlaceFood() {
	int x, y;
	while (true) {
		x = random_w(engine);
		y = random_h(engine);
		// Check that the location is not occupied by a snake item before placing
		// food.
		if (!snake_player.SnakeCell(x, y)) {
			food.x = x;
			food.y = y;
			return;
		}
	}
}

void Game::Update() {
	if (!snake_player.alive) return;

	RunEnemies();
	snake_player.Update();

	int new_x = static_cast<int>(snake_player.head_x);
	int new_y = static_cast<int>(snake_player.head_y);

	// Check if there's food over here
	if (food.x == new_x && food.y == new_y) {
		score++;
		PlaceFood();
		// Grow snake and increase speed.
		snake_player.GrowBody();
		snake_player.speed += 0.02;
	}
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake_player.size; }

void Game::RunEnemies()
{
	for (auto& sn : snake_enemies)
		sn.Update();

}

void Game::CreateEnemies(int count, std::size_t grid_width, std::size_t grid_height)
{
	//snake_enemies.resize(count);
	for (int i = 0; i < count; i++)
	{
		int x = random_w(engine);
		int y = random_h(engine);
		Snake en = Snake(grid_width, grid_height, x, y, Color::green(), 2);
		if (i % 2 == 0)
			en.direction = Snake::Direction::kDown;
		else
			en.direction = Snake::Direction::kRight;

		for (int sz = 0; sz < en.size; sz++)
		{
			switch (en.direction) {
			case Snake::Direction::kUp:
				en.head_y -= en.speed;
				break;

			case Snake::Direction::kDown:
				en.head_y += en.speed;
				break;

			case Snake::Direction::kLeft:
				en.head_x -= en.speed;
				break;

			case Snake::Direction::kRight:
				en.head_x += en.speed;
				break;
			}
			// Wrap the Snake around to the beginning if going off of the screen.
			en.head_x = fmod(en.head_x + grid_width, grid_width);
			en.head_y = fmod(en.head_y + grid_height, grid_height);

			SDL_Point cell{ static_cast<int>(en.head_x), static_cast<int>(en.head_y) };
			en.body.push_back(cell);
		}
		snake_enemies.push_back(en);
	}

}

void Game::UpdateEnemies()
{
	while (running)
	{
		unique_lock<mutex> lock(mutex_player);
		cv.wait(lock, [&] { return  !is_player_turn; });
		// algorithm

		auto player_direction = snake_player.direction;
		for (auto& en : snake_enemies)
		{
			if (isDirectionParallel(en.direction, player_direction) == false) continue; // not paralle, they are colliding 
			if (player_direction == Snake::Direction::kDown || player_direction == Snake::Direction::kUp)
			{
				if (snake_player.head_x < en.head_x) en.direction = Snake::Direction::kLeft;
				else if (snake_player.head_x > en.head_x) en.direction = Snake::Direction::kRight;
			}
			else
			{
				if (snake_player.head_y > en.head_y) en.direction = Snake::Direction::kDown;
				else if (snake_player.head_y < en.head_y) en.direction = Snake::Direction::kUp;
			}
		}

		is_player_turn = true;
		RunEnemies();
		cv.notify_all();
	}

}

bool Game::isDirectionParallel(Snake::Direction d1, Snake::Direction d2)
{
	if ((d1 == Snake::Direction::kDown || d1 == Snake::Direction::kUp) && (d2 == Snake::Direction::kDown || d2 == Snake::Direction::kUp))
		return true;
	if ((d1 == Snake::Direction::kRight || d1 == Snake::Direction::kLeft) && (d2 == Snake::Direction::kLeft || d2 == Snake::Direction::kRight))
		return true;
	return false;

}