#pragma once
#include "include.h"

struct ImageWindow {
  void init(const std::wstring &filename, int x, int y);
  void move(Direction direction);

  HWND window;
  HINSTANCE instance;
  UINT width;
  UINT height;
  int x;
  int y;
};
