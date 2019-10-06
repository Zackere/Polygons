// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

#include <complex>

#include "../drawing_board.hpp"

namespace gk {
class Point : public DrawingBoard::DrawableObject {
 public:
  Point(std::complex<DrawingBoard::Coordinate> const& position, COLORREF color);

  // Overridden from DrawingBoard::DrawableObject
  void Display(DrawingBoard* board) override;
  bool OnMouseLButtonDown(DrawingBoard* board, POINT mouse_pos) override;
  bool OnMouseLButtonUp(DrawingBoard* board, POINT mouse_pos) override;
  bool OnMouseMove(DrawingBoard* board, POINT mouse_pos) override;

 private:
  std::complex<DrawingBoard::Coordinate> position_;
  COLORREF color_;

  bool clicked_ = false;
};
}  // namespace gk
