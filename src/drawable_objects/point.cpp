// Copyright Wojciech Replin 2019

#include "point.hpp"

namespace gk {
Point::Point(CoordinatePair const& position, COLORREF color)
    : position_(position), color_(color), clicked_(false) {}

Point::~Point() = default;

void Point::Display(DrawingBoard* board) {
  board->SetPixel(position_.first, position_.second, color_);
}

bool Point::OnMouseLButtonDown(DrawingBoard* board, POINT mouse_pos) {
  clicked_ = mouse_pos.x == position_.first && mouse_pos.y == position_.second;
  return false;
}

bool Point::OnMouseLButtonUp(DrawingBoard* board, POINT mouse_pos) {
  clicked_ = false;
  return false;
}

bool Point::OnMouseMove(DrawingBoard* board, POINT mouse_pos) {
  if (clicked_) {
    CoordinatePair new_pos = std::make_pair(mouse_pos.x, mouse_pos.y);
    if (new_pos != position_) {
      position_ = std::move(new_pos);
      return true;
    }
  }
  return false;
}
}  // namespace gk
