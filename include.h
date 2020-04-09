#pragma once
#include <iostream>
#include <Windows.h>
#include <thread>
#include <chrono>
#include <map>
#include <unordered_map>
#include <string>

#include <d3d11_1.h>
#include <dxgi1_2.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <d3dcompiler.h>
#include <DDSTextureLoader.h>

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
      SPEED = 20
    };
  }
}

enum PlayerState {
  STAND,
  RIGHT,
  LEFT,
  UP,
  DOWN
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