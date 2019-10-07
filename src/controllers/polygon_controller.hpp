// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

#include <memory>
#include <optional>
#include <unordered_set>
#include <utility>

#include "../drawable_objects/drawable_object.hpp"
#include "../drawing_board.hpp"
#include "controller.hpp"

namespace gk {
class PolygonController : public Controller {
 public:
  // Overridden from Controller
  ~PolygonController() override;
  bool OnMouseLButtonDown(DrawingBoard* board,
                          DrawingBoard::CoordinatePair mouse_pos) override;
  bool OnMouseLButtonUp(DrawingBoard* board,
                        DrawingBoard::CoordinatePair mouse_pos) override;
  bool OnMouseLButtonDoubleClick(
      DrawingBoard* board,
      DrawingBoard::CoordinatePair mouse_pos) override;
  bool OnMouseMove(DrawingBoard* board,
                   DrawingBoard::CoordinatePair mouse_pos) override;
  bool OnKeyDown(DrawingBoard* board, WPARAM key_code, bool was_down) override;
  bool OnKeyUp(DrawingBoard* board, WPARAM key_code) override;
  void Draw(DrawingBoard* board) override;

 private:
  std::unordered_set<std::unique_ptr<DrawableObject>> objects_;
  enum class State {
    FREE,
    CREATE_POINT,
    CREATE_LINE,
    CREATE_POLYGON,
    TOTAL_STATES
  } state_ = State::FREE;

  std::optional<DrawingBoard::CoordinatePair> last_click_;
};
}  // namespace gk
