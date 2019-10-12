// Copyright Wojciech Replin 2019

#include "polygon.hpp"

#undef min
#undef max

#include <algorithm>

namespace gk {
namespace {
constexpr double kMinDistanceFromVertexSquared = 6;
constexpr double kMinDistanceFromEdgeSquared = 6;

float DistanceSquared(DrawingBoard::Point2d const& from,
                      DrawingBoard::Point2d const& to) {
  return static_cast<float>(from.x - to.x) * (from.x - to.x) +
         static_cast<float>(from.y - to.y) * (from.y - to.y);
}

float DotProduct(DrawingBoard::Point2d const& a,
                 DrawingBoard::Point2d const& b) {
  return static_cast<float>(a.x) * b.x + static_cast<float>(a.y) * b.y;
}

float DistanceToSegmentSquared(DrawingBoard::Point2d const& s1,
                               DrawingBoard::Point2d const& s2,
                               DrawingBoard::Point2d const& p) {
  if (s1 == s2)
    return DistanceSquared(s1, p);
  const float dist_sq = DistanceSquared(s1, s2);
  const float t =
      std::max(0.f, std::min(1.f, DotProduct({p.x - s1.x, p.y - s1.y},
                                             {s2.x - s1.x, s2.y - s1.y}) /
                                      dist_sq));
  const DrawingBoard::Point2d projection(
      static_cast<int>(s1.x + t * (static_cast<float>(s2.x) - s1.x)),
      static_cast<int>(s1.y + t * (static_cast<float>(s2.y) - s1.y)));
  return DistanceSquared(p, projection);
}
}  // namespace
std::unique_ptr<Polygon> Polygon::Create(DrawingBoard::Point2d const& p1,
                                         DrawingBoard::Point2d const& p2,
                                         DrawingBoard::Point2d const& p3,
                                         COLORREF edge_color,
                                         COLORREF vertex_color) {
  auto ret = std::make_unique<Polygon>();
  ret->body_ = std::make_unique<PolygonEdge>(p1, p2, edge_color, vertex_color);
  ret->body_->AddAfter(new PolygonEdge(p2, p3, edge_color, vertex_color));
  ret->body_->AddBefore(new PolygonEdge(p3, p1, edge_color, vertex_color));
  return ret;
}

void Polygon::Display(DrawingBoard* drawing_board) {
  auto* ptr = body_.get();
  while ((ptr = ptr->Next()) != body_.get())
    ptr->Display(drawing_board);
  ptr->Display(drawing_board);
}

bool Polygon::OnMouseLButtonDown(DrawingBoard* drawing_board,
                                 DrawingBoard::Point2d const& mouse_pos) {
  auto* ptr = body_.get();
  bool ret = false;
  while ((ptr = ptr->Next()) != body_.get())
    ret = ptr->OnMouseLButtonDown(mouse_pos) || ret;
  return body_->OnMouseLButtonDown(mouse_pos) || ret;
}

bool Polygon::OnMouseLButtonUp(DrawingBoard* drawing_board,
                               DrawingBoard::Point2d const& mouse_pos) {
  auto* ptr = body_.get();
  bool ret = false;
  while ((ptr = ptr->Next()) != body_.get())
    ret = ptr->OnMouseLButtonUp(mouse_pos) || ret;
  return body_->OnMouseLButtonUp(mouse_pos) || ret;
}

bool Polygon::OnMouseMove(DrawingBoard* drawing_board,
                          DrawingBoard::Point2d const& mouse_pos) {
  auto* ptr = body_.get();
  bool ret = false;
  while ((ptr = ptr->Next()) != body_.get())
    if (ptr->OnMouseMove(mouse_pos, drawing_board->GetPreviousMousePos()))
      return true;
  return body_->OnMouseMove(mouse_pos, drawing_board->GetPreviousMousePos());
}

void Polygon::OnControllerStateChanged(PolygonController* controller) {
  auto* ptr = body_.get();
  while ((ptr = ptr->Next()) != body_.get())
    ptr->OnControllerStateChanged(controller);
  return body_->OnControllerStateChanged(controller);
}

bool Polygon::AddVertex(DrawingBoard::Point2d const& pos) {
  auto* ptr = body_.get();
  while ((ptr = ptr->Next()) != body_.get())
    if (ptr->Split(pos))
      return true;
  return body_->Split(pos);
}

bool Polygon::Remove(DrawingBoard::Point2d const& point) {
  auto* ptr = body_.get();
  auto* head = body_.get();
  while ((ptr = ptr->Next()) != body_.get())
    if (ptr->Remove(point, &head)) {
      body_.release();
      body_.reset(head);
      if (body_->Next()->Next() == body_.get())
        return false;
      return true;
    }
  if (body_->Remove(point, &head)) {
    body_.release();
    body_.reset(head);
    if (body_->Next()->Next() == body_.get())
      return false;
  }
  return true;
}

Polygon::PolygonEdge::~PolygonEdge() {
  if (next_ != this) {
    prev_->next_ = nullptr;
    delete next_;
  }
}

Polygon::PolygonEdge::PolygonEdge(DrawingBoard::Point2d const& begin,
                                  DrawingBoard::Point2d const& end,
                                  COLORREF edge_color,
                                  COLORREF vertex_color)
    : begin_(begin),
      end_(end),
      next_(this),
      prev_(this),
      edge_color_(edge_color),
      vertex_color_(vertex_color) {}

void Polygon::PolygonEdge::Display(DrawingBoard* drawing_board) {
  DrawingBoard::Coordinate x, y, i, xe, ye;
  DrawingBoard::Coordinate dx = end_.x - begin_.x;
  DrawingBoard::Coordinate dy = end_.y - begin_.y;
  DrawingBoard::Coordinate dx1 = std::abs(dx);
  DrawingBoard::Coordinate dy1 = std::abs(dy);
  DrawingBoard::Coordinate px = 2 * dy1 - dx1;
  DrawingBoard::Coordinate py = 2 * dx1 - dy1;
  if (dy1 <= dx1) {
    if (dx >= 0) {
      x = begin_.x;
      y = begin_.y;
      xe = end_.x;
    } else {
      x = end_.x;
      y = end_.y;
      xe = begin_.x;
    }
    drawing_board->SetPixel(x, y, edge_color_);
    for (i = 0; x < xe; ++i) {
      ++x;
      if (px < 0) {
        px += 2 * dy1;
      } else {
        if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
          ++y;
        else
          --y;
        px += 2 * (dy1 - dx1);
      }
      drawing_board->SetPixel(x, y, edge_color_);
    }
  } else {
    if (dy >= 0) {
      x = begin_.x;
      y = begin_.y;
      ye = end_.y;
    } else {
      x = end_.x;
      y = end_.y;
      ye = begin_.y;
    }
    drawing_board->SetPixel(x, y, edge_color_);
    for (i = 0; y < ye; ++i) {
      ++y;
      if (py <= 0) {
        py += 2 * dx1;
      } else {
        if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0))
          ++x;
        else
          --x;
        py += 2 * (dx1 - dy1);
      }
      drawing_board->SetPixel(x, y, edge_color_);
    }
  }
  drawing_board->SetPixel(begin_.x, begin_.y, vertex_color_);
  drawing_board->SetPixel(end_.x, end_.y, vertex_color_);
}

void Polygon::PolygonEdge::AddAfter(PolygonEdge* edge) {
  edge->prev_ = this;
  edge->next_ = next_;
  next_->prev_ = edge;
  next_ = edge;
}

void Polygon::PolygonEdge::AddBefore(PolygonEdge* edge) {
  edge->next_ = this;
  edge->prev_ = prev_;
  prev_->next_ = edge;
  prev_ = edge;
}
bool Polygon::PolygonEdge::OnMouseLButtonDown(
    DrawingBoard::Point2d const& mouse_pos) {
  if (DistanceSquared(mouse_pos, begin_) < kMinDistanceFromVertexSquared) {
    begin_clicked_ = true;
    is_clicked_ = true;
    is_edge_clicked_ = false;
    return true;
  } else if (DistanceSquared(mouse_pos, end_) < kMinDistanceFromVertexSquared) {
    begin_clicked_ = false;
    is_clicked_ = true;
    is_edge_clicked_ = false;
    return true;
  } else if (DistanceToSegmentSquared(begin_, end_, mouse_pos) <
             kMinDistanceFromEdgeSquared) {
    begin_clicked_ = false;
    is_clicked_ = true;
    is_edge_clicked_ = true;
  }
  return is_clicked_;
}

bool Polygon::PolygonEdge::OnMouseLButtonUp(
    DrawingBoard::Point2d const& mouse_pos) {
  is_edge_clicked_ = is_clicked_ = begin_clicked_ = false;
  return false;
}

bool Polygon::PolygonEdge::OnMouseMove(
    DrawingBoard::Point2d const& mouse_pos,
    DrawingBoard::Point2d const& prev_mouse_pos) {
  if (is_clicked_) {
    if (mouse_pos == prev_mouse_pos)
      return false;
    const DrawingBoard::Point2d vector = mouse_pos - prev_mouse_pos;
    if (begin_clicked_) {
      SetBegin(begin_ + vector);
      prev_->SetEnd(begin_);
    } else if (!is_edge_clicked_) {
      SetEnd(end_ + vector);
      next_->SetBegin(end_);
    } else if (is_edge_clicked_) {
      SetBegin(begin_ + vector);
      SetEnd(end_ + vector);
      prev_->SetEnd(begin_);
      next_->SetBegin(end_);
    }
    return true;
  }
  return false;
}

void Polygon::PolygonEdge::OnControllerStateChanged(
    PolygonController* controller) {
  is_edge_clicked_ = is_clicked_ = begin_clicked_ = false;
}

bool Polygon::PolygonEdge::Split(DrawingBoard::Point2d const& mouse_pos) {
  if (DistanceToSegmentSquared(begin_, end_, mouse_pos) <
      kMinDistanceFromEdgeSquared) {
    DrawingBoard::Point2d mid = {begin_.x + end_.x, begin_.y + end_.y};
    mid = {mid.x / 2, mid.y / 2};
    auto* new_edge = new PolygonEdge(mid, end_, edge_color_, vertex_color_);
    new_edge->next_ = next_;
    next_->prev_ = new_edge;
    next_ = new_edge;
    new_edge->prev_ = this;
    end_ = mid;
    return true;
  }
  return false;
}

bool Polygon::PolygonEdge::Remove(DrawingBoard::Point2d const& point,
                                  PolygonEdge** head) {
  if (DistanceSquared(end_, point) < kMinDistanceFromVertexSquared) {
    next_->prev_ = prev_;
    next_->SetBegin(begin_);
    prev_->next_ = next_;
    if (*head == this)
      *head = next_;
    prev_ = this;
    next_ = this;
    delete this;
    return true;
  }
  return false;
}
void Polygon::PolygonEdge::SetBegin(DrawingBoard::Point2d const& begin) {
  begin_ = begin;
}

void Polygon::PolygonEdge::SetEnd(DrawingBoard::Point2d const& end) {
  end_ = end;
}
}  // namespace gk
