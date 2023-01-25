#ifndef SNAKE_H
#define SNAKE_H

#include <vector>
#include "SDL.h"
#include "Color.h"
//#include <game.h>

class Snake {
 public:
  enum class Direction { kUp, kDown, kLeft, kRight };

  Snake(int grid_width, int grid_height)
      : grid_width(grid_width),
        grid_height(grid_height),
        head_x(grid_width / 2),
        head_y(grid_height / 2),
        color(Color::blue()),
        size(1)
        {}

  Snake (int grid_width, int grid_height , float head_x , float head_y,Color  color, int size)
      : grid_width(grid_width),
      grid_height(grid_height),
      head_x(head_x),
      head_y(head_y),
      color(color),
      size(size){}

  void Update();

  void GrowBody();
  bool SnakeCell(int x, int y);

  Direction direction = Direction::kUp;

  float speed{0.1f};
  int size;
  bool alive{true};
  float head_x;
  float head_y;
  std::vector<SDL_Point> body;
  Color color;
  bool isPlayer{true};
  //void SetGame(Game* game);

 private:
  void UpdateHead();
  void UpdateBody(SDL_Point &current_cell, SDL_Point &prev_cell);

  bool growing{false};
  int grid_width;
  int grid_height;
  //Game* game;
 
};

#endif