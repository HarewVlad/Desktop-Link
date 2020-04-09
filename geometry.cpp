#include "Geometry.h"

void Geometry::init(Directx *dx, float x, float y, float w, float h) {
  vertices.push_back(Vertex{ x, y, 0, -1, 0, 0, 0, 1 });
  vertices.push_back(Vertex{ x, y + h, 0, 0, -1, 0, 0, 0 });
  vertices.push_back(Vertex{ x + w, y + h, 0, 1, 0, 0, 1, 0 });
  vertices.push_back(Vertex{ x + w, y, 0, 0, 1, 0, 1, 1 });

  indices = { 0, 1, 2, 0, 2, 3 };

  D3D11_BUFFER_DESC bd = {};
  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.ByteWidth = sizeof(int) * indices.size();
  bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
  bd.CPUAccessFlags = 0;
  bd.MiscFlags = 0;

  D3D11_SUBRESOURCE_DATA init_data = {};
  init_data.pSysMem = &indices[0];

  DX::ThrowIfFailed(dx->device->CreateBuffer(&bd, &init_data, &index_buffer));

  bd.Usage = D3D11_USAGE_DYNAMIC;
  bd.ByteWidth = sizeof(Vertex) * vertices.size();
  bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  bd.MiscFlags = 0;

  init_data.pSysMem = &vertices[0];

  DX::ThrowIfFailed(dx->device->CreateBuffer(&bd, &init_data, &vertex_buffer));
}

void Geometry::update_uv(Directx *dx) {
  D3D11_MAPPED_SUBRESOURCE resource;
  dx->device_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
  memcpy(resource.pData, &vertices[0], sizeof(Vertex) * vertices.size());
  dx->device_context->Unmap(vertex_buffer, 0);
}

void Geometry::update_pos(Directx *dx) {
  D3D11_MAPPED_SUBRESOURCE resource;
  dx->device_context->Map(vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
  memcpy(resource.pData, &vertices[0], sizeof(Vertex) * vertices.size());
  dx->device_context->Unmap(vertex_buffer, 0);
}

void Geometry::scale(XMFLOAT3 scaler) {
  vertices[0].pos.x += scaler.x;
  vertices[0].pos.y += scaler.y;
  vertices[1].pos.x += scaler.x;
  vertices[1].pos.y += scaler.y;
  vertices[2].pos.x += scaler.x;
  vertices[2].pos.y += scaler.y;
  vertices[3].pos.x += scaler.x;
  vertices[3].pos.y += scaler.y;
}

void Geometry::set_uv(float u[4], float v[4]) {
  vertices[0].tex = { u[0], v[0] };
  vertices[1].tex = { u[1], v[1] };
  vertices[2].tex = { u[2], v[2] };
  vertices[3].tex = { u[3], v[3] };
}