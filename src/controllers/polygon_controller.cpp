// Copyright Wojciech Replin 2019

#include "polygon_controller.hpp"

#include "../drawable_objects/line.hpp"
#include "../drawable_objects/point.hpp"

namespace gk {
PolygonController::~PolygonController() = default;

bool PolygonController::OnMouseLButtonDown(DrawingBoard* board,
    DrawingBoard::CoordinatePair mouse_pos) {
  bool ret = false;
  if (state_ == State::FREE) {
    for (auto& obj : objects_)
      ret = obj->OnMouseLButtonDown(board, mouse_pos) || ret;
  }
  return ret;
}

bool PolygonController::OnMouseLButtonUp(
    DrawingBoard* board,
    DrawingBoard::CoordinatePair mouse_pos) {
  bool ret = false;
  if (state_ == State::FREE) {
    for (auto& obj : objects_)
      ret = obj->OnMouseLButtonUp(board, mouse_pos) || ret;
  }
  return ret;
}

bool PolygonController::OnMouseLButtonDoubleClick(DrawingBoard* board,
    DrawingBoard::CoordinatePair mouse_pos) {
  switch (state_) {
    case gk::PolygonController::State::CREATE_POINT:
      objects_.insert(std::make_unique<Point>(
          std::make_pair(mouse_pos.first, mouse_pos.second), RGB(255, 0, 0)));
      return true;
    case gk::PolygonController::State::CREATE_LINE:
      if (last_click_.has_value()) {
        objects_.insert(std::make_unique<Line>(
            last_click_.value(), Line::Vertex{mouse_pos.first, mouse_pos.second},
            RGB(0, 255, 0), RGB(255, 0, 0)));
        last_click_.reset();
        return true;
      } else {
        last_click_.emplace(mouse_pos.first, mouse_pos.second);
        return false;
      }
    case gk::PolygonController::State::CREATE_POLYGON:
      break;
  }
  return false;
}

bool PolygonController::OnMouseMove(DrawingBoard* board,
                                    DrawingBoard::CoordinatePair mouse_pos) {
  bool ret = false;
  if (state_ == State::FREE) {
    for (auto& obj : objects_)
      ret = obj->OnMouseMove(board, mouse_pos) || ret;
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
      last_click_.reset();
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
