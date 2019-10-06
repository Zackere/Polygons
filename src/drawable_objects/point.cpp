// Copyright Wojciech Replin 2019

#include "point.hpp"

namespace gk {
Point::Point(DrawingBoard::CoordinatePair const& position, COLORREF color)
    : position_(position), color_(color), clicked_(false) {}

Point::~Point() = default;

void Point::Display(DrawingBoard* board) {
  board->SetPixel(position_.first, position_.second, color_);
}

bool Point::OnMouseLButtonDown(DrawingBoard* board,
                               DrawingBoard::CoordinatePair mouse_pos) {
  clicked_ = mouse_pos.first == position_.first && mouse_pos.second == position_.second;
  return false;
}

bool Point::OnMouseLButtonUp(DrawingBoard* board,
                             DrawingBoard::CoordinatePair mouse_pos) {
  clicked_ = false;
  return false;
}

bool Point::OnMouseMove(DrawingBoard* board,
                        DrawingBoard::CoordinatePair mouse_pos) {
  if (clicked_) {
    DrawingBoard::CoordinatePair new_pos =
        std::make_pair(mouse_pos.first, mouse_pos.second);
    if (new_pos != position_) {
      position_ = std::move(new_pos);
      return true;
    }
  }
  return false;
}
}  // namespace gk
