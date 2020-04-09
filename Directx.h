#pragma once
#include "include.h"

enum ShaderType {
  PIXEL,
  VERTEX
};

struct VertexShader {
  ID3D11VertexShader *vertex_shader;
  ID3D10Blob *vertex_shader_blob;
};

struct PixelShader {
  ID3D11PixelShader *pixel_shader;
  ID3D10Blob *pixel_shader_blob;
};

struct Shader {
  ShaderType type;
  union
  {
    VertexShader *vertex_shader;
    PixelShader *pixel_shader;
  };
};

struct ConstantBuffer {
  XMMATRIX world;
  XMMATRIX view;
  XMMATRIX proj;
  XMMATRIX world_view_proj;
  XMMATRIX world_inv_transpose;
  XMFLOAT3 eye_pos;
  float pad1;
};

struct Directx {
public:
  void on_init();
  void create_vertex_shader(
    LPCWSTR shaderName,
    LPCSTR shaderMainFunc);
  void create_pixel_shader(
    LPCWSTR shaderName,
    LPCSTR shaderMainFunc);

  void init_input_layout(const std::wstring &layout_name,
    D3D11_INPUT_ELEMENT_DESC vertex_desc[],
    UINT num_elements,
    VertexShader *vertex_shader);

  void create_index_buffer(const std::wstring &buffername, std::vector<int> &indices);
  void create_vertex_buffer(const std::wstring &buffername, std::vector<Vertex> &vertices);
  void create_constant_buffer(const std::wstring &buffername, const ConstantBuffer &cb);

  inline VertexShader *get_vertex_shader(const std::wstring &shader_name) {
    Shader *shader = shaders[shader_name];
    assert(shader->type == VERTEX);
    return shader->vertex_shader;
  }
  inline PixelShader *get_pixel_shader(const std::wstring &shader_name) {
    Shader *shader = shaders[shader_name];
    assert(shader->type == PIXEL);
    return shader->pixel_shader;
  }
  inline ID3D11Buffer *get_buffer(const std::wstring &buffername) {
    return buffers[buffername];
  }
  inline ID3D11Buffer **get_buffer_address(const std::wstring &buffername) {
    return &buffers[buffername];
  }
public:
  HINSTANCE app_instance;
  HWND main_window;
  D3D_DRIVER_TYPE driver_type = D3D_DRIVER_TYPE_NULL;
  D3D_FEATURE_LEVEL feature_level = D3D_FEATURE_LEVEL_11_0;
  ID3D11Device *device;
  ID3D11Device1 *device1;
  ID3D11DeviceContext *device_context;
  IDXGISwapChain *swap_chain;
  IDXGISwapChain1 *swap_chain1;
  ID3D11DeviceContext1 *device_context1;
  ID3D11Texture2D *back_buffer;
  ID3D11RenderTargetView *render_target_view;
  ID3D11Texture2D *depth_stencil;
  ID3D11DepthStencilView *depth_stencil_view;
  ID3D11SamplerState *sampler;
  ID3D11DepthStencilState *depth_stencil_state;
  ID3D11BlendState *blend_state;

  // Shaders
  std::unordered_map<std::wstring, Shader *> shaders;
  std::unordered_map<std::wstring, ID3D11InputLayout *> layouts;
  std::unordered_map<std::wstring, ID3D11Buffer *> buffers;
private:
  void init_window();
  void init_graphics();
};