#ifndef GAME_H
#define GAME_H

#include <random>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"

using namespace std;
class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height, int enemies_count);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;

 private:
  Snake snake;
  SDL_Point food;
 std:: vector<Snake> snake_enemies;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int score{0};

  void PlaceFood();
  void Update();
  void CreateEnemies(int count, std::size_t grid_width, std::size_t grid_height);
  void RunEnemies();
};

#endif