// Copyright Wojciech Replin 2019

#include "line.hpp"

#include <algorithm>
#include <cmath>

namespace gk {
namespace {

std::list<Line::Vertex> Bresenham(Line::Vertex begin, Line::Vertex end) {
  std::list<Line::Vertex> ret;

  const bool steep =
      std::abs(begin.second - end.second) > std::abs(begin.first - end.first);

  if (steep) {
    begin = Line::Vertex{begin.second, begin.first};
    end = Line::Vertex{end.second, end.first};
  }

  if (begin.first > end.first)
    std::swap(begin, end);

  const double dx = static_cast<double>(end.first) - begin.first;
  const DrawingBoard::Coordinate dy = std::abs(begin.second - end.second);

  double err = dx / 2.f;
  const int ystep = (begin.second < end.second) ? 1 : -1;
  DrawingBoard::Coordinate y = begin.second;

  const DrawingBoard::Coordinate max_x = end.first;

  for (DrawingBoard::Coordinate x = begin.first; x < max_x; ++x) {
    ret.emplace_back(steep ? Line::Vertex{y, x} : Line::Vertex{x, y});
    err -= dy;
    if (err < 0) {
      y += ystep;
      err += dx;
    }
  }
  return ret;
}
}  // namespace

Line::Line(Vertex const& begin, Vertex const& end, COLORREF color)
    : begin_(begin),
      end_(end),
      color_(color),
      vertex_color_(color),
      line_points_(Bresenham(begin_, end_)),
      vertex_clicked_(false),
      line_clicked_(false) {}

Line::Line(Vertex const& begin,
           Vertex const& end,
           COLORREF color,
           COLORREF vertex_color)
    : begin_(begin),
      end_(end),
      color_(color),
      vertex_color_(vertex_color),
      line_points_(Bresenham(begin_, end_)),
      vertex_clicked_(false),
      line_clicked_(false) {}

Line::~Line() = default;

void Line::Display(DrawingBoard* board) {
  for (auto& point : line_points_)
    board->SetPixel(point.first, point.second, color_);
  board->SetPixel(begin_.first, begin_.second, vertex_color_);
  board->SetPixel(end_.first, end_.second, vertex_color_);
}

bool Line::OnMouseLButtonDown(DrawingBoard* board, POINT mouse_pos) {
  if (mouse_pos.x == begin_.first && mouse_pos.y == begin_.second) {
    vertex_clicked_ = true;
    clicked_vertex_ = &begin_;
    line_clicked_ = false;
    last_mouse_pos_.emplace(mouse_pos.x, mouse_pos.y);
  } else if (mouse_pos.x == end_.first && mouse_pos.y == end_.second) {
    vertex_clicked_ = true;
    clicked_vertex_ = &end_;
    line_clicked_ = false;
    last_mouse_pos_.emplace(mouse_pos.x, mouse_pos.y);
  } else if (std::find_if(line_points_.begin(), line_points_.end(),
                          [&mouse_pos](auto const& elem) {
                            return elem.first == mouse_pos.x &&
                                   elem.second == mouse_pos.y;
                          }) != line_points_.end()) {
    vertex_clicked_ = false;
    line_clicked_ = true;
    last_mouse_pos_.emplace(mouse_pos.x, mouse_pos.y);
  }
  return false;
}

bool Line::OnMouseLButtonUp(DrawingBoard* board, POINT mouse_pos) {
  vertex_clicked_ = line_clicked_ = false;
  last_mouse_pos_.reset();
  return false;
}

bool Line::OnMouseMove(DrawingBoard* board, POINT mouse_pos) {
  if (vertex_clicked_) {
    *clicked_vertex_ = Vertex{mouse_pos.x, mouse_pos.y};
    line_points_ = Bresenham(begin_, end_);
  } else if (line_clicked_) {
    begin_ =
        Vertex{begin_.first + mouse_pos.x - last_mouse_pos_.value().first,
               begin_.second + mouse_pos.y - last_mouse_pos_.value().second};
    end_ = Vertex{end_.first + mouse_pos.x - last_mouse_pos_.value().first,
                  end_.second + mouse_pos.y - last_mouse_pos_.value().second};
    last_mouse_pos_.emplace(mouse_pos.x, mouse_pos.y);
    line_points_ = Bresenham(begin_, end_);
  }
  return vertex_clicked_ || line_clicked_;
}

}  // namespace gk
