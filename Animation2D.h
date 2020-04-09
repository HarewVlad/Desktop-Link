#pragma once
#include "include.h"
#include "Directx.h"
#include "Geometry.h"

enum AnimationType {
  STATIC,
  DYNAMIC
};

struct FramesInfo {
  // Start frame
  float start_u[4];
  float start_v[4];
  int current_frame;
  int num_frames;
};

struct Animation2D {
public:
  void init(Directx *dx, Geometry *geometry, const std::wstring &filename, AnimationType type);
  void set_start_frame(float u[4], float v[4], int num_frames, PlayerState type);
  void update(Directx *dx, PlayerState current_state, PlayerState previous_state, float t);
public:
  AnimationType type;
  Geometry *geometry;
  ID3D11ShaderResourceView *texture_resource_view;
  std::unordered_map<PlayerState, FramesInfo *> frames;
};