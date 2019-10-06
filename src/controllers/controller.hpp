// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

namespace gk {
class DrawingBoard;

class Controller {
 public:
  virtual bool OnMouseLButtonDown(DrawingBoard* board, POINT mouse_pos) = 0;
  virtual bool OnMouseLButtonUp(DrawingBoard* board, POINT mouse_pos) = 0;
  virtual bool OnMouseLButtonDoubleClick(DrawingBoard* board,
                                         POINT mouse_pos) = 0;
  virtual bool OnMouseMove(DrawingBoard* board, POINT mouse_pos) = 0;
  virtual bool OnKeyDown(DrawingBoard* board,
                         WPARAM key_code,
                         bool was_down) = 0;
  virtual bool OnKeyUp(DrawingBoard* board, WPARAM key_code) = 0;
  virtual void Draw(DrawingBoard* board) = 0;
};
}  // namespace gk
