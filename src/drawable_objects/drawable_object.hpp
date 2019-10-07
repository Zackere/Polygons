// Copyright Wojciech Replin 2019

#pragma once

#include "../drawing_board.hpp"

namespace gk {
class DrawableObject {
 public:
  virtual ~DrawableObject() = default;
  virtual void Display(DrawingBoard* board) = 0;
  virtual bool OnMouseLButtonDown(DrawingBoard* board,
                                  DrawingBoard::CoordinatePair mouse_pos) = 0;
  virtual bool OnMouseLButtonUp(DrawingBoard* board,
                                DrawingBoard::CoordinatePair mouse_pos) = 0;
  virtual bool OnMouseMove(DrawingBoard* board,
                           DrawingBoard::CoordinatePair mouse_pos) = 0;
};
}  // namespace gk