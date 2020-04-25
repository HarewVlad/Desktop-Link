#pragma once
#include "include.h"
#include "Directx.h"
#include "Utils.h"
#include "Animation2D.h"
#include "Geometry.h"
#include "Player.h"

struct Game {
public:
  void on_run();
  void on_init();
  void on_render();
  void on_update(float t);
  void on_input(float t);
private:
  Directx *dx;
  Animation2D *background;
  Player *player;
};