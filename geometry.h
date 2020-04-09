#pragma once
#include "include.h"
#include "Directx.h"

struct Geometry {
public:
  void init(Directx *dx, float x, float y, float w, float h);
  void update_uv(Directx *dx);
  void update_pos(Directx *dx);
  void set_uv(float u[4], float v[4]);
  void scale(XMFLOAT3 scaler);
public:
  ID3D11Buffer *index_buffer;
  ID3D11Buffer *vertex_buffer;
  std::vector<Vertex> vertices;
  std::vector<int> indices;
};