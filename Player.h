#pragma once
#include "include.h"
#include "Animation2D.h"

struct Player {
public:
  void init(Animation2D *animation);
  void update(Directx *dx, float t);
  void move();
public:
  PlayerState current_state;
  PlayerState previous_state;
  Animation2D *animation;
};