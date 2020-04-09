#include "Directx.h"

LRESULT wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  switch (msg)
  {
  case WM_CLOSE:
    PostQuitMessage(0);
    break;
  }
  return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK main_wnd_proc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
  return wnd_proc(hwnd, msg, wParam, lParam);
}

BOOL CALLBACK enum_windows_proc(HWND hWnd, LPARAM lParam) {
  HWND window = FindWindowEx(hWnd, NULL, "SHELLDLL_DefView", NULL);
  HWND *ret = (HWND*)lParam;

  if (window) {
    *ret = FindWindowEx(NULL, hWnd, "WorkerW", NULL);
  }

  return TRUE;
}

BOOL initInstance(HINSTANCE hInstance, HWND parent, int nCmdShow)
{
  HWND hWnd = CreateWindowA("Main", "Main", WS_VISIBLE | WS_CHILD,
    CW_USEDEFAULT, 0, 900, 672, parent, nullptr, hInstance, nullptr);

  return TRUE;
}

void Directx::init_window() {
  // Create WorkerW
  HWND progman = FindWindowA("Progman", NULL);
  if (SendMessageTimeoutA(progman,
    0x052C,
    0,
    0,
    SMTO_NORMAL,
    1000,
    nullptr)) {
    OutputDebugStringA("WorkerW created succesfully");
  }

  EnumWindows(enum_windows_proc, (LPARAM)&main_window);

  if (!main_window) {
     assert("Error");
  }

  /*
  // Register class
  WNDCLASS wc;
  wc.style = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc = main_wnd_proc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = app_instance;
  wc.hIcon = LoadIcon(0, IDI_APPLICATION);
  wc.hCursor = LoadCursor(0, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.lpszMenuName = 0;
  wc.lpszClassName = "Main";

  if (!RegisterClass(&wc)) {
    OutputDebugString("cant register class");
    exit(EXIT_FAILURE);
  }

  // Window create
  RECT rc = { 0, 0, Global::Constant::WIDTH, Global::Constant::HEIGHT };
  AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);
  int width = rc.right - rc.left;
  int height = rc.bottom - rc.top;
  main_window = CreateWindow(
    "Main",
    "Main",
    WS_OVERLAPPEDWINDOW,
    CW_USEDEFAULT,
    CW_USEDEFAULT,
    width,
    height,
    0,
    0,
    app_instance,
    0);

  if (!main_window)
  {
    __debugbreak();
  }
  */

  ShowWindow(main_window, SW_SHOW);
  UpdateWindow(main_window);
}

void Directx::init_graphics() {
  {
    D3D_DRIVER_TYPE driverTypes[] =
    {
      D3D_DRIVER_TYPE_HARDWARE,
      D3D_DRIVER_TYPE_WARP,
      D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
      D3D_FEATURE_LEVEL_11_1,
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    HRESULT hr;
    // Device
    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
      driver_type = driverTypes[driverTypeIndex];
      hr = D3D11CreateDevice(nullptr, driver_type, nullptr, D3D11_CREATE_DEVICE_DEBUG, featureLevels, numFeatureLevels,
        D3D11_SDK_VERSION, &device, &feature_level, &device_context);

      if (hr == E_INVALIDARG)
      {
        // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
        hr = D3D11CreateDevice(nullptr, driver_type, nullptr, 0, &featureLevels[1], numFeatureLevels - 1,
          D3D11_SDK_VERSION, &device, &feature_level, &device_context);
      }

      if (SUCCEEDED(hr))
        break;
    }
    DX::ThrowIfFailed(hr);

    // Factory
    IDXGIFactory1* dxgiFactory = nullptr;
    {
      IDXGIDevice* dxgiDevice = nullptr;
      hr = device->QueryInterface(IID_PPV_ARGS(&dxgiDevice));
      if (SUCCEEDED(hr))
      {
        IDXGIAdapter* adapter = nullptr;
        hr = dxgiDevice->GetAdapter(&adapter);
        if (SUCCEEDED(hr))
        {
          hr = adapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
          adapter->Release();
        }
        dxgiDevice->Release();
      }
    }
    DX::ThrowIfFailed(hr);

    // Swap chain
    IDXGIFactory2 *dxgiFactory2 = nullptr;
    hr = dxgiFactory->QueryInterface(IID_PPV_ARGS(&dxgiFactory2));
    if (dxgiFactory2)
    {
      // DirectX 11.1 or later
      hr = device->QueryInterface(IID_PPV_ARGS(&device1));
      if (SUCCEEDED(hr))
      {
        (void)device_context->QueryInterface(IID_PPV_ARGS(&device_context1));
      }

      DXGI_SWAP_CHAIN_DESC1 sd; // TODO: MAKE {}
      ZeroMemory(&sd, sizeof(sd));
      sd.Width = Global::Constant::WIDTH;
      sd.Height = Global::Constant::HEIGHT;
      sd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
      sd.SampleDesc.Count = 1;
      sd.SampleDesc.Quality = 0;
      sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      sd.BufferCount = 1;

      hr = dxgiFactory2->CreateSwapChainForHwnd(device, main_window, &sd, nullptr, nullptr, &swap_chain1);
      if (SUCCEEDED(hr))
      {
        hr = swap_chain1->QueryInterface(IID_PPV_ARGS(&swap_chain));
      }

      dxgiFactory2->Release();
    }
    else
    {
      // DirectX 11.0 systems
      DXGI_SWAP_CHAIN_DESC sd;
      ZeroMemory(&sd, sizeof(sd));
      sd.BufferCount = 1;
      sd.BufferDesc.Width = Global::Constant::WIDTH;
      sd.BufferDesc.Height = Global::Constant::HEIGHT;
      sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
      sd.BufferDesc.RefreshRate.Numerator = 60;
      sd.BufferDesc.RefreshRate.Denominator = 1;
      sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
      sd.OutputWindow = main_window;
      sd.SampleDesc.Count = 1;
      sd.SampleDesc.Quality = 0;
      sd.Windowed = TRUE;

      hr = dxgiFactory->CreateSwapChain(device, &sd, &swap_chain);
    }

    dxgiFactory->MakeWindowAssociation(main_window, DXGI_MWA_NO_ALT_ENTER);
    dxgiFactory->Release();

    DX::ThrowIfFailed(hr);

    // Viewport
    D3D11_VIEWPORT vp;
    vp.Width = (float)Global::Constant::WIDTH;
    vp.Height = (float)Global::Constant::HEIGHT;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    device_context->RSSetViewports(1, &vp);
  }

  // Render target view
  DX::ThrowIfFailed(swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer)));
  DX::ThrowIfFailed(device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view));

  // Depth stencil view
  D3D11_TEXTURE2D_DESC desc_depth = {};
  desc_depth.Width = Global::Constant::WIDTH;
  desc_depth.Height = Global::Constant::HEIGHT;
  desc_depth.MipLevels = 1;
  desc_depth.ArraySize = 1;
  desc_depth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  desc_depth.SampleDesc.Count = 1;
  desc_depth.SampleDesc.Quality = 0;
  desc_depth.Usage = D3D11_USAGE_DEFAULT;
  desc_depth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
  desc_depth.CPUAccessFlags = 0;
  desc_depth.MiscFlags = 0;

  DX::ThrowIfFailed(device->CreateTexture2D(&desc_depth, nullptr, &depth_stencil));
  DX::ThrowIfFailed(device->CreateDepthStencilView(depth_stencil, nullptr, &depth_stencil_view));

  // Create depth stencil state
  D3D11_DEPTH_STENCIL_DESC dss_desc = {};
  dss_desc.DepthEnable = true;
  dss_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  dss_desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
  dss_desc.StencilEnable = true;
  dss_desc.StencilReadMask = 0xFF;
  dss_desc.StencilWriteMask = 0xFF;

  dss_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  dss_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
  dss_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  dss_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  dss_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  dss_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
  dss_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  dss_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

  DX::ThrowIfFailed(device->CreateDepthStencilState(&dss_desc, &depth_stencil_state));

  // Create blend state
  D3D11_BLEND_DESC bs_desc = {};
  bs_desc.RenderTarget[0].BlendEnable = TRUE;
  bs_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
  bs_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;

  bs_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
  bs_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_INV_DEST_ALPHA;
  bs_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
  bs_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
  bs_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

  DX::ThrowIfFailed(device->CreateBlendState(&bs_desc, &blend_state));

  // Sampler state
  D3D11_SAMPLER_DESC sd = {};
  sd.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  sd.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  sd.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
  sd.MinLOD = 0;
  sd.MaxLOD = D3D11_FLOAT32_MAX;
  DX::ThrowIfFailed(device->CreateSamplerState(&sd, &sampler));

  // Config
}

void Directx::on_init() {
  init_window();
  init_graphics();
}

void Directx::create_vertex_shader(
  LPCWSTR shader_name,
  LPCSTR shaderMainFunc) {
  ID3D10Blob *compilationMsgs = nullptr;

  ID3D11VertexShader *vertex_shader = nullptr;
  ID3D10Blob *shader_blob = nullptr;

  DX::ThrowIfFailed(D3DCompileFromFile(shader_name, 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, shaderMainFunc, "vs_5_0", D3DCOMPILE_DEBUG, 0, &shader_blob, &compilationMsgs));
  if (compilationMsgs != 0)
  {
    MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
  }
  DX::ThrowIfFailed(device->CreateVertexShader((shader_blob)->GetBufferPointer(), (shader_blob)->GetBufferSize(), NULL, &vertex_shader));

  shaders[shader_name] = new Shader{ VERTEX, new VertexShader {vertex_shader, shader_blob} };
}

void Directx::create_pixel_shader(
  LPCWSTR shader_name,
  LPCSTR shaderMainFunc) {
  ID3D10Blob *compilationMsgs = nullptr;
  ID3D11PixelShader *pixel_shader = nullptr;
  ID3D10Blob *shader_blob = nullptr;
  DX::ThrowIfFailed(D3DCompileFromFile(shader_name, 0, D3D_COMPILE_STANDARD_FILE_INCLUDE, shaderMainFunc, "ps_5_0", D3DCOMPILE_DEBUG, 0, &shader_blob, &compilationMsgs));
  if (compilationMsgs != 0)
  {
    MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
  }
  DX::ThrowIfFailed(device->CreatePixelShader((shader_blob)->GetBufferPointer(), (shader_blob)->GetBufferSize(), NULL, &pixel_shader));

  shaders[shader_name] = new Shader{ PIXEL, reinterpret_cast<VertexShader *>(new PixelShader {pixel_shader, shader_blob}) };
}

void Directx::init_input_layout(const std::wstring &layout_name,
  D3D11_INPUT_ELEMENT_DESC vertex_desc[],
  UINT num_elements,
  VertexShader *vertex_shader) {
  ID3D11InputLayout *input_layout = nullptr;

  ID3D10Blob *shader_blob = vertex_shader->vertex_shader_blob;
  DX::ThrowIfFailed(device->CreateInputLayout(
    vertex_desc,
    num_elements,
    shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(),
    &input_layout
  ));

  layouts[layout_name] = input_layout;
}


void Directx::create_index_buffer(const std::wstring &buffername, std::vector<int> &indices) {
  ID3D11Buffer *index_buffer = nullptr;

  D3D11_BUFFER_DESC bd = {};
  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.ByteWidth = sizeof(int) * indices.size();
  bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
  bd.CPUAccessFlags = 0;
  bd.MiscFlags = 0;

  D3D11_SUBRESOURCE_DATA init_data = {};
  init_data.pSysMem = &indices[0];

  DX::ThrowIfFailed(device->CreateBuffer(&bd, &init_data, &index_buffer));

  buffers[buffername] = index_buffer;
}

void Directx::create_vertex_buffer(const std::wstring &buffername, std::vector<Vertex> &vertices) {
  ID3D11Buffer *vertex_buffer = nullptr;

  D3D11_BUFFER_DESC bd = {};
  bd.Usage = D3D11_USAGE_DYNAMIC;
  bd.ByteWidth = sizeof(Vertex) * vertices.size();
  bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  bd.MiscFlags = 0;

  D3D11_SUBRESOURCE_DATA init_data = {};
  init_data.pSysMem = &vertices[0];

  DX::ThrowIfFailed(device->CreateBuffer(&bd, &init_data, &vertex_buffer));

  buffers[buffername] = vertex_buffer;
}

void Directx::create_constant_buffer(const std::wstring &buffername, const ConstantBuffer &cb) {
  ID3D11Buffer *constant_buffer = nullptr;

  D3D11_BUFFER_DESC bd = {};
  bd.Usage = D3D11_USAGE_DEFAULT;
  bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bd.ByteWidth = sizeof(ConstantBuffer);
  bd.CPUAccessFlags = 0;

  D3D11_SUBRESOURCE_DATA init_data = {};
  init_data.pSysMem = &cb;
  DX::ThrowIfFailed(device->CreateBuffer(&bd, &init_data, &constant_buffer));

  buffers[buffername] = constant_buffer;
}