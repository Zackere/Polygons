// Copyright Wojciech Replin 2019

#include "polygon_controller.hpp"

#include <cmath>

#include "../polygon/polygon.hpp"

namespace gk {
PolygonController::PolygonController() {
  polygon_verticies_.reserve(2);
}

PolygonController::~PolygonController() = default;

bool PolygonController::OnMouseLButtonDown(DrawingBoard* board,
                                           DrawingBoard::Point2d mouse_pos) {
  if (state_ == State::FREE) {
    for (auto& polygon : polygons_)
      if (polygon->OnMouseLButtonDown(board, mouse_pos))
        return true;
  }
  return false;
}

bool PolygonController::OnMouseLButtonUp(DrawingBoard* board,
                                         DrawingBoard::Point2d mouse_pos) {
  if (state_ == State::FREE) {
    for (auto& polygon : polygons_)
      if (polygon->OnMouseLButtonUp(board, mouse_pos))
        return true;
  }
  return false;
}

bool PolygonController::OnMouseLButtonDoubleClick(
    DrawingBoard* board,
    DrawingBoard::Point2d mouse_pos) {
  switch (state_) {
    case State::CREATE_VERTEX:
      for (auto& polygon : polygons_)
        if (polygon->AddVertex(mouse_pos))
          return true;
      return false;
    case State::CREATE_POLYGON:
      if (polygon_verticies_.size() == 2) {
        polygons_.insert(Polygon::Create(polygon_verticies_[0],
                                         polygon_verticies_[1], mouse_pos,
                                         RGB(0, 255, 0), RGB(255, 0, 0)));
        polygon_verticies_.clear();
        return true;
      } else {
        polygon_verticies_.emplace_back(mouse_pos);
        return false;
      }
    case State::PURE_DESTRUCTION: {
      for (auto it = polygons_.begin(); it != polygons_.end();) {
        auto& polygon = *it;
        if (!polygon->Remove(mouse_pos))
          it = polygons_.erase(it);
        else
          ++it;
      }
      return true;
      case State::SET_PERPENDICULAR: {
        if (last_click_.has_value()) {
          for (auto& polygon : polygons_)
            if (polygon->SetPerpendicular(last_click_.value(), mouse_pos)) {
              last_click_.reset();
              SetState(State::FREE, board);
              return true;
            }
          last_click_.emplace(mouse_pos);
          return false;
        } else {
          last_click_.emplace(mouse_pos);
          return false;
        }
      }
      case State::SET_EQUAL_LENGTH: {
        if (last_click_.has_value()) {
          for (auto& polygon : polygons_)
            if (polygon->SetEqualLength(last_click_.value(), mouse_pos)) {
              last_click_.reset();
              SetState(State::FREE, board);
              return true;
            }
          last_click_.emplace(mouse_pos);
          return false;
        } else {
          last_click_.emplace(mouse_pos);
          return false;
        }
      }
    }
  }
  return false;
}

bool PolygonController::OnMouseMove(DrawingBoard* board,
                                    DrawingBoard::Point2d mouse_pos) {
  if (state_ == State::FREE) {
    for (auto& polygon : polygons_)
      if (polygon->OnMouseMove(board, mouse_pos,
                               board->GetKeyState(VK_CONTROL)))
        return true;
  }
  return false;
}

bool PolygonController::OnKeyDown(DrawingBoard* board,
                                  WPARAM key_code,
                                  bool was_down) {
  return false;
}

bool PolygonController::OnKeyUp(DrawingBoard* board, WPARAM key_code) {
  switch (key_code) {
    case 'Q':
      SetState(State::FREE, board);
      break;
    case 'W':
      SetState(State::CREATE_VERTEX, board);
      break;
    case 'E':
      SetState(State::CREATE_POLYGON, board);
      break;
    case 'A':
      SetState(State::SET_PERPENDICULAR, board);
      break;
    case 'S':
      SetState(State::SET_EQUAL_LENGTH, board);
      break;
    case 'D':
      SetState(State::PURE_DESTRUCTION, board);
      break;
    case VK_SPACE:
      polygons_.insert(Polygon::CreateSamplePolygon());
      break;
  }
  return false;
}

void PolygonController::Draw(DrawingBoard* board) {
  for (auto& polygon : polygons_)
    polygon->Display(board);
}

void PolygonController::SetState(State state, DrawingBoard* board) {
  State old_state = state_;
  switch (state) {
    case State::FREE:
      board->SetTitle(L"Free mode");
      break;
    case State::CREATE_VERTEX:
      board->SetTitle(L"Vertex creation mode");
      break;
    case State::CREATE_POLYGON:
      board->SetTitle(L"Polygon creation mode");
      polygon_verticies_.clear();
      break;
    case State::PURE_DESTRUCTION:
      board->SetTitle(L"Vertex deletion mode");
      break;
    case State::SET_PERPENDICULAR:
      last_click_.reset();
      board->SetTitle(L"Adding perpendicular constraint");
      break;
    case State::SET_EQUAL_LENGTH:
      last_click_.reset();
      board->SetTitle(L"Adding equal length constraint");
      break;
    case State::TOTAL_STATES:
    default:
      return;
  }
  state_ = state;
  if (old_state != state_)
    for (auto& polygon : polygons_)
      polygon->OnControllerStateChanged(this);
}

}  // namespace gk
