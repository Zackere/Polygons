// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

#include <memory>

#include "../drawing_board.hpp"

namespace gk {
class DrawableObject;

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
  virtual void AddObject(std::unique_ptr<DrawableObject> object) = 0;
};
}  // namespace gk
