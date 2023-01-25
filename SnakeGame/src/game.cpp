#include "game.h"
#include <iostream>
#include "SDL.h"
#include <chrono>


using namespace std;
using namespace std;
Game::Game(std::size_t grid_width, std::size_t grid_height, int enemies_count)
	:snake_player (grid_width, grid_height),
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
	while (running && snake_player.alive)
	{
		RefreshScreen(controller, renderer, target_frame_duration, title_timestamp, frame_start, frame_end,
			frame_duration, frame_count, running);

		// finding the difference time of two consecutive cycle, if it is 4 seconds then we should stop, go to next thread to update the enemies direction, then back here
		// the switch between two threads is presicely tested, there is no race neither a mutual exclusion
		// first we lock a shared mutex, the player turn initialzed as true in the beggining of the program
		// so even if the update enemies function called entered first (it is running by another thread), it will wait by conditional variable condition.
		// as soon as we enter to this IF scope, player turn will change to protect any fact race from current thread again, notify the sleeping thread on the other side and goes into wait until the next round
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

	// if score is already deducted and has not reseted yet, dont decrease the score again. 
	if (CheckMainPlayerIsHitted() == true && scoreDeducted == false)
	{
		score--;
		scoreDeducted = true;
		if (score < 0)
			snake_player.alive = false;
	}


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
	if (!snake_player.alive)
		return;
	
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

bool Game::CheckMainPlayerIsHitted()
{
	// we check if the enemy's head hit the player body. as soon as it happens we return true, only one collision is enough to return,
	// we do not decrease few point for one collision, this happens because the objects are moving and the head of enemy may hit few parts of player
	// so only one hit is counted 
	for (auto& en : snake_enemies)
	{
		SDL_Point head = { en.head_x  , en.head_y };
		for (auto& cell : snake_player.body)
			if (Points_Are_Equal(cell, head))
				 return true;
	}
	// if all of the enemies checked and there is no collision anymore, then we reset the scoreDeducted, means if any collision happens again we will decrease a point
	scoreDeducted = false;
	return false;
}

bool Game::Points_Are_Equal(SDL_Point& p1, SDL_Point& p2)
{
	if (p1.x == p2.x && p1.y == p2.y)
		return true;
	return false;

}

void Game::RunEnemies()
{
	for (auto& sn : snake_enemies)
		sn.Update();

}

void Game::CreateEnemies(int count, std::size_t grid_width, std::size_t grid_height)
{
	for (int i = 0; i < count; i++)
	{
		int x = random_w(engine);
		int y = random_h(engine);
		Snake en = Snake(grid_width, grid_height, x, y, Color::green(), 2);
		if (i % 2 == 0)
			en.direction = Snake::Direction::kDown;
		else
			en.direction = Snake::Direction::kRight;

		// create each cell of enemy's bedy and add to bedy vector
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
	// based on the direction of each enemy and player direction, we update the enemy direction. 
	// to make it hard, enemy will turn towards the player to hit the player
	while (running)
	{
		unique_lock<mutex> lock(mutex_player);
		cv.wait(lock, [&] { return  !is_player_turn; });
		// algorithm

		auto player_direction = snake_player.direction;
		for (auto& en : snake_enemies)
		{
			if (isDirectionParallel(en.direction, player_direction) == false) continue; // not parallel, they are colliding 
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

		// when the algorithm is done, we swithc the turn, to snake player. and notify the waited player. need to run enemies to render new direction of enemies. 
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