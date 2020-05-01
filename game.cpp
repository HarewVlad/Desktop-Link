#include "include.h"
#include "Game.h"

void Game::on_init() {
  dx = new Directx{};
  dx->on_init();

  // Shaders
  dx->create_vertex_shader(L"vs.hlsl", "VS");
  dx->create_pixel_shader(L"ps.hlsl", "PS");

  // Input layouts
  D3D11_INPUT_ELEMENT_DESC vertex_desc[] =
  {
    { "position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "normal", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "texcoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  };
  dx->init_input_layout(L"main_layout", vertex_desc, ARRAYSIZE(vertex_desc), dx->get_vertex_shader(L"vs.hlsl"));

  // Background
  background = new Animation2D{};
  Geometry *background_geometry = new Geometry{};
  background_geometry->init(dx,
                   -Global::Constant::WIDTH / 2.0f, -Global::Constant::HEIGHT / 2,
                    Global::Constant::WIDTH, Global::Constant::HEIGHT);
  background->init(dx, background_geometry, L"hdi.dds", STATIC);

  // Player
  Geometry *player_geometry = new Geometry{};
  player_geometry->init(dx, 0, 0, Global::Player::PLAYER_WIDTH, Global::Player::PLAYER_HEIGHT);
  Animation2D *player_animation = new Animation2D{};
  player_animation->init(dx, player_geometry, L"link.dds", DYNAMIC);
  // Set start {u, v} for STAND, UP, DOWN, LEFT, RIGHT
  // STAND
  {
    float u[] = { 0, 0, 0.1f, 0.1f };
    float v[] = { 0.125f, 0, 0, 0.125f };
    player_animation->set_start_frame(u, v, 3, IDLE); // FOR STAND
  }
  // LEFT
  {
    float u[] = { 0, 0, 0.1f, 0.1f };
    float v[] = { 0.75f, 0.625f, 0.625f, 0.75f };
    player_animation->set_start_frame(u, v, 10, LEFT);
  }
  // RIGHT
  {
    float u[] = { 0, 0, 0.1f, 0.1f };
    float v[] = { 1.0f, 0.875f, 0.875f, 1.0f };
    player_animation->set_start_frame(u, v, 10, RIGHT);
  }
  // UP
  {
    float u[] = { 0, 0, 0.1f, 0.1f };
    float v[] = { 0.875f, 0.75f, 0.75f, 0.875f };
    player_animation->set_start_frame(u, v, 10, UP);
  }
  // DOWN
  {
    float u[] = { 0, 0, 0.1f, 0.1f };
    float v[] = { 0.625f, 0.5f, 0.5f, 0.625f };
    player_animation->set_start_frame(u, v, 10, DOWN);
  }
  
  player = new Player{};
  player->init(player_animation);

  // Buffers
  XMMATRIX world = XMMatrixIdentity();
  XMMATRIX view = XMMatrixLookAtLH(XMVECTOR{ 0, 0, -1 }, XMVECTOR{ 0, 0, 0 }, XMVECTOR{ 0, 1, 0 });
  XMMATRIX proj = XMMatrixOrthographicLH(Global::Constant::WIDTH, Global::Constant::HEIGHT, 0, 2);

  ConstantBuffer cb = {};
  cb.world = XMMatrixTranspose(world);
  cb.view = XMMatrixTranspose(view);
  cb.proj = XMMatrixTranspose(proj);
  cb.world_view_proj = XMMatrixTranspose(world * view * proj);
  cb.eye_pos = XMFLOAT3{ 0, 0, -1};
  dx->create_constant_buffer(L"main_constant_buffer", cb);
}

void Game::on_run() {
  MSG msg = {};

  float update_timer = 0;
  float frame_time = 1 / 24.0f;

  Time::TimePoint start_time = Time::timeGet();
  Time::TimePoint last_time = Time::timeGet();

  while (msg.message != WM_QUIT)
  {
    Time::TimePoint current_time = Time::timeGet();
    float passed_time = Time::getDuration(current_time, last_time);

    update_timer += passed_time;

    bool should_render = false;
    while (update_timer >= frame_time)
    {
      on_input(frame_time);
      on_update(frame_time);
      update_timer -= frame_time;
      should_render = true;
    }

    if (should_render)
    {
      on_render();
    }

    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
    else
    {
      std::this_thread::sleep_for(std::chrono::microseconds(1));
    }

    last_time = current_time;
  }
}

void Game::on_render() {
  dx->device_context->ClearRenderTargetView(dx->render_target_view, Colors::Gray);
  dx->device_context->ClearDepthStencilView(dx->depth_stencil_view, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

  dx->device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  dx->device_context->IASetInputLayout(dx->layouts[L"main_layout"]);

  dx->device_context->OMSetDepthStencilState(dx->depth_stencil_state, 0);
  float blend_factor[] = { 0, 0, 0, 0 };
  dx->device_context->OMSetBlendState(dx->blend_state, blend_factor, 0xffffffff);
  dx->device_context->OMSetRenderTargets(1, &dx->render_target_view, dx->depth_stencil_view);

  dx->device_context->VSSetShader(dx->get_vertex_shader(L"vs.hlsl")->vertex_shader, nullptr, 0);
  dx->device_context->VSSetConstantBuffers(0, 1, dx->get_buffer_address(L"main_constant_buffer"));
  dx->device_context->PSSetShader(dx->get_pixel_shader(L"ps.hlsl")->pixel_shader, nullptr, 0);
  dx->device_context->PSSetSamplers(0, 1, &dx->sampler);

  // Background
  ID3D11Buffer *vertex_buffer = background->geometry->vertex_buffer;
  ID3D11Buffer *index_buffer = background->geometry->index_buffer;
  UINT num_indices = background->geometry->indices.size();

  UINT stride = sizeof(Vertex);
  UINT offset = 0;

  dx->device_context->PSSetShaderResources(0, 1, &background->texture_resource_view);
  dx->device_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
  dx->device_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
  dx->device_context->DrawIndexed(num_indices, 0, 0);

  // Player
  vertex_buffer = player->animation->geometry->vertex_buffer;
  index_buffer = player->animation->geometry->index_buffer;

  dx->device_context->PSSetShaderResources(0, 1, &player->animation->texture_resource_view);
  dx->device_context->IASetVertexBuffers(0, 1, &vertex_buffer, &stride, &offset);
  dx->device_context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
  dx->device_context->DrawIndexed(num_indices, 0, 0);

  DX::ThrowIfFailed(dx->swap_chain->Present(0, 0));
}

void Game::on_update(float t) {
  player->update(dx, t);
}

void Game::on_input(float t) {
#ifdef MANUAL_CONTROL
  if (GetAsyncKeyState(VK_UP)) {
    player->previous_state = player->current_state;
    player->current_state = UP;
  } else if (GetAsyncKeyState(VK_DOWN)) {
    player->previous_state = player->current_state;
    player->current_state = DOWN;
  } else if (GetAsyncKeyState(VK_LEFT)) {
    player->previous_state = player->current_state;
    player->current_state = LEFT;
  } else if (GetAsyncKeyState(VK_RIGHT)) {
    player->previous_state = player->current_state;
    player->current_state = RIGHT;
  } else {
    player->previous_state = player->current_state;
    player->current_state = STAND;
  }

  player->move();
#endif

#ifdef AUTO_CONTROL

  if (GetAsyncKeyState(VK_UP) & 0x8000) {
    player->state = ACTION_OPEN_WINDOW;
  }

  player->auto_move();
#endif
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
  srand(time(NULL));
  Game *game = new Game;
  game->on_init();
  game->on_run();

  return 0;
}