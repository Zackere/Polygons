// Copyright Wojciech Replin 2019

#include "point.hpp"

#include <utility>

namespace gk {
Point::Point(std::complex<DrawingBoard::Coordinate> const& position,
             COLORREF color)
    : position_(position), color_(color), clicked_(false) {}

void Point::Display(DrawingBoard* board) {
  board->SetPixel(position_.real(), position_.imag(), color_);
}

bool Point::OnMouseLButtonDown(DrawingBoard* board, POINT mouse_pos) {
  clicked_ = mouse_pos.x == position_.real() && mouse_pos.y == position_.imag();
  return false;
}

bool Point::OnMouseLButtonUp(DrawingBoard* board, POINT mouse_pos) {
  clicked_ = false;
  return false;
}

bool Point::OnMouseMove(DrawingBoard* board, POINT mouse_pos) {
  if (clicked_) {
    std::complex<DrawingBoard::Coordinate> new_pos{
        static_cast<DrawingBoard::Coordinate>(mouse_pos.x),
        static_cast<DrawingBoard::Coordinate>(mouse_pos.y)};
    if (new_pos != position_) {
      position_ = std::move(new_pos);
      return true;
    }
  }
  return false;
}
}  // namespace gk
