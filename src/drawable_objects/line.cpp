// Copyright Wojciech Replin 2019

#include "line.hpp"

namespace gk {
namespace {
std::list<DrawingBoard::CoordinatePair> Bresenham(
    DrawingBoard::CoordinatePair begin,
    DrawingBoard::CoordinatePair end) {
  std::list<DrawingBoard::CoordinatePair> ret;
  DrawingBoard::Coordinate x, y, i, xe, ye;
  DrawingBoard::Coordinate dx = end.first - begin.first;
  DrawingBoard::Coordinate dy = end.second - begin.second;
  DrawingBoard::Coordinate dx1 = std::abs(dx);
  DrawingBoard::Coordinate dy1 = std::abs(dy);
  DrawingBoard::Coordinate px = 2 * dy1 - dx1;
  DrawingBoard::Coordinate py = 2 * dx1 - dy1;
  if (dy1 <= dx1) {
    if (dx >= 0) {
      x = begin.first;
      y = begin.second;
      xe = end.first;
    } else {
      x = end.first;
      y = end.second;
      xe = begin.first;
    }
    ret.emplace_back(x, y);
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
      ret.emplace_back(x, y);
    }
  } else {
    if (dy >= 0) {
      x = begin.first;
      y = begin.second;
      ye = end.second;
    } else {
      x = end.first;
      y = end.second;
      ye = begin.second;
    }
    ret.emplace_back(x, y);
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
      ret.emplace_back(x, y);
    }
  }
  return ret;
}
}  // namespace

Line::Line(DrawingBoard::CoordinatePair const& begin,
           DrawingBoard::CoordinatePair const& end,
           COLORREF color)
    : begin_(begin), end_(end), color_(color), vertex_color_(color) {}

Line::Line(DrawingBoard::CoordinatePair const& begin,
           DrawingBoard::CoordinatePair const& end,
           COLORREF color,
           COLORREF vertex_color)
    : begin_(begin), end_(end), color_(color), vertex_color_(vertex_color) {}

Line::~Line() = default;

void Line::Display(DrawingBoard* board) {
  if (line_points_.empty())
    Recalculate();
  for (auto& point : line_points_)
    board->SetPixel(point.first, point.second, color_);
  board->SetPixel(begin_.first, begin_.second, vertex_color_);
  board->SetPixel(end_.first, end_.second, vertex_color_);
}

void Line::SetBegin(DrawingBoard::CoordinatePair const& val) {
  if (begin_ != val) {
    begin_ = val;
    Recalculate();
  }
}

void Line::SetEnd(DrawingBoard::CoordinatePair const& val) {
  if (end_ != val) {
    end_ = val;
    Recalculate();
  }
}

void Line::MoveByVector(DrawingBoard::CoordinatePair const& vector) {
  if (vector != DrawingBoard::CoordinatePair{0, 0}) {
    begin_ = {begin_.first + vector.first, begin_.second + vector.second};
    end_ = {end_.first + vector.first, end_.second + vector.second};
    Recalculate();
  }
}

void Line::Recalculate() {
  line_points_ = Bresenham(begin_, end_);
}

bool Line::Contains(DrawingBoard::CoordinatePair const& point) {
  return std::find(line_points_.begin(), line_points_.end(), point) !=
         line_points_.end();
}

bool Line::IsVertex(DrawingBoard::CoordinatePair const& point) {
  return point == begin_ || point == end_;
}
}  // namespace gk
