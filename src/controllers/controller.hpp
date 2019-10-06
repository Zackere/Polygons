// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

#include "../drawing_board.hpp"

namespace gk {
class Controller {
 public:
  virtual ~Controller() = default;
  virtual bool OnMouseLButtonDown(DrawingBoard* board,
                                  DrawingBoard::CoordinatePair mouse_pos) = 0;
  virtual bool OnMouseLButtonUp(DrawingBoard* board,
                                DrawingBoard::CoordinatePair mouse_pos) = 0;
  virtual bool OnMouseLButtonDoubleClick(
      DrawingBoard* board,
      DrawingBoard::CoordinatePair mouse_pos) = 0;
  virtual bool OnMouseMove(DrawingBoard* board,
                           DrawingBoard::CoordinatePair mouse_pos) = 0;
  virtual bool OnKeyDown(DrawingBoard* board,
                         WPARAM key_code,
                         bool was_down) = 0;
  virtual bool OnKeyUp(DrawingBoard* board, WPARAM key_code) = 0;
  virtual void Draw(DrawingBoard* board) = 0;
};
}  // namespace gk
