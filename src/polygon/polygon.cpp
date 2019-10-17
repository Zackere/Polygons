// Copyright Wojciech Replin 2019

#include "polygon.hpp"

#undef min
#undef max

#include <algorithm>
#include <map>
#include <numeric>
#include <optional>
#include <string>
#include <string_view>

namespace gk {
namespace {
constexpr double kMinDistanceFromVertexSquared = 6;
constexpr double kMinDistanceFromEdgeSquared = 6;
constexpr wchar_t kUpTack[] = {8869};
constexpr wchar_t kEqualSign[] = L"=";
constexpr double kVerySmallValue = 0.001;
constexpr unsigned int kMaxIters = 100;

double DistanceSquared(DrawingBoard::Point2d const& from,
                       DrawingBoard::Point2d const& to) {
  return (static_cast<double>(from.x) - to.x) *
             (static_cast<double>(from.x) - to.x) +
         (static_cast<double>(from.y) - to.y) *
             (static_cast<double>(from.y) - to.y);
}

double DotProduct(DrawingBoard::Point2d const& a,
                  DrawingBoard::Point2d const& b) {
  return static_cast<double>(a.x) * b.x + static_cast<double>(a.y) * b.y;
}

double DistanceToSegmentSquared(DrawingBoard::Point2d const& s1,
                                DrawingBoard::Point2d const& s2,
                                DrawingBoard::Point2d const& p) {
  if (s1 == s2)
    return DistanceSquared(s1, p);
  const double dist_sq = DistanceSquared(s1, s2);
  const double t =
      std::max(0.0, std::min(1.0, DotProduct(p - s1, s2 - s1) / dist_sq));
  const DrawingBoard::Point2d projection(
      static_cast<int>(s1.x + t * (static_cast<double>(s2.x) - s1.x)),
      static_cast<int>(s1.y + t * (static_cast<double>(s2.y) - s1.y)));
  return DistanceSquared(p, projection);
}

DrawingBoard::Point2d ClosestPointOnCircle(DrawingBoard::Point2d const& center,
                                           double radius,
                                           DrawingBoard::Point2d const& to) {
  auto pos = (to - center) * radius;
  pos = pos / std::sqrt(DistanceSquared(to, center));
  return center + pos;
}

void DisplayLabel(DrawingBoard* board,
                  DrawingBoard::Point2d const& pos,
                  std::wstring_view label) {
  constexpr int font_size = 15;
  board->DrawTxt(pos.x, pos.y, label.data(), font_size, RGB(255, 0, 0));
}

double Determinant(DrawingBoard::Point2d const& p1,
                   DrawingBoard::Point2d const& p2) {
  return p1.x * p2.y - p1.y * p2.x;
}

bool Colinear(DrawingBoard::Point2d const& p1,
              DrawingBoard::Point2d const& p2,
              DrawingBoard::Point2d const& p3) {
  return std::abs(Determinant(p1 - p2, p2 - p3)) < kVerySmallValue;
}

std::optional<DrawingBoard::Point2d> IntersectLines(
    DrawingBoard::Point2d const& A1,
    DrawingBoard::Point2d const& A2,
    DrawingBoard::Point2d const& B1,
    DrawingBoard::Point2d const& B2) {
  // Line represented as a1x + b1y = c1;
  double a1 = A2.y - A1.y;
  double b1 = A1.x - A2.x;
  double c1 = a1 * A1.x + b1 * A1.y;
  // Line represented as a2x + b2y = c2;
  double a2 = B2.y - B1.y;
  double b2 = B1.x - B2.x;
  double c2 = a2 * B1.x + b2 * B1.y;
  double det = a1 * b2 - a2 * b1;
  if (std::abs(det) < kVerySmallValue)
    return std::nullopt;
  return DrawingBoard::Point2d{(b2 * c1 - b1 * c2) / det,
                               (a1 * c2 - a2 * c1) / det};
}

auto CircleIntersection(DrawingBoard::Point2d const& center1,
                        DrawingBoard::Point2d const& center2,
                        DrawingBoard::Point2d const& point1,
                        DrawingBoard::Point2d const& point2) {
  struct Result {
    DrawingBoard::Point2d p1, p2;
  };
  std::optional<Result> result;
  const auto r1 = std::sqrt(DistanceSquared(center1, point1)),
             r2 = std::sqrt(DistanceSquared(center2, point2)),
             d = std::sqrt(DistanceSquared(center1, center2));
  if (d > r1 + r2 || d < std::abs(r1 - r2))
    return result;

  const auto e = (center2 - center1) / d;
  const auto x = (r1 * r1 - r2 * r2 + d * d) / (2 * d),
             y = std::sqrt(r1 * r1 - x * x);
  result = Result{DrawingBoard::Point2d{center1.x + x * e.x - y * e.y,
                                        center1.y + x * e.y + y * e.x},
                  DrawingBoard::Point2d{center1.x + x * e.x + y * e.y,
                                        center1.y + x * e.y - y * e.x}};
  return result;
}
}  // namespace
std::unique_ptr<Polygon> Polygon::CreateSamplePolygon(
    DrawingBoard* drawing_board) {
  constexpr COLORREF edge_color = RGB(0, 255, 0);
  constexpr COLORREF vertex_color = RGB(255, 0, 0);
  constexpr double x[] = {262.0, 290.0, 170.0, 197.0, 90.0, 11.0, 86.0};
  constexpr double y[] = {279.0, 222.0, 198.0, 59.0, 5.0, 110.0, 278.0};
  auto ret = std::make_unique<Polygon>();
  PolygonEdge* edge[] = {
      new PolygonEdge(drawing_board, DrawingBoard::Point2d{x[0], y[0]},
                      DrawingBoard::Point2d{x[1], y[1]}, edge_color,
                      vertex_color),
      new PolygonEdge(drawing_board, DrawingBoard::Point2d{x[1], y[1]},
                      DrawingBoard::Point2d{x[2], y[2]}, edge_color,
                      vertex_color),
      new PolygonEdge(drawing_board, DrawingBoard::Point2d{x[2], y[2]},
                      DrawingBoard::Point2d{x[3], y[3]}, edge_color,
                      vertex_color),
      new PolygonEdge(drawing_board, DrawingBoard::Point2d{x[3], y[3]},
                      DrawingBoard::Point2d{x[4], y[4]}, edge_color,
                      vertex_color),
      new PolygonEdge(drawing_board, DrawingBoard::Point2d{x[4], y[4]},
                      DrawingBoard::Point2d{x[5], y[5]}, edge_color,
                      vertex_color),
      new PolygonEdge(drawing_board, DrawingBoard::Point2d{x[5], y[5]},
                      DrawingBoard::Point2d{x[6], y[6]}, edge_color,
                      vertex_color),
      new PolygonEdge(drawing_board, DrawingBoard::Point2d{x[6], y[6]},
                      DrawingBoard::Point2d{x[0], y[0]}, edge_color,
                      vertex_color),
  };
  ret->body_.reset(edge[0]);
  for (int i = 1; i < 7; ++i)
    ret->body_->AddBefore(edge[i]);

  edge[0]->constraint_ = PolygonEdge::Constraint::PERPENDICULAR;
  edge[0]->constraint_id_ = id_manager::Get();
  edge[0]->constrained_edge_ = edge[3];

  edge[1]->constraint_ = PolygonEdge::Constraint::PERPENDICULAR;
  edge[1]->constraint_id_ = id_manager::Get();
  edge[1]->constrained_edge_ = edge[2];

  edge[2]->constraint_ = edge[1]->constraint_;
  edge[2]->constraint_id_ = edge[1]->constraint_id_;
  edge[2]->constrained_edge_ = edge[1];

  edge[3]->constraint_ = edge[0]->constraint_;
  edge[3]->constraint_id_ = edge[0]->constraint_id_;
  edge[3]->constrained_edge_ = edge[0];

  edge[4]->constraint_ = PolygonEdge::Constraint::EQUAL_LENGTH;
  edge[4]->constraint_id_ = id_manager::Get();
  edge[4]->constrained_edge_ = edge[5];

  edge[5]->constraint_ = edge[4]->constraint_;
  edge[5]->constraint_id_ = edge[4]->constraint_id_;
  edge[5]->constrained_edge_ = edge[4];

  edge[6]->constraint_ = PolygonEdge::Constraint::NONE;
  edge[6]->constraint_id_ = 0;
  edge[6]->constrained_edge_ = nullptr;

  ret->nverticies_ = 7;
  ret->drawing_board_ = drawing_board;
  return ret;
}

std::unique_ptr<Polygon> Polygon::Create(DrawingBoard* drawing_board,
                                         DrawingBoard::Point2d const& p1,
                                         DrawingBoard::Point2d const& p2,
                                         DrawingBoard::Point2d const& p3,
                                         COLORREF edge_color,
                                         COLORREF vertex_color) {
  auto ret = std::make_unique<Polygon>();
  ret->body_ = std::make_unique<PolygonEdge>(drawing_board, p1, p2, edge_color,
                                             vertex_color);
  ret->body_->AddAfter(
      new PolygonEdge(drawing_board, p2, p3, edge_color, vertex_color));
  ret->body_->AddBefore(
      new PolygonEdge(drawing_board, p3, p1, edge_color, vertex_color));
  ret->nverticies_ = 3;
  ret->drawing_board_ = drawing_board;
  return ret;
}

void Polygon::Display() {
  auto* ptr = body_.get();
  do {
    ptr->Display();
  } while ((ptr = ptr->Next()) != body_.get());
}

bool Polygon::OnMouseLButtonDown(DrawingBoard::Point2d const& mouse_pos) {
  auto* ptr = body_.get();
  bool ret = false;
  do {
    ret = ptr->OnMouseLButtonDown(mouse_pos) || ret;
  } while ((ptr = ptr->Next()) != body_.get());
  return ret;
}

bool Polygon::OnMouseLButtonUp(DrawingBoard::Point2d const& mouse_pos) {
  auto* ptr = body_.get();
  bool ret = false;
  do {
    ret = ptr->OnMouseLButtonUp(mouse_pos) || ret;
  } while ((ptr = ptr->Next()) != body_.get());
  return ret;
}

bool Polygon::OnMouseMove(DrawingBoard::Point2d const& mouse_pos,
                          bool move_whole) {
  if (move_whole)
    return body_->MoveWhole(mouse_pos, drawing_board_->GetPreviousMousePos());
  auto* ptr = body_.get();
  do {
    if (ptr->OnMouseMove(mouse_pos, drawing_board_->GetPreviousMousePos(),
                         3 * nverticies_))
      return true;
  } while ((ptr = ptr->Next()) != body_.get());
  return false;
}

void Polygon::OnControllerStateChanged(PolygonController* controller) {
  auto* ptr = body_.get();
  do {
    ptr->OnControllerStateChanged(controller);
  } while ((ptr = ptr->Next()) != body_.get());
}

bool Polygon::AddVertex(DrawingBoard::Point2d const& pos) {
  auto* ptr = body_.get();
  while ((ptr = ptr->Next()) != body_.get())
    if (ptr->Split(pos)) {
      ++nverticies_;
      return true;
    }
  return body_->Split(pos);
}

bool Polygon::Remove(DrawingBoard::Point2d const& point) {
  auto* ptr = body_.get();
  auto* head = body_.get();
  do {
    if (ptr->Remove(point, &head, 3 * nverticies_)) {
      --nverticies_;
      body_.release();
      body_.reset(head);
      if (body_->Next()->Next() == body_.get())
        return false;
      return true;
    }
  } while ((ptr = ptr->Next()) != body_.get());
  return true;
}

bool Polygon::SetPerpendicular(DrawingBoard::Point2d const& p1,
                               DrawingBoard::Point2d const& p2) {
  PolygonEdge *e1 = nullptr, *e2 = nullptr;
  auto* ptr = body_.get();
  do {
    if (DistanceToSegmentSquared(ptr->Begin(), ptr->End(), p1) <
        kMinDistanceFromEdgeSquared) {
      e1 = ptr;
    }
    if (DistanceToSegmentSquared(ptr->Begin(), ptr->End(), p2) <
        kMinDistanceFromEdgeSquared) {
      e2 = ptr;
    }
  } while ((ptr = ptr->Next()) != body_.get());
  if (!e1 || !e2 || e1 == e2)
    return false;
  return e1->SetPerpendicular(e2, 3 * nverticies_);
}

bool Polygon::SetEqualLength(DrawingBoard::Point2d const& p1,
                             DrawingBoard::Point2d const& p2) {
  PolygonEdge *e1 = nullptr, *e2 = nullptr;
  auto* ptr = body_.get();
  do {
    if (DistanceToSegmentSquared(ptr->Begin(), ptr->End(), p1) <
        kMinDistanceFromEdgeSquared) {
      e1 = ptr;
    }
    if (DistanceToSegmentSquared(ptr->Begin(), ptr->End(), p2) <
        kMinDistanceFromEdgeSquared) {
      e2 = ptr;
    }
  } while ((ptr = ptr->Next()) != body_.get());
  if (!e1 || !e2 || e1 == e2)
    return false;
  return e1->SetEqualLength(e2, 3 * nverticies_);
}

std::unique_ptr<Polygon> Polygon::Clone() {
  auto ret = std::make_unique<Polygon>();
  ret->drawing_board_ = drawing_board_;
  ret->nverticies_ = nverticies_;
  auto* ptr = body_.get();
  std::map<PolygonEdge*, PolygonEdge*> map;
  ret->body_.reset(new PolygonEdge(*body_));
  map[body_.get()] = ret->body_.get();
  ptr = ptr->Next();
  do {
    auto* edge = new PolygonEdge(*ptr);
    map[ptr] = edge;
    ret->body_->AddBefore(edge);
  } while ((ptr = ptr->Next()) != body_.get());
  auto* ret_body = ret->body_.get();
  do {
    ret_body->constrained_edge_ = map[ptr->constrained_edge_];
    ret_body->constraint_ = ptr->constraint_;
    ret_body->constraint_id_ = ptr->constraint_id_;

    ret_body = ret_body->Next();
  } while ((ptr = ptr->Next()) != body_.get());
  return ret;
}

bool Polygon::Active() {
  auto* ptr = body_.get();
  do {
    if (ptr->Active())
      return true;
  } while ((ptr = ptr->Next()) != body_.get());
  return false;
}

Polygon::PolygonEdge::~PolygonEdge() {
  if (next_ != this) {
    prev_->next_ = nullptr;
    delete next_;
  }
}

Polygon::PolygonEdge::PolygonEdge(DrawingBoard* drawing_board,
                                  DrawingBoard::Point2d const& begin,
                                  DrawingBoard::Point2d const& end,
                                  COLORREF edge_color,
                                  COLORREF vertex_color)
    : drawing_board_(drawing_board),
      begin_(begin),
      end_(end),
      next_(this),
      prev_(this),
      edge_color_(edge_color),
      vertex_color_(vertex_color) {}

Polygon::PolygonEdge::PolygonEdge(PolygonEdge const& other)
    : drawing_board_(other.drawing_board_),
      begin_(other.begin_),
      end_(other.end_),
      next_(this),
      prev_(this),
      edge_color_(other.edge_color_),
      vertex_color_(other.vertex_color_),
      is_edge_clicked_(other.is_edge_clicked_),
      is_clicked_(other.is_clicked_),
      begin_clicked_(other.begin_clicked_),
      correct_(other.correct_) {}

void Polygon::PolygonEdge::Display() {
  DrawingBoard::Point2d constrained_begin{
      std::max(0.0, std::min<double>(drawing_board_->GetWidth(), begin_.x)),
      std::max(0.0, std::min<double>(drawing_board_->GetWidth(), begin_.y))};
  DrawingBoard::Point2d constrained_end{
      std::max(0.0, std::min<double>(drawing_board_->GetWidth(), end_.x)),
      std::max(0.0, std::min<double>(drawing_board_->GetWidth(), end_.y))};
  int x, y, i, xe, ye;
  int dx = static_cast<int>(end_.x) - static_cast<int>(begin_.x);
  int dy = static_cast<int>(end_.y) - static_cast<int>(begin_.y);
  int dx1 = std::abs(dx);
  int dy1 = std::abs(dy);
  int px = 2 * dy1 - dx1;
  int py = 2 * dx1 - dy1;
  if (dy1 <= dx1) {
    if (dx >= 0) {
      x = begin_.x;
      y = begin_.y;
      xe = constrained_end.x;
    } else {
      x = end_.x;
      y = end_.y;
      xe = constrained_begin.x;
    }
    drawing_board_->SetPixel(x, y, edge_color_);
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
      drawing_board_->SetPixel(x, y, edge_color_);
    }
  } else {
    if (dy >= 0) {
      x = begin_.x;
      y = begin_.y;
      ye = constrained_end.y;
    } else {
      x = end_.x;
      y = end_.y;
      ye = constrained_begin.y;
    }
    drawing_board_->SetPixel(x, y, edge_color_);
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
      drawing_board_->SetPixel(x, y, edge_color_);
    }
  }
  drawing_board_->SetPixel(begin_.x, begin_.y, vertex_color_);
  drawing_board_->SetPixel(end_.x, end_.y, vertex_color_);
  switch (constraint_) {
    case Constraint::PERPENDICULAR:
      DisplayLabel(
          drawing_board_, (begin_ + end_) / 2,
          std::wstring(kUpTack).append(std::to_wstring(constraint_id_)));
      break;
    case Constraint::EQUAL_LENGTH:
      DisplayLabel(
          drawing_board_, (begin_ + end_) / 2,
          std::wstring(kEqualSign).append(std::to_wstring(constraint_id_)));
      break;
  }
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

double Polygon::PolygonEdge::Length() const {
  return std::sqrt(DistanceSquared(begin_, end_));
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
    DrawingBoard::Point2d const& prev_mouse_pos,
    int max_calls) {
  if (is_clicked_) {
    if (mouse_pos == prev_mouse_pos)
      return true;
    if (is_edge_clicked_) {
      MoveByVector(mouse_pos - prev_mouse_pos, max_calls);
    } else if (begin_clicked_) {
      SetBegin(mouse_pos, max_calls);
    } else {
      SetEnd(mouse_pos, max_calls);
    }
    return true;
  }
  return false;
}

bool Polygon::PolygonEdge::MoveWhole(
    DrawingBoard::Point2d const& mouse_pos,
    DrawingBoard::Point2d const& prev_mouse_pos) {
  bool is_clicked = false;
  auto* ptr = this;
  do {
    if (ptr->is_clicked_) {
      if (mouse_pos == prev_mouse_pos)
        return true;
      const DrawingBoard::Point2d vector = mouse_pos - prev_mouse_pos;
      ptr = this;
      do {
        ptr->begin_ = ptr->begin_ + vector;
        ptr->end_ = ptr->end_ + vector;
      } while ((ptr = ptr->Next()) != this);
      return true;
    }
  } while ((ptr = ptr->Next()) != this);
  return false;
}

void Polygon::PolygonEdge::OnControllerStateChanged(
    PolygonController* controller) {
  is_edge_clicked_ = is_clicked_ = begin_clicked_ = false;
}

bool Polygon::PolygonEdge::Split(DrawingBoard::Point2d const& mouse_pos) {
  if (DistanceToSegmentSquared(begin_, end_, mouse_pos) <
      kMinDistanceFromEdgeSquared) {
    RemoveConstraint();
    auto mid = DrawingBoard::Point2d{begin_.x + end_.x, begin_.y + end_.y} / 2;
    auto* new_edge =
        new PolygonEdge(drawing_board_, mid, end_, edge_color_, vertex_color_);
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
                                  PolygonEdge** head,
                                  int max_calls) {
  if (DistanceSquared(end_, point) < kMinDistanceFromVertexSquared) {
    RemoveConstraint();
    next_->RemoveConstraint();
    next_->prev_ = prev_;
    next_->SetBegin(begin_, max_calls - 1);
    prev_->next_ = next_;
    if (*head == this)
      *head = next_;
    prev_ = this;
    next_ = this;
    delete this;
    return true;
  } else if (DistanceSquared(begin_, point) < kMinDistanceFromEdgeSquared) {
    RemoveConstraint();
    prev_->RemoveConstraint();
    prev_->next_ = next_;
    prev_->SetBegin(end_, max_calls - 1);
    next_->prev_ = prev_;
    if (*head == this)
      *head = prev_;
    prev_ = this;
    next_ = this;
    delete this;
    return true;
  } else if (DistanceToSegmentSquared(begin_, end_, point) <
             kMinDistanceFromEdgeSquared) {
    RemoveConstraint();
    return true;
  }
  return false;
}

void Polygon::PolygonEdge::SetBegin(DrawingBoard::Point2d const& begin,
                                    int max_calls) {
  if (begin_ == begin)
    return;
  if (max_calls < 0) {
    SetIncorrect();
    return;
  }
  switch (constraint_) {
    case Constraint::NONE:
      begin_ = begin;
      prev_->SetEnd(begin_, max_calls - 1);
      break;
    case Constraint::PERPENDICULAR:
      if (next_ == constrained_edge_) {
        begin_ = begin;
        constrained_edge_->SetPerpendicularByEnd(this, max_calls - 1);
        prev_->SetEnd(begin_, max_calls - 1);
      } else if (prev_ == constrained_edge_) {
        auto projection_onto_this =
            ((end_ - begin_) * DotProduct(begin - begin_, end_ - begin_)) /
            DistanceSquared(end_, begin_);
        auto projection_onto_prev =
            ((prev_->end_ - prev_->begin_) *
             DotProduct(begin - begin_, prev_->end_ - prev_->begin_)) /
            DistanceSquared(prev_->end_, prev_->begin_);
        prev_->end_ = begin_ = begin_ + projection_onto_this;
        prev_->SetPerpendicularByBegin(this, max_calls - 1);
        end_ = end_ + projection_onto_prev;
        prev_->end_ = begin_ = begin_ + projection_onto_prev;
        SetPerpendicularByEnd(prev_, max_calls - 1);
      } else {
        begin_ = begin;
        constrained_edge_->SetPerpendicularByEnd(this, max_calls - 1);
        prev_->SetEnd(begin_, max_calls - 1);
      }
      break;
    case Constraint::EQUAL_LENGTH: {
      begin_ = begin;
      if (next_ == constrained_edge_) {
        constrained_edge_->SetLengthByEnd(Length(), max_calls - 1);
        prev_->SetEnd(begin_, max_calls - 1);
      } else if (prev_ == constrained_edge_) {
        constrained_edge_->SetLengthByBegin(Length(), max_calls - 1);
        prev_->end_ = begin;
      } else {
        constrained_edge_->SetLengthByBegin(Length(), max_calls - 1);
        prev_->SetEnd(begin_, max_calls - 1);
      }
    } break;
  }
}

void Polygon::PolygonEdge::SetEnd(DrawingBoard::Point2d const& end,
                                  int max_calls) {
  if (end == end_)
    return;
  if (max_calls < 0) {
    SetIncorrect();
    return;
  }
  switch (constraint_) {
    case Constraint::NONE:
      end_ = end;
      next_->SetBegin(end_, max_calls - 1);
      break;
    case Constraint::PERPENDICULAR:
      if (prev_ == constrained_edge_) {
        end_ = end;
        constrained_edge_->SetPerpendicularByBegin(this, max_calls - 1);
        next_->SetBegin(end_, max_calls - 1);
      } else if (next_ == constrained_edge_) {
        auto projection_onto_this =
            ((end_ - begin_) * DotProduct(end - end_, end_ - begin_)) /
            DistanceSquared(end_, begin_);
        auto projection_onto_next =
            ((next_->end_ - next_->begin_) *
             DotProduct(end - end_, next_->end_ - next_->begin_)) /
            DistanceSquared(next_->end_, next_->begin_);
        next_->begin_ = end_ = end_ + projection_onto_this;
        next_->SetPerpendicularByEnd(this, max_calls - 1);
        begin_ = begin_ + projection_onto_next;
        next_->begin_ = end_ = end_ + projection_onto_next;
        SetPerpendicularByBegin(next_, max_calls - 1);
      } else {
        end_ = end;
        constrained_edge_->SetPerpendicularByBegin(this, max_calls - 1);
        next_->SetBegin(end_, max_calls - 1);
      }
      break;
    case Constraint::EQUAL_LENGTH: {
      end_ = end;
      if (prev_ == constrained_edge_) {
        constrained_edge_->SetLengthByBegin(Length(), max_calls - 1);
        next_->SetBegin(end_, max_calls - 1);
      } else if (next_ == constrained_edge_) {
        constrained_edge_->SetLengthByEnd(Length(), max_calls - 1);
        next_->begin_ = end;
      } else {
        constrained_edge_->SetLengthByEnd(Length(), max_calls - 1);
        next_->SetBegin(end_, max_calls - 1);
      }
    } break;
  }
}

void Polygon::PolygonEdge::MoveByVector(DrawingBoard::Point2d const& vector,
                                        int max_calls) {
  if (vector == DrawingBoard::Point2d{0, 0})
    return;
  switch (constraint_) {
    case Constraint::PERPENDICULAR:
    case Constraint::EQUAL_LENGTH:
    case Constraint::NONE:
      begin_ = begin_ + vector;
      end_ = end_ + vector;
      next_->SetBegin(end_, max_calls - 1);
      prev_->SetEnd(begin_, max_calls - 1);
      break;
  }
}

bool Polygon::PolygonEdge::SetPerpendicular(PolygonEdge* edge, int max_calls) {
  if (constraint_ != Constraint::NONE ||
      edge->constraint_ != Constraint::NONE || constrained_edge_ ||
      edge->constrained_edge_)
    return false;
  if (edge->next_ == this || edge->prev_ == this) {
    if (edge->next_ != this)
      return edge->SetPerpendicular(this, max_calls - 1);
    constraint_ = edge->constraint_ = Constraint::PERPENDICULAR;
    constrained_edge_ = edge;
    edge->constrained_edge_ = this;
    constraint_id_ = edge->constraint_id_ = id_manager::Get();
    const auto circle_center = (end_ + edge->begin_) / 2;
    if (begin_ == circle_center) {
      begin_ = edge->end_ =
          (edge->begin_ + end_ * DrawingBoard::Point2d{0, 1}) /
          DrawingBoard::Point2d{0, 1};
    } else {
      begin_ = edge->end_ = ClosestPointOnCircle(
          circle_center, std::sqrt(DistanceSquared(end_, circle_center)),
          begin_);
    }
  } else {
    constraint_ = edge->constraint_ = Constraint::PERPENDICULAR;
    constrained_edge_ = edge;
    edge->constrained_edge_ = this;
    constraint_id_ = edge->constraint_id_ = id_manager::Get();
    edge->SetPerpendicularByBegin(this, max_calls);
  }
  return true;
}

bool Polygon::PolygonEdge::SetEqualLength(PolygonEdge* edge, int max_calls) {
  if (constraint_ != Constraint::NONE ||
      edge->constraint_ != Constraint::NONE || constrained_edge_ ||
      edge->constrained_edge_)
    return false;
  constraint_ = edge->constraint_ = Constraint::EQUAL_LENGTH;
  constrained_edge_ = edge;
  edge->constrained_edge_ = this;
  constraint_id_ = edge->constraint_id_ = id_manager::Get();
  if (edge == next_)
    edge->SetLengthByBegin(Length(), max_calls);
  else if (edge == prev_)
    edge->SetLengthByEnd(Length(), max_calls);
  else
    edge->SetLengthByBegin(Length(), max_calls);

  return true;
}

void Polygon::PolygonEdge::RemoveConstraint() {
  id_manager::Release(constraint_id_);
  constraint_id_ = 0;
  constraint_ = Constraint::NONE;
  if (constrained_edge_) {
    constrained_edge_->constraint_id_ = 0;
    constrained_edge_->constraint_ = Constraint::NONE;
    constrained_edge_->constrained_edge_ = nullptr;
    constrained_edge_ = nullptr;
  }
}

void Polygon::PolygonEdge::SetLengthByBegin(double length, int max_calls) {
  if (std::abs(length * length - DistanceSquared(begin_, end_)) <
      kVerySmallValue)
    return;
  auto vec = begin_ - end_;
  vec = vec / Length();
  vec = vec * length;
  begin_ = end_ + vec;
  if (prev_->constraint_ == Constraint::EQUAL_LENGTH) {
    auto intersection =
        CircleIntersection(end_, prev_->begin_, begin_, prev_->end_);
    if (intersection.has_value()) {
      if (DistanceSquared(begin_, intersection->p1) <
          DistanceSquared(begin_, intersection->p2)) {
        prev_->end_ = begin_ = intersection->p1;
      } else {
        prev_->end_ = begin_ = intersection->p2;
      }
      return;
    }
  }
  prev_->SetEnd(begin_, max_calls - 1);
}

void Polygon::PolygonEdge::SetLengthByEnd(double length, int max_calls) {
  if (std::abs(length * length - DistanceSquared(begin_, end_)) <
      kVerySmallValue)
    return;
  auto vec = end_ - begin_;
  vec = vec / Length();
  vec = vec * length;
  end_ = begin_ + vec;
  if (next_->constraint_ == Constraint::EQUAL_LENGTH) {
    auto intersection =
        CircleIntersection(begin_, next_->end_, end_, next_->begin_);
    if (intersection.has_value()) {
      if (DistanceSquared(end_, intersection->p1) <
          DistanceSquared(end_, intersection->p2)) {
        next_->begin_ = end_ = intersection->p1;
      } else {
        next_->begin_ = end_ = intersection->p2;
      }
      return;
    }
  }
  next_->SetBegin(end_, max_calls - 1);
}

void Polygon::PolygonEdge::SetPerpendicularByBegin(PolygonEdge* edge,
                                                   int max_calls) {
  auto vec = edge->end_ - edge->begin_;
  vec = vec / std::sqrt(DistanceSquared(vec, {0, 0}));
  vec = vec * Length();
  vec = vec * DrawingBoard::Point2d{0, 1};
  if (DistanceSquared(begin_, end_ + vec) < DistanceSquared(begin_, end_ - vec))
    begin_ = end_ + vec;
  else
    begin_ = end_ - vec;
  if (Colinear(begin_, end_, prev_->end_)) {
    begin_ = prev_->end_;
  } else {
    if (prev_->constraint_ == Constraint::PERPENDICULAR) {
      auto intersection =
          IntersectLines(begin_, end_, prev_->begin_, prev_->end_);
      if (intersection.has_value()) {
        begin_ = prev_->end_ = intersection.value();
      } else {
        RemoveConstraint();
        SetBegin(prev_->end_, max_calls - 1);
      }
    } else {
      prev_->SetEnd(begin_, max_calls - 1);
    }
  }
}

void Polygon::PolygonEdge::SetPerpendicularByEnd(PolygonEdge* edge,
                                                 int max_calls) {
  auto vec = edge->end_ - edge->begin_;
  vec = vec / std::sqrt(DistanceSquared(vec, {0, 0}));
  vec = vec * Length();
  vec = vec * DrawingBoard::Point2d{0, 1};
  if (DistanceSquared(end_, begin_ + vec) < DistanceSquared(end_, begin_ - vec))
    end_ = begin_ + vec;
  else
    end_ = begin_ - vec;
  if (Colinear(begin_, end_, next_->begin_)) {
    end_ = next_->begin_;
  } else {
    if (next_->constraint_ == Constraint::PERPENDICULAR) {
      auto intersection =
          IntersectLines(begin_, end_, next_->begin_, next_->end_);
      if (intersection.has_value()) {
        end_ = next_->begin_ = intersection.value();
      } else {
        RemoveConstraint();
        SetBegin(next_->begin_, max_calls - 1);
      }
    } else {
      next_->SetBegin(end_, max_calls - 1);
    }
  }
}

void Polygon::PolygonEdge::SetIncorrect() {
  if (!correct_)
    return;
  correct_ = false;
  next_->SetIncorrect();
  prev_->SetIncorrect();
}
}  // namespace gk
