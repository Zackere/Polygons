// Copyright Wojciech Replin 2019

#include "polygon_line.hpp"

namespace gk {
std::unique_ptr<PolygonLine> PolygonLine::Create(
    DrawingBoard::CoordinatePair const& p1,
    DrawingBoard::CoordinatePair const& p2,
    DrawingBoard::CoordinatePair const& p3,
    COLORREF color,
    COLORREF vertex_color) {
  std::unique_ptr<PolygonLine> ret(
      new PolygonLine(p1, p2, color, vertex_color));
  ret->AddAfter(PolygonLine(p2, p3, color, vertex_color));
  ret->AddBefore(PolygonLine(p3, p1, color, vertex_color));
  return ret;
}

PolygonLine::~PolygonLine() {
  if (next_ != this) {
    prev_->next_ = nullptr;
    delete next_;
  }
}

void PolygonLine::Display(DrawingBoard* board) {
  Line::Display(board);
}

bool PolygonLine::OnMouseLButtonDown(
    DrawingBoard* board,
    DrawingBoard::CoordinatePair const& mouse_pos) {
  if (mouse_pos == GetBegin()) {
    vertex_clicked_ = true;
    begin_clicked_ = true;
    line_clicked_ = false;
    last_mouse_pos_.emplace(mouse_pos.first, mouse_pos.second);
  } else if (mouse_pos == GetEnd()) {
    vertex_clicked_ = true;
    begin_clicked_ = false;
    line_clicked_ = false;
    last_mouse_pos_.emplace(mouse_pos.first, mouse_pos.second);
  } else if (Contains(mouse_pos)) {
    vertex_clicked_ = false;
    line_clicked_ = true;
    last_mouse_pos_.emplace(mouse_pos.first, mouse_pos.second);
  }
  return vertex_clicked_ || line_clicked_;
}

bool PolygonLine::OnMouseLButtonUp(
    DrawingBoard* board,
    DrawingBoard::CoordinatePair const& mouse_pos) {
  vertex_clicked_ = begin_clicked_ = line_clicked_ = false;
  last_mouse_pos_.reset();
  return false;
}

bool PolygonLine::OnMouseMove(DrawingBoard* board,
                              DrawingBoard::CoordinatePair const& mouse_pos) {
  if (vertex_clicked_) {
    if (begin_clicked_) {
      SetBegin(mouse_pos);
      prev_->SetEnd(GetBegin());
    } else {
      SetEnd(mouse_pos);
      next_->SetBegin(GetEnd());
    }
  } else if (line_clicked_) {
    MoveByVector({mouse_pos.first - last_mouse_pos_.value().first,
                  mouse_pos.second - last_mouse_pos_.value().second});
    prev_->SetEnd(GetBegin());
    next_->SetBegin(GetEnd());
    last_mouse_pos_.emplace(mouse_pos.first, mouse_pos.second);
  }
  return vertex_clicked_ || line_clicked_;
}

bool PolygonLine::IsVertex(DrawingBoard::CoordinatePair const& point) {
  return point == GetBegin() || point == GetEnd();
}

bool PolygonLine::RequestRemoval(DrawingBoard::CoordinatePair const& point) {
  if (Contains(point)) {
    next_->DeletePrev();
    return false;
  }
  return true;
}

bool PolygonLine::AddVertex(DrawingBoard::CoordinatePair const& point,
                            Controller* controller) {
  if (Contains(point) && !IsVertex(point)) {
    AddAfter(PolygonLine(point, GetEnd(), GetColor(), GetVertexColor()));
    SetEnd(point);
    return true;
  }
  return false;
}

void PolygonLine::OnControllerStateChanged(Controller* controller) {
  vertex_clicked_ = begin_clicked_ = line_clicked_ = false;
  last_mouse_pos_.reset();
}

PolygonLine::PolygonLine(DrawingBoard::CoordinatePair const& begin,
                         DrawingBoard::CoordinatePair const& end,
                         COLORREF color)
    : Line(begin, end, color), next_(this), prev_(this) {}

PolygonLine::PolygonLine(DrawingBoard::CoordinatePair const& begin,
                         DrawingBoard::CoordinatePair const& end,
                         COLORREF color,
                         COLORREF vertex_color)
    : Line(begin, end, color, vertex_color), next_(this), prev_(this) {}

void PolygonLine::AddBefore(PolygonLine const& line) {
  PolygonLine* before = new PolygonLine(line);
  prev_->next_ = before;
  before->prev_ = prev_;
  before->next_ = this;
  prev_ = before;
}

void PolygonLine::AddAfter(PolygonLine const& line) {
  PolygonLine* after = new PolygonLine(line);
  after->next_ = next_;
  after->prev_ = this;
  next_->prev_ = after;
  next_ = after;
}

void PolygonLine::DeletePrev() {
  PolygonLine* new_prev = prev_->prev_;
  prev_->next_ = prev_;
  delete prev_;
  prev_ = new_prev;
  new_prev->next_ = this;
  SetBegin(prev_->GetEnd());
}
}  // namespace gk
