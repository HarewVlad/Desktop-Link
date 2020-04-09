#include "Player.h"

void Player::init(Animation2D *animation) {
  this->animation = animation;
  this->current_state = STAND;
  this->previous_state = STAND;
}

void Player::update(Directx *dx, float t) {
  animation->update(dx, current_state, previous_state, t);
}

void Player::move() {
  switch (current_state) {
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
  case STAND:
    break;
  default:
    assert(0);
    break;
  }
}