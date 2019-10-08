// Copyright Wojciech Replin 2019

#pragma once

#include "../drawing_board.hpp"

namespace gk {
class Controller;

class DrawableObject {
 public:
  virtual ~DrawableObject() = default;
  virtual void Display(DrawingBoard* board) = 0;
  virtual bool OnMouseLButtonDown(
      DrawingBoard* board,
      DrawingBoard::CoordinatePair const& mouse_pos) = 0;
  virtual bool OnMouseLButtonUp(
      DrawingBoard* board,
      DrawingBoard::CoordinatePair const& mouse_pos) = 0;
  virtual bool OnMouseMove(DrawingBoard* board,
                           DrawingBoard::CoordinatePair const& mouse_pos) = 0;
  virtual bool Contains(DrawingBoard::CoordinatePair const& point) = 0;
  virtual bool IsVertex(DrawingBoard::CoordinatePair const& point) = 0;
  virtual bool RequestRemoval(DrawingBoard::CoordinatePair const& point) = 0;
  virtual bool AddVertex(DrawingBoard::CoordinatePair const& point,
                         Controller* controller) = 0;
  virtual void OnControllerStateChanged(Controller* controller) = 0;
};
}  // namespace gk
