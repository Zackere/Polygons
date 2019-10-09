// Copyright Wojciech Replin 2019

#include "polygon_controller.hpp"

#include "../drawable_objects/point.hpp"
#include "../drawable_objects/polygon.hpp"
#include "../drawable_objects/simple_line.hpp"

namespace gk {
PolygonController::PolygonController() {
  polygon_verticies_.reserve(2);
}

PolygonController::~PolygonController() = default;

bool PolygonController::OnMouseLButtonDown(
    DrawingBoard* board,
    DrawingBoard::CoordinatePair mouse_pos) {
  if (state_ == State::FREE) {
    for (auto& obj : objects_)
      if (obj->OnMouseLButtonDown(board, mouse_pos))
        return true;
  }
  return false;
}

bool PolygonController::OnMouseLButtonUp(
    DrawingBoard* board,
    DrawingBoard::CoordinatePair mouse_pos) {
  if (state_ == State::FREE) {
    for (auto& obj : objects_)
      if (obj->OnMouseLButtonUp(board, mouse_pos))
        return true;
  }
  return false;
}

bool PolygonController::OnMouseLButtonDoubleClick(
    DrawingBoard* board,
    DrawingBoard::CoordinatePair mouse_pos) {
  switch (state_) {
    case gk::PolygonController::State::CREATE_POINT:
      AddObject(std::make_unique<Point>(mouse_pos, RGB(255, 0, 0)));
      return true;
    case gk::PolygonController::State::CREATE_VERTEX: {
      for (auto it = objects_.begin(); it != objects_.end();) {
        auto& object = *it;
        if (!object->AddVertex(mouse_pos, this))
          it = objects_.erase(it);
        else
          ++it;
      }
      return true;
    }
    case gk::PolygonController::State::CREATE_LINE:
      if (last_click_.has_value()) {
        AddObject(std::make_unique<SimpleLine>(
            last_click_.value(),
            DrawingBoard::CoordinatePair{mouse_pos.first, mouse_pos.second},
            RGB(0, 255, 0), RGB(255, 0, 0)));
        last_click_.reset();
        return true;
      } else {
        last_click_.emplace(mouse_pos.first, mouse_pos.second);
        return false;
      }
    case gk::PolygonController::State::CREATE_POLYGON:
      if (polygon_verticies_.size() == 2) {
        AddObject(std::make_unique<Polygon>(polygon_verticies_[0],
                                            polygon_verticies_[1], mouse_pos,
                                            RGB(0, 255, 0), RGB(255, 0, 0)));
        polygon_verticies_.clear();
        return true;
      } else {
        polygon_verticies_.emplace_back(mouse_pos);
        return false;
      }
    case gk::PolygonController::State::PURE_DESTRUCTION: {
      for (auto it = objects_.begin(); it != objects_.end();) {
        auto& object = *it;
        if (!object->RequestRemoval(mouse_pos))
          it = objects_.erase(it);
        else
          ++it;
      }
      return true;
    }
  }
  return false;
}

bool PolygonController::OnMouseMove(DrawingBoard* board,
                                    DrawingBoard::CoordinatePair mouse_pos) {
  if (state_ == State::FREE) {
    for (auto& obj : objects_)
      if (obj->OnMouseMove(board, mouse_pos))
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
  State old_state = state_;
  switch (key_code) {
    case 'Q':
      state_ = State::FREE;
      board->SetTitle(L"Free mode");
      break;
    case 'W':
      state_ = State::CREATE_VERTEX;
      board->SetTitle(L"Vertex creation mode");
      break;
    case 'E':
      state_ = State::CREATE_LINE;
      last_click_.reset();
      board->SetTitle(L"Line creation mode");
      break;
    case 'R':
      state_ = State::CREATE_POLYGON;
      board->SetTitle(L"Polygon creation mode");
      polygon_verticies_.clear();
      break;
    case 'T':
      state_ = State::CREATE_POINT;
      board->SetTitle(L"Point creation mode");
      break;
    case 'D':
      state_ = gk::PolygonController::State::PURE_DESTRUCTION;
      board->SetTitle(L"Object deletion mode");
      break;
  }
  if (old_state != state_)
    for (auto& object : objects_)
      object->OnControllerStateChanged(this);
  return false;
}

void PolygonController::Draw(DrawingBoard* board) {
  for (auto& obj : objects_)
    obj->Display(board);
}

void PolygonController::AddObject(std::unique_ptr<DrawableObject> object) {
  objects_.push_front(std::move(object));
}
}  // namespace gk
