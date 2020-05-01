#pragma once
#include <iostream>
#include <Windows.h>
#include <thread>
#include <chrono>
#include <map>
#include <unordered_map>
#include <string>
#include <random>

#include <d3d11_1.h>
#include <dxgi1_2.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <d3dcompiler.h>
#include <DDSTextureLoader.h>

// #define MANUAL_CONTROL
#define AUTO_CONTROL

using namespace DirectX;

namespace DX
{
  inline void ThrowIfFailed(HRESULT hr)
  {
    if (FAILED(hr))
    {
      // Set a breakpoint on this line to catch DirectX API errors
      throw std::exception();
    }
  }
}

namespace Global
{
  namespace Constant
  {
    enum
    {
      WIDTH = 2560,
      HEIGHT = 1440
    };
  }
  namespace Player
  {
    enum
    {
      SPEED = 15,
      AUTO_MOVE_TIME = 15,
      AUTO_OPEN_WINDOW_TIME = 300,
      PLAYER_WIDTH = 100,
      PLAYER_HEIGHT = 100
    };
  }
}

enum Direction {
  IDLE,
  RIGHT,
  LEFT,
  UP,
  DOWN,
};

enum PlayerState {
  ACTION_NONE,
  ACTION_OPEN_WINDOW,
};

struct Vertex
{
  XMFLOAT3 pos;
  XMFLOAT3 normal;
  XMFLOAT2 tex;

  Vertex() {};
  Vertex(float px, float py, float pz, float nx, float ny, float nz, float u, float v)
  {
    pos = { px, py, pz };
    normal = { nx, ny, nz };
    tex = { u, v };
  }
};