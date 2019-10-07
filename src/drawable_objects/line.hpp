// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

#include <list>
#include <optional>
#include <utility>

#include "../drawing_board.hpp"
#include "drawable_object.hpp"

namespace gk {
class Line : public DrawableObject {
 public:
  Line(DrawingBoard::CoordinatePair const& begin,
       DrawingBoard::CoordinatePair const& end,
       COLORREF color);
  Line(DrawingBoard::CoordinatePair const& begin,
       DrawingBoard::CoordinatePair const& end,
       COLORREF color,
       COLORREF vertex_color);
  ~Line() override;

  // Overridden from DrawingBoard::DrawableObject
  void Display(DrawingBoard* board) override;
  bool OnMouseLButtonDown(
      DrawingBoard* board,
      DrawingBoard::CoordinatePair const& mouse_pos) override;
  bool OnMouseLButtonUp(DrawingBoard* board,
                        DrawingBoard::CoordinatePair const& mouse_pos) override;
  bool OnMouseMove(DrawingBoard* board,
                   DrawingBoard::CoordinatePair const& mouse_pos) override;
  bool Contains(DrawingBoard::CoordinatePair const& point) override;
  bool IsVertex(DrawingBoard::CoordinatePair const& point) override;
  bool RequestRemoval(DrawingBoard::CoordinatePair const& point) override;
  bool AddVertex(DrawingBoard::CoordinatePair const& point,
                 Controller* controller) override;

 private:
  DrawingBoard::CoordinatePair begin_;
  DrawingBoard::CoordinatePair end_;
  COLORREF color_;
  COLORREF vertex_color_;

  std::list<DrawingBoard::CoordinatePair> line_points_;

  bool vertex_clicked_ = false;
  DrawingBoard::CoordinatePair* clicked_vertex_ = nullptr;
  bool line_clicked_ = false;

  std::optional<DrawingBoard::CoordinatePair> last_mouse_pos_;
};
}  // namespace gk
