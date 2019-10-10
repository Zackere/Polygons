#include "polygon.hpp"

#undef min
#undef max

#include <algorithm>

namespace gk {
namespace {
constexpr double kMinDistanceFromVertexSquared = 6;
constexpr double kMinDistanceFromEdgeSquared = 6;

float DistanceSquared(DrawingBoard::CoordinatePair const& from,
                      DrawingBoard::CoordinatePair const& to) {
  return static_cast<float>(from.first - to.first) * (from.first - to.first) +
         static_cast<float>(from.second - to.second) *
             (from.second - to.second);
}

float DotProduct(DrawingBoard::CoordinatePair const& a,
                 DrawingBoard::CoordinatePair const& b) {
  return static_cast<float>(a.first) * b.first +
         static_cast<float>(a.second) * b.second;
}

float DistanceToSegmentSquared(DrawingBoard::CoordinatePair const& s1,
                               DrawingBoard::CoordinatePair const& s2,
                               DrawingBoard::CoordinatePair const& p) {
  if (s1 == s2)
    return DistanceSquared(s1, p);
  const float dist_sq = DistanceSquared(s1, s2);
  const float t = std::max(
      0.f,
      std::min(1.f, DotProduct({p.first - s1.first, p.second - s1.second},
                               {s2.first - s1.first, s2.second - s1.second}) /
                        dist_sq));
  const DrawingBoard::CoordinatePair projection(
      static_cast<int>(s1.first +
                         t * (static_cast<float>(s2.first) - s1.first)),
      static_cast<int>(s1.second +
                         t * (static_cast<float>(s2.second) - s1.second)));
  return DistanceSquared(p, projection);
}
}  // namespace
std::unique_ptr<Polygon> Polygon::Create(DrawingBoard::CoordinatePair const& p1,
                                         DrawingBoard::CoordinatePair const& p2,
                                         DrawingBoard::CoordinatePair const& p3,
                                         COLORREF edge_color,
                                         COLORREF vertex_color) {
  std::shared_ptr<DrawingBoard::CoordinatePair> v1, v2, v3;
  v1 = std::make_shared<DrawingBoard::CoordinatePair>(p1);
  v2 = std::make_shared<DrawingBoard::CoordinatePair>(p2);
  v3 = std::make_shared<DrawingBoard::CoordinatePair>(p3);
  auto ret = std::make_unique<Polygon>();
  ret->body_ = std::make_unique<PolygonEdge>(v1, v2, edge_color, vertex_color);
  ret->body_->AddAfter(new PolygonEdge(v2, v3, edge_color, vertex_color));
  ret->body_->AddBefore(new PolygonEdge(v3, v1, edge_color, vertex_color));
  return ret;
}

void Polygon::Display(DrawingBoard* drawing_board) {
  auto* ptr = body_.get();
  while ((ptr = ptr->Next()) != body_.get())
    ptr->Display(drawing_board);
  ptr->Display(drawing_board);
}

bool Polygon::OnMouseLButtonDown(
    DrawingBoard* drawing_board,
    DrawingBoard::CoordinatePair const& mouse_pos) {
  auto* ptr = body_.get();
  bool ret = false;
  while ((ptr = ptr->Next()) != body_.get())
    ret = ptr->OnMouseLButtonDown(mouse_pos) || ret;
  return body_->OnMouseLButtonDown(mouse_pos) || ret;
}

bool Polygon::OnMouseLButtonUp(DrawingBoard* drawing_board,
                               DrawingBoard::CoordinatePair const& mouse_pos) {
  auto* ptr = body_.get();
  bool ret = false;
  while ((ptr = ptr->Next()) != body_.get())
    ret = ptr->OnMouseLButtonUp(mouse_pos) || ret;
  return body_->OnMouseLButtonUp(mouse_pos) || ret;
}

bool Polygon::OnMouseMove(DrawingBoard* drawing_board,
                          DrawingBoard::CoordinatePair const& mouse_pos) {
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

bool Polygon::AddVertex(DrawingBoard::CoordinatePair const& pos) {
  auto* ptr = body_.get();
  while ((ptr = ptr->Next()) != body_.get())
    if (ptr->Split(pos))
      return true;
  return body_->Split(pos);
}

bool Polygon::Remove(DrawingBoard::CoordinatePair const& point) {
  if (body_->Next()->Next()->Next() == body_.get())
    return false;
  auto* ptr = body_.get();
  auto* head = body_.get();
  while ((ptr = ptr->Next()) != body_.get())
    if (ptr->Remove(point, &head)) {
      body_.release();
      body_.reset(head);
      return true;
    }
  if (body_->Remove(point, &head)) {
    body_.release();
    body_.reset(head);
    return true;
  }
  return false;
}

Polygon::PolygonEdge::~PolygonEdge() {
  if (next_ != this) {
    prev_->next_ = nullptr;
    delete next_;
  }
}

Polygon::PolygonEdge::PolygonEdge(
    std::shared_ptr<DrawingBoard::CoordinatePair> begin,
    std::shared_ptr<DrawingBoard::CoordinatePair> end,
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
  DrawingBoard::Coordinate dx = end_->first - begin_->first;
  DrawingBoard::Coordinate dy = end_->second - begin_->second;
  DrawingBoard::Coordinate dx1 = std::abs(dx);
  DrawingBoard::Coordinate dy1 = std::abs(dy);
  DrawingBoard::Coordinate px = 2 * dy1 - dx1;
  DrawingBoard::Coordinate py = 2 * dx1 - dy1;
  if (dy1 <= dx1) {
    if (dx >= 0) {
      x = begin_->first;
      y = begin_->second;
      xe = end_->first;
    } else {
      x = end_->first;
      y = end_->second;
      xe = begin_->first;
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
      x = begin_->first;
      y = begin_->second;
      ye = end_->second;
    } else {
      x = end_->first;
      y = end_->second;
      ye = begin_->second;
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
  drawing_board->SetPixel(begin_->first, begin_->second, vertex_color_);
  drawing_board->SetPixel(end_->first, end_->second, vertex_color_);
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
    DrawingBoard::CoordinatePair const& mouse_pos) {
  if (DistanceSquared(mouse_pos, *begin_) < kMinDistanceFromVertexSquared) {
    begin_clicked_ = true;
    is_clicked_ = true;
    is_edge_clicked_ = false;
    return true;
  } else if (DistanceSquared(mouse_pos, *end_) <
             kMinDistanceFromVertexSquared) {
    begin_clicked_ = false;
    is_clicked_ = true;
    is_edge_clicked_ = false;
    return true;
  } else if (DistanceToSegmentSquared(*begin_, *end_, mouse_pos) <
             kMinDistanceFromEdgeSquared) {
    begin_clicked_ = false;
    is_clicked_ = true;
    is_edge_clicked_ = true;
  }
  return is_clicked_;
}

bool Polygon::PolygonEdge::OnMouseLButtonUp(
    DrawingBoard::CoordinatePair const& mouse_pos) {
  is_edge_clicked_ = is_clicked_ = begin_clicked_ = false;
  return false;
}

bool Polygon::PolygonEdge::OnMouseMove(
    DrawingBoard::CoordinatePair const& mouse_pos,
    DrawingBoard::CoordinatePair const& prev_mouse_pos) {
  if (is_clicked_) {
    const DrawingBoard::CoordinatePair vector = {
        mouse_pos.first - prev_mouse_pos.first,
        mouse_pos.second - prev_mouse_pos.second};
    if (vector == DrawingBoard::CoordinatePair{0, 0})
      return false;
    if (begin_clicked_) {
      *begin_ = {begin_->first + vector.first, begin_->second + vector.second};
    } else if (!is_edge_clicked_) {
      *end_ = {end_->first + vector.first, end_->second + vector.second};
    } else if (is_edge_clicked_) {
      *begin_ = {begin_->first + vector.first, begin_->second + vector.second};
      *end_ = {end_->first + vector.first, end_->second + vector.second};
    }
    return true;
  }
  return false;
}

void Polygon::PolygonEdge::OnControllerStateChanged(
    PolygonController* controller) {
  is_edge_clicked_ = is_clicked_ = begin_clicked_ = false;
}

bool Polygon::PolygonEdge::Split(
    DrawingBoard::CoordinatePair const& mouse_pos) {
  if (DistanceToSegmentSquared(*begin_, *end_, mouse_pos) <
      kMinDistanceFromEdgeSquared) {
    DrawingBoard::CoordinatePair mid = {begin_->first + end_->first,
                                        begin_->second + end_->second};
    mid = {mid.first / 2, mid.second / 2};
    auto mid_ptr = std::make_shared<DrawingBoard::CoordinatePair>(mid);
    auto* new_edge = new PolygonEdge(mid_ptr, end_, edge_color_, vertex_color_);
    new_edge->next_ = next_;
    next_->prev_ = new_edge;
    next_ = new_edge;
    new_edge->prev_ = this;
    end_ = mid_ptr;
    return true;
  }
  return false;
}

bool Polygon::PolygonEdge::Remove(DrawingBoard::CoordinatePair const& point,
                                  PolygonEdge** head) {
  if (DistanceSquared(*begin_, point) < kMinDistanceFromVertexSquared ||
      DistanceSquared(*end_, point) < kMinDistanceFromVertexSquared ||
      DistanceToSegmentSquared(*begin_, *end_, point) <
          kMinDistanceFromEdgeSquared) {
    next_->prev_ = prev_;
    next_->begin_ = begin_;
    prev_->next_ = next_;
    prev_->end_ = begin_;
    if (*head == this)
      *head = next_;
    prev_ = this;
    next_ = this;
    delete this;
    return true;
  }
  return false;
}
}  // namespace gk
