// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

#include <utility>

#include "../drawing_board.hpp"

namespace gk {
class Point : public DrawingBoard::DrawableObject {
 public:
  using CoordinatePair =
      std::pair<DrawingBoard::Coordinate, DrawingBoard::Coordinate>;
  Point(CoordinatePair const& position, COLORREF color);
  ~Point() override;

  // Overridden from DrawingBoard::DrawableObject
  void Display(DrawingBoard* board) override;
  bool OnMouseLButtonDown(DrawingBoard* board, POINT mouse_pos) override;
  bool OnMouseLButtonUp(DrawingBoard* board, POINT mouse_pos) override;
  bool OnMouseMove(DrawingBoard* board, POINT mouse_pos) override;

 private:
  CoordinatePair position_;
  COLORREF color_;

  bool clicked_ = false;
};
}  // namespace gk
