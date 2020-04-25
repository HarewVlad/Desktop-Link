#include "Player.h"

void Player::init(Animation2D *animation) {
  this->animation = animation;
  this->state = ACTION_NONE;
  this->current_direction = IDLE;
  this->previous_direction = IDLE;
}

void Player::update(Directx *dx, float t) {
  animation->update(dx, current_direction, previous_direction, t);
}

void Player::move(Direction direction) {
  switch (direction) {
  case UP:
    animation->geometry->scale(XMFLOAT3{ 0, Global::Player::SPEED, 0 });
    break;
  case DOWN:
    animation->geometry->scale(XMFLOAT3{ 0, -Global::Player::SPEED, 0 });
    break;
  case LEFT:
    animation->geometry->scale(XMFLOAT3{ -Global::Player::SPEED, 0, 0 });
    break;
  case RIGHT:
    animation->geometry->scale(XMFLOAT3{ Global::Player::SPEED, 0, 0 });
    break;
  case IDLE:
    break;
  default:
    assert(0);
    break;
  }
}

void Player::auto_move() {
  static int move_count = 0;
  static bool isWindowOpened = false;
  
  XMFLOAT3 player_pos_left_bottom = animation->geometry->vertices[1].pos;
  XMFLOAT3 player_pos_right_top = animation->geometry->vertices[2].pos;

  if (state == ACTION_OPEN_WINDOW) {
    if (!isWindowOpened && player_pos_left_bottom.x <= -Global::Constant::WIDTH / 2) {
      // Open window
      image_window = new ImageWindow{};
      image_window->init(L"ho", player_pos_left_bottom.x + Global::Constant::WIDTH / 2, player_pos_left_bottom.y + Global::Constant::HEIGHT / 2);
      // Window should contain image
      // That change state
      isWindowOpened = true;
    } else if (isWindowOpened) {
      // Move to right with window
      image_window->move(RIGHT);
      set_direction(RIGHT);
    } else {
      // Move to left to open window
      set_direction(LEFT);
    }
  } else {
    if (move_count == Global::Player::AUTO_MOVE_TIME) {
      set_direction(static_cast<Direction>(rand() % (sizeof(PlayerState) + 1)));
      move_count = 0;
    }

    // Check bounds
    switch (current_direction) {
    case LEFT:
      if (player_pos_left_bottom.x <= -Global::Constant::WIDTH / 2) {
        set_direction(RIGHT);
      }
      break;
    case RIGHT:
      if (player_pos_right_top.x >= Global::Constant::WIDTH / 2) {
        set_direction(LEFT);
      }
      break;
    case UP:
      if (player_pos_right_top.y >= Global::Constant::HEIGHT / 2) {
        set_direction(DOWN);
      }
      break;
    case DOWN:
      if (player_pos_left_bottom.y <= -Global::Constant::HEIGHT / 2) {
        set_direction(UP);
      }
      break;
    case IDLE:
      break;
    default:
      assert(0);
      break;
    }

    // Move
    move_count++;
  }
  
  move(current_direction);
}

void Player::set_direction(Direction direction) {
  previous_direction = current_direction;
  current_direction = direction;
}