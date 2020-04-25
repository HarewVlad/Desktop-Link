#pragma once
#include "include.h"
#include "Animation2D.h"
#include "ImageWindow.h"

struct Player {
public:
  void init(Animation2D *animation);
  void update(Directx *dx, float t);
  void move(Direction direction);
  void set_direction(Direction direction);
  void auto_move();
public:
  Direction current_direction;
  Direction previous_direction;
  PlayerState state;
  Animation2D *animation;
  ImageWindow *image_window;
};