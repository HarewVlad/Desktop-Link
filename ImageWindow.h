#pragma once
#include "include.h"

struct ImageWindow {
  void init(const std::wstring &filename, int x, int y);
  void move(Direction direction);

  HWND window;
  HINSTANCE instance;
  int width;
  int height;
  int x;
  int y;
};
