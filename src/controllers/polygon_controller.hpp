// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

#include <memory>
#include <unordered_set>

#include "../drawing_board.hpp"
#include "controller.hpp"

namespace gk {
class PolygonController : public Controller {
 public:
  // Overridden from Controller
  bool OnMouseLButtonDown(DrawingBoard* board, POINT mouse_pos) override;
  bool OnMouseLButtonUp(DrawingBoard* board, POINT mouse_pos) override;
  bool OnMouseLButtonDoubleClick(DrawingBoard* board, POINT mouse_pos) override;
  bool OnMouseMove(DrawingBoard* board, POINT mouse_pos) override;
  bool OnKeyDown(DrawingBoard* board, WPARAM key_code, bool was_down) override;
  bool OnKeyUp(DrawingBoard* board, WPARAM key_code) override;
  void Draw(DrawingBoard* board) override;

 private:
  std::unordered_set<std::unique_ptr<DrawingBoard::DrawableObject>> objects_;
  enum class State {
    FREE,
    CREATE_POINT,
    CREATE_LINE,
    CREATE_POLYGON,
    TOTAL_STATES
  } state_ = State::FREE;
};
}  // namespace gk
