#include "ImageWindow.h"

static HBITMAP image;
static BITMAP image_info;

LRESULT image_wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
  case WM_PAINT:
    PAINTSTRUCT     ps;
    HDC             hdc;
    BITMAP          bitmap;
    HDC             hdc_mem;
    HGDIOBJ         old_bitmap;

    hdc = BeginPaint(hwnd, &ps);

    hdc_mem = CreateCompatibleDC(hdc);
    old_bitmap = SelectObject(hdc_mem, image);

    GetObject(image, sizeof(bitmap), &bitmap);
    BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdc_mem, 0, 0, SRCCOPY);

    SelectObject(hdc_mem, old_bitmap);
    DeleteDC(hdc_mem);

    EndPaint(hwnd, &ps);
    break;
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

  image = (HBITMAP)LoadImageW(instance, filename.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
  GetObject(image, sizeof(BITMAP), &image_info);

  RECT rc = { 0, 0, image_info.bmWidth, image_info.bmHeight }; // TODO: make size of an image
  AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

  this->width = rc.right - rc.left;
  this->height = rc.bottom - rc.top;

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
    (LPVOID)filename.c_str());

  if (!window)
  {
    __debugbreak();
  }

  // Move window to prevent flickering
  MoveWindow(window, x - width, y, width, height, TRUE);
  ShowWindow(window, SW_SHOW);
  UpdateWindow(window);
}

void ImageWindow::move(Direction direction) {
  switch (direction) {
  case RIGHT:
    x += Global::Player::SPEED;
    MoveWindow(window, x - width, y, width, height, TRUE);
    break;
  default:
    assert(0);
    break;
  }
}