// Copyright Wojciech Replin 2019

#include "line.hpp"

#include <algorithm>
#include <cmath>

#include "../controllers/controller.hpp"

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
    : begin_(begin),
      end_(end),
      color_(color),
      vertex_color_(color),
      line_points_(Bresenham(begin_, end_)),
      vertex_clicked_(false),
      line_clicked_(false) {}

Line::Line(DrawingBoard::CoordinatePair const& begin,
           DrawingBoard::CoordinatePair const& end,
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

bool Line::OnMouseLButtonDown(DrawingBoard* board,
                              DrawingBoard::CoordinatePair const& mouse_pos) {
  if (mouse_pos.first == begin_.first && mouse_pos.second == begin_.second) {
    vertex_clicked_ = true;
    clicked_vertex_ = &begin_;
    line_clicked_ = false;
    last_mouse_pos_.emplace(mouse_pos.first, mouse_pos.second);
  } else if (mouse_pos.first == end_.first && mouse_pos.second == end_.second) {
    vertex_clicked_ = true;
    clicked_vertex_ = &end_;
    line_clicked_ = false;
    last_mouse_pos_.emplace(mouse_pos.first, mouse_pos.second);
  } else if (std::find_if(line_points_.begin(), line_points_.end(),
                          [&mouse_pos](auto const& elem) {
                            return elem.first == mouse_pos.first &&
                                   elem.second == mouse_pos.second;
                          }) != line_points_.end()) {
    vertex_clicked_ = false;
    line_clicked_ = true;
    last_mouse_pos_.emplace(mouse_pos.first, mouse_pos.second);
  }
  return false;
}

bool Line::OnMouseLButtonUp(DrawingBoard* board,
                            DrawingBoard::CoordinatePair const& mouse_pos) {
  vertex_clicked_ = line_clicked_ = false;
  last_mouse_pos_.reset();
  return false;
}

bool Line::OnMouseMove(DrawingBoard* board,
                       DrawingBoard::CoordinatePair const& mouse_pos) {
  if (vertex_clicked_) {
    *clicked_vertex_ =
        DrawingBoard::CoordinatePair{mouse_pos.first, mouse_pos.second};
    line_points_ = Bresenham(begin_, end_);
  } else if (line_clicked_) {
    begin_ = DrawingBoard::CoordinatePair{
        begin_.first + mouse_pos.first - last_mouse_pos_.value().first,
        begin_.second + mouse_pos.second - last_mouse_pos_.value().second};
    end_ = DrawingBoard::CoordinatePair{
        end_.first + mouse_pos.first - last_mouse_pos_.value().first,
        end_.second + mouse_pos.second - last_mouse_pos_.value().second};
    last_mouse_pos_.emplace(mouse_pos.first, mouse_pos.second);
    line_points_ = Bresenham(begin_, end_);
  }
  return vertex_clicked_ || line_clicked_;
}

bool Line::Contains(DrawingBoard::CoordinatePair const& point) {
  if (line_points_.empty())
    line_points_ = Bresenham(begin_, end_);
  return std::find(line_points_.begin(), line_points_.end(), point) !=
         line_points_.end();
}

bool Line::IsVertex(DrawingBoard::CoordinatePair const& point) {
  return point == begin_ || point == end_;
}

bool Line::RequestRemoval(DrawingBoard::CoordinatePair const& point) {
  return !Contains(point);
}

bool Line::AddVertex(DrawingBoard::CoordinatePair const& point,
                     Controller* controller) {
  if (Contains(point) && !IsVertex(point)) {
    controller->AddObject(
        std::make_unique<Line>(begin_, point, color_, vertex_color_));
    controller->AddObject(
        std::make_unique<Line>(point, end_, color_, vertex_color_));
    return false;
  }
  return true;
}

}  // namespace gk
