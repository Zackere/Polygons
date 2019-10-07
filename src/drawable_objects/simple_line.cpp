// Copyright Wojciech Replin 2019

#include "simple_line.hpp"

#include <algorithm>
#include <cmath>
#include <memory>

#include "../controllers/controller.hpp"

namespace gk {
SimpleLine::SimpleLine(DrawingBoard::CoordinatePair const& begin,
                       DrawingBoard::CoordinatePair const& end,
                       COLORREF color)
    : Line(begin, end, color), vertex_clicked_(false), line_clicked_(false) {}

SimpleLine::SimpleLine(DrawingBoard::CoordinatePair const& begin,
                       DrawingBoard::CoordinatePair const& end,
                       COLORREF color,
                       COLORREF vertex_color)
    : Line(begin, end, color, vertex_color),
      vertex_clicked_(false),
      line_clicked_(false) {}

SimpleLine::~SimpleLine() = default;

void SimpleLine::Display(DrawingBoard* board) {
  Line::Display(board);
}

bool SimpleLine::OnMouseLButtonDown(
    DrawingBoard* board,
    DrawingBoard::CoordinatePair const& mouse_pos) {
  if (mouse_pos.first == GetBegin().first &&
      mouse_pos.second == GetBegin().second) {
    vertex_clicked_ = true;
    begin_clicked_ = true;
    line_clicked_ = false;
    last_mouse_pos_.emplace(mouse_pos.first, mouse_pos.second);
  } else if (mouse_pos.first == GetEnd().first &&
             mouse_pos.second == GetEnd().second) {
    vertex_clicked_ = true;
    begin_clicked_ = false;
    line_clicked_ = false;
    last_mouse_pos_.emplace(mouse_pos.first, mouse_pos.second);
  } else if (Contains(mouse_pos)) {
    vertex_clicked_ = false;
    line_clicked_ = true;
    last_mouse_pos_.emplace(mouse_pos.first, mouse_pos.second);
  }
  return false;
}

bool SimpleLine::OnMouseLButtonUp(
    DrawingBoard* board,
    DrawingBoard::CoordinatePair const& mouse_pos) {
  vertex_clicked_ = line_clicked_ = false;
  last_mouse_pos_.reset();
  return false;
}

bool SimpleLine::OnMouseMove(DrawingBoard* board,
                             DrawingBoard::CoordinatePair const& mouse_pos) {
  if (vertex_clicked_) {
    if (begin_clicked_)
      SetBegin(DrawingBoard::CoordinatePair{mouse_pos.first, mouse_pos.second});
    else
      SetEnd(DrawingBoard::CoordinatePair{mouse_pos.first, mouse_pos.second});
  } else if (line_clicked_) {
    MoveByVector({mouse_pos.first - last_mouse_pos_.value().first,
                  mouse_pos.second - last_mouse_pos_.value().second});
    last_mouse_pos_.emplace(mouse_pos.first, mouse_pos.second);
  }
  return vertex_clicked_ || line_clicked_;
}

bool SimpleLine::IsVertex(DrawingBoard::CoordinatePair const& point) {
  return point == GetBegin() || point == GetEnd();
}

bool SimpleLine::RequestRemoval(DrawingBoard::CoordinatePair const& point) {
  return !Contains(point);
}

bool SimpleLine::AddVertex(DrawingBoard::CoordinatePair const& point,
                           Controller* controller) {
  if (Contains(point) && !IsVertex(point)) {
    controller->AddObject(std::make_unique<SimpleLine>(
        GetBegin(), point, GetColor(), GetVertexColor()));
    controller->AddObject(std::make_unique<SimpleLine>(
        point, GetEnd(), GetColor(), GetVertexColor()));
    return false;
  }
  return true;
}

}  // namespace gk
