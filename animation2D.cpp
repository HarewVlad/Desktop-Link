#include "Animation2D.h"

void Animation2D::init(Directx *dx, Geometry *geometry, const std::wstring &filename, AnimationType type) {
  this->type = type;
  this->geometry = geometry;

  DX::ThrowIfFailed(CreateDDSTextureFromFile(dx->device, filename.c_str(), nullptr, &texture_resource_view));
}

void Animation2D::set_start_frame(float u[4], float v[4], int num_frames, Direction direction) {
  FramesInfo *frame_info = new FramesInfo{};
  memcpy(frame_info->start_u, u, sizeof(float) * 4);
  memcpy(frame_info->start_v, v, sizeof(float) * 4);
  frame_info->num_frames = num_frames;

  frames[direction] = frame_info;
}

void Animation2D::update(Directx *dx, Direction current_direction, Direction previous_direction, float t) {
  if (type == STATIC) {
    return;
  }

  // Update frames
  {
    FramesInfo *frames_info_current = frames[current_direction];
    frames_info_current->current_frame = (frames_info_current->current_frame + 1) % frames_info_current->num_frames;

    float u[4] = {};
    memcpy(u, frames_info_current->start_u, sizeof(float) * 4);
    u[2] *= (frames_info_current->current_frame + 1);
    u[3] *= (frames_info_current->current_frame + 1);
    u[0] = u[2] - frames_info_current->start_u[2];
    u[1] = u[0];
    
    geometry->set_uv(u, frames_info_current->start_v);

    if (previous_direction != current_direction) {
      FramesInfo *frames_info_previous = frames[previous_direction];
      frames_info_previous->current_frame = 0;
    }
  }

  // Update geometry
  {
    geometry->update(dx);
    geometry->print();
  }
}