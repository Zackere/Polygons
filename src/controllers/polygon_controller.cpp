// Copyright Wojciech Replin 2019

#include "polygon_controller.hpp"

#include "../drawable_objects/point.hpp"

namespace gk {
bool PolygonController::OnMouseLButtonDown(DrawingBoard* board,
                                           POINT mouse_pos) {
  bool ret = false;
  if (state_ == State::FREE) {
    for (auto& obj : objects_)
      ret = ret || obj->OnMouseLButtonDown(board, mouse_pos);
  }
  return ret;
}

bool PolygonController::OnMouseLButtonUp(DrawingBoard* board, POINT mouse_pos) {
  bool ret = false;
  if (state_ == State::FREE) {
    for (auto& obj : objects_)
      ret = ret || obj->OnMouseLButtonUp(board, mouse_pos);
  }
  return ret;
}

bool PolygonController::OnMouseLButtonDoubleClick(DrawingBoard* board,
                                                  POINT mouse_pos) {
  switch (state_) {
    case gk::PolygonController::State::CREATE_POINT:
      objects_.insert(std::make_unique<Point>(
          std::make_pair(mouse_pos.x, mouse_pos.y), RGB(255, 0, 0)));
      return true;
    case gk::PolygonController::State::CREATE_LINE:
      break;
    case gk::PolygonController::State::CREATE_POLYGON:
      break;
  }
  return false;
}

bool PolygonController::OnMouseMove(DrawingBoard* board, POINT mouse_pos) {
  bool ret = false;
  if (state_ == State::FREE) {
    for (auto& obj : objects_)
      ret = ret || obj->OnMouseMove(board, mouse_pos);
  }
  return ret;
}

bool PolygonController::OnKeyDown(DrawingBoard* board,
                                  WPARAM key_code,
                                  bool was_down) {
  return false;
}

bool PolygonController::OnKeyUp(DrawingBoard* board, WPARAM key_code) {
  switch (key_code) {
    case 'Q':
      state_ = State::FREE;
      break;
    case 'W':
      state_ = State::CREATE_POINT;
      break;
    case 'E':
      state_ = State::CREATE_LINE;
      break;
    case 'R':
      state_ = State::CREATE_POLYGON;
      break;
  }
  return false;
}

void PolygonController::Draw(DrawingBoard* board) {
  for (auto& obj : objects_)
    obj->Display(board);
}
}  // namespace gk
