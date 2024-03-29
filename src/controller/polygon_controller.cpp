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
      if (polygon->OnMouseLButtonDown(mouse_pos))
        return true;
  }
  return false;
}

bool PolygonController::OnMouseLButtonUp(DrawingBoard* board,
                                         DrawingBoard::Point2d mouse_pos) {
  if (state_ == State::FREE) {
    for (auto& polygon : polygons_)
      if (polygon->OnMouseLButtonUp(mouse_pos))
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
        polygons_.insert(Polygon::Create(board, polygon_verticies_[0],
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
          for (auto it = polygons_.begin(); it != polygons_.end(); ++it) {
            auto copy = (*it)->Clone();
            if ((*it)->SetPerpendicular(last_click_.value(), mouse_pos)) {
              if (!(*it)->Correct()) {
                it = polygons_.erase(it);
                polygons_.insert(std::move(copy));
                board->ShowError(
                    L"Could not add perpendicular constraint. Try again later.",
                    false);
              }
              return true;
            }
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
          for (auto it = polygons_.begin(); it != polygons_.end(); ++it) {
            auto copy = (*it)->Clone();
            if ((*it)->SetEqualLength(last_click_.value(), mouse_pos)) {
              if (!(*it)->Correct()) {
                it = polygons_.erase(it);
                polygons_.insert(std::move(copy));
                board->ShowError(
                    L"Could not add equal length constraint. Try again later.",
                    false);
              }
              return true;
            }
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
}  // namespace gk

bool PolygonController::OnMouseMove(DrawingBoard* board,
                                    DrawingBoard::Point2d mouse_pos) {
  if (state_ == State::FREE) {
    for (auto it = polygons_.begin(); it != polygons_.end(); ++it) {
      if ((*it)->Active()) {
        auto copy = (*it)->Clone();
        if ((*it)->OnMouseMove(mouse_pos, board->GetKeyState(VK_CONTROL))) {
          if (!(*it)->Correct()) {
            it = polygons_.erase(it);
            copy->OnMouseMove(mouse_pos, true);
            polygons_.insert(std::move(copy));
          }
          return true;
        }
      }
    }
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
      polygons_.insert(Polygon::CreateSamplePolygon(board));
      return true;
  }
  return false;
}

void PolygonController::Draw(DrawingBoard* board) {
  for (auto& polygon : polygons_)
    polygon->Display();
}

void PolygonController::SetState(State state, DrawingBoard* board) {
  const State old_state = state_;
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
      board->SetTitle(L"Deletion mode");
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
