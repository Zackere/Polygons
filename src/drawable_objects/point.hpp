// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

#include <utility>

#include "../drawing_board.hpp"
#include "drawable_object.hpp"

namespace gk {
class Point : public DrawableObject {
 public:
  Point(DrawingBoard::CoordinatePair const& position, COLORREF color);
  ~Point() override;

  // Overridden from DrawingBoard::DrawableObject
  void Display(DrawingBoard* board) override;
  bool OnMouseLButtonDown(
      DrawingBoard* board,
      DrawingBoard::CoordinatePair const& mouse_pos) override;
  bool OnMouseLButtonUp(DrawingBoard* board,
                        DrawingBoard::CoordinatePair const& mouse_pos) override;
  bool OnMouseMove(DrawingBoard* board,
                   DrawingBoard::CoordinatePair const& mouse_pos) override;
  bool Contains(DrawingBoard::CoordinatePair const& point) override;
  bool IsVertex(DrawingBoard::CoordinatePair const& point) override;
  bool RequestRemoval(DrawingBoard::CoordinatePair const& point) override;
  bool AddVertex(DrawingBoard::CoordinatePair const& point,
                 Controller* controller) override;

 private:
  DrawingBoard::CoordinatePair position_;
  COLORREF color_;

  bool clicked_ = false;
};
}  // namespace gk
