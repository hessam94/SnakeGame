#include <SDL.h>
#include <iostream>
#include <math.h>
#include "controller.h"
#include "game.h"
#include "renderer.h"

using namespace std;
int main(int argc, char* args[]) 
{
  constexpr std::size_t kFramesPerSecond{60};
  constexpr std::size_t kMsPerFrame{1000 / kFramesPerSecond};
  constexpr std::size_t kScreenWidth{640};
  constexpr std::size_t kScreenHeight{640};
  constexpr std::size_t kGridWidth{32};
  constexpr std::size_t kGridHeight{32};

  Renderer renderer(kScreenWidth, kScreenHeight, kGridWidth, kGridHeight);
  Controller controller;
  shared_ptr<Game> game = make_shared<Game>(kGridWidth, kGridHeight, 3);
  //Game game(kGridWidth, kGridHeight, 3);
  game->is_player_turn = true;
  game->running = true;



  thread t(&Game::UpdateEnemies, game);
  game->Run(controller, renderer, kMsPerFrame);

  //thread t(&Game::UpdateEnemies, &game);
  //game.Run(controller, renderer, kMsPerFrame);
  // to make sure the second thread comes out  of wait and terminates
  game->is_player_turn = false;
  game->running = false;
  game->cv.notify_all();
  t.join();
  std::cout << "Game has terminated successfully!\n";
  std::cout << "Score: " << game->GetScore() << "\n";
  std::cout << "Size: " << game->GetSize() << "\n";
  return 0;
}