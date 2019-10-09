// Copyright Wojciech Replin 2019

#include "polygon.hpp"

namespace gk {
Polygon::Polygon(DrawingBoard::CoordinatePair const& p1,
                 DrawingBoard::CoordinatePair const& p2,
                 DrawingBoard::CoordinatePair const& p3,
                 COLORREF color,
                 COLORREF vertex_color)
    : color_(color),
      vertex_color_(vertex_color),
      body_(PolygonLine::Create(p1, p2, p3, color, vertex_color)) {}

Polygon::~Polygon() = default;

void Polygon::Display(DrawingBoard* board) {
  auto* ptr = body_.get();
  do {
    ptr->Display(board);
    ptr = ptr->GetNext();
  } while (ptr != body_.get());
}

bool Polygon::OnMouseLButtonDown(
    DrawingBoard* board,
    DrawingBoard::CoordinatePair const& mouse_pos) {
  if (Contains(mouse_pos))
    clicked_ = true;
  auto* ptr = body_.get();
  do {
    if (ptr->OnMouseLButtonDown(board, mouse_pos))
      return true;
    ptr = ptr->GetNext();
  } while (ptr != body_.get());
  return false;
}

bool Polygon::OnMouseLButtonUp(DrawingBoard* board,
                               DrawingBoard::CoordinatePair const& mouse_pos) {
  clicked_ = false;
  auto* ptr = body_.get();
  do {
    if (ptr->OnMouseLButtonUp(board, mouse_pos))
      return true;
    ptr = ptr->GetNext();
  } while (ptr != body_.get());
  return false;
}

bool Polygon::OnMouseMove(DrawingBoard* board,
                          DrawingBoard::CoordinatePair const& mouse_pos) {
  bool ret = false;
  if (clicked_) {
    auto* ptr = body_.get();
    do {
      ret = ptr->OnMouseMove(board, mouse_pos) || ret;
      ptr = ptr->GetNext();
    } while (ptr != body_.get());
  }
  return ret;
}

bool Polygon::Contains(DrawingBoard::CoordinatePair const& point) {
  auto* ptr = body_.get();
  do {
    if (ptr->Contains(point))
      return true;
    ptr = ptr->GetNext();
  } while (ptr != body_.get());
  return false;
}

bool Polygon::IsVertex(DrawingBoard::CoordinatePair const& point) {
  auto* ptr = body_.get();
  do {
    if (ptr->IsVertex(point))
      return true;
    ptr = ptr->GetNext();
  } while (ptr != body_.get());
  return false;
}

bool Polygon::RequestRemoval(DrawingBoard::CoordinatePair const& point) {
  if (Contains(point)) {
    if (nverticies_ == 3)
      return false;
    bool removed = false;
    auto* ptr = body_.get();
    do {
      if (ptr->Contains(point)) {
        auto* pom = ptr->GetNext();
        body_.release();
        body_.reset(pom);
        if (!ptr->RequestRemoval(point))
          removed = true;
        ptr = pom;
      } else {
        ptr = ptr->GetNext();
      }
    } while (ptr != body_.get());
    if (removed)
      --nverticies_;
  }
  return true;
}

bool Polygon::AddVertex(DrawingBoard::CoordinatePair const& point,
                        Controller* controller) {
  auto* ptr = body_.get();
  do {
    if (ptr->AddVertex(point, controller)) {
      ++nverticies_;
      break;
    }
    ptr = ptr->GetNext();
  } while (ptr != body_.get());
  return true;
}

void Polygon::OnControllerStateChanged(Controller* controller) {
  clicked_ = false;
}
}  // namespace gk
