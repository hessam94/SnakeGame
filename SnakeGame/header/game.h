#ifndef GAME_H
#define GAME_H

#include <random>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include <mutex>

using namespace std;
class Game {
public:
	Game(std::size_t grid_width, std::size_t grid_height, int enemies_count);
	void Run(Controller const& controller, Renderer& renderer,
		std::size_t target_frame_duration);
	int GetScore() const;
	int GetSize() const;
	void UpdateEnemies();
    bool is_player_turn;
	condition_variable cv;
	mutex mutex_player;
	bool running;
private:
	Snake snake_player;
	SDL_Point food;
	std::vector<Snake> snake_enemies;
	
	std::random_device dev;
	std::mt19937 engine;
	std::uniform_int_distribution<int> random_w;
	std::uniform_int_distribution<int> random_h;

	int score{ 0 };


	void PlaceFood();
	void Update();
	void CreateEnemies(int count, std::size_t grid_width, std::size_t grid_height);
	void RunEnemies();
	void RefreshScreen(Controller const& controller, Renderer& renderer, std::size_t& target_frame_duration,
		Uint32& title_timestamp, Uint32& frame_start, Uint32& frame_end, Uint32& frame_duration, int& frame_count, bool& running);
	bool isDirectionParallel(Snake::Direction d1, Snake::Direction d2);
	
};

#endif