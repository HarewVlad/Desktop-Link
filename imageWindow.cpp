#include "ImageWindow.h"

LRESULT image_wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
  case WM_CLOSE:
    DestroyWindow(hwnd);
    break;
  default:
    return DefWindowProc(hwnd, msg, wParam, lParam);
    break;
  }
}

LRESULT CALLBACK main_image_wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  return image_wnd_proc(hwnd, msg, wParam, lParam);
}

void ImageWindow::init(const std::wstring &filename, int x, int y) {
  static bool isInited = false;

  if (!isInited) {
    WNDCLASS wc;
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = main_image_wnd_proc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = instance;
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName = 0;
    wc.lpszClassName = "Image";

    if (!RegisterClass(&wc)) {
      OutputDebugString("can't create image class");
      exit(EXIT_FAILURE);
    }

    isInited = true;
  }

  this->x = x;
  this->y = y;

  RECT rc = { 0, 0, 100, 100 }; // TODO: make size of an image
  AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);
  int width = rc.right - rc.left;
  int height = rc.bottom - rc.top;
  window = CreateWindow(
    "Image",
    "Image",
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    width,
    height,
    0,
    0,
    instance,
    0);

  if (!window)
  {
    __debugbreak();
  }

  MoveWindow(window, 0, 0, width, height, TRUE);
  ShowWindow(window, SW_SHOW);
  UpdateWindow(window);
}

void ImageWindow::move(Direction direction) {
  switch (direction) {
  case RIGHT:
    x += Global::Player::SPEED;
    MoveWindow(window, x, y, width, height, TRUE);
    break;
  default:
    assert(0);
    break;
  }
}