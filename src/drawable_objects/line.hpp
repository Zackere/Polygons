// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

#include <list>
#include <utility>
#include <optional>

#include "../drawing_board.hpp"

namespace gk {
class Line : public DrawingBoard::DrawableObject {
 public:
  using Vertex = std::pair<DrawingBoard::Coordinate, DrawingBoard::Coordinate>;

  Line(Vertex const& begin, Vertex const& end, COLORREF color);
  Line(Vertex const& begin,
       Vertex const& end,
       COLORREF color,
       COLORREF vertex_color);
  ~Line() override;

  // Overridden from DrawingBoard::DrawableObject
  void Display(DrawingBoard* board) override;
  bool OnMouseLButtonDown(DrawingBoard* board, DrawingBoard::CoordinatePair mouse_pos) override;
  bool OnMouseLButtonUp(DrawingBoard* board,
                        DrawingBoard::CoordinatePair mouse_pos) override;
  bool OnMouseMove(DrawingBoard* board,
                   DrawingBoard::CoordinatePair mouse_pos) override;

 private:
  Vertex begin_;
  Vertex end_;
  COLORREF color_;
  COLORREF vertex_color_;

  std::list<Vertex> line_points_;

  bool vertex_clicked_ = false;
  Vertex* clicked_vertex_ = nullptr;
  bool line_clicked_ = false;

  std::optional<Vertex> last_mouse_pos_;
};
}  // namespace gk
