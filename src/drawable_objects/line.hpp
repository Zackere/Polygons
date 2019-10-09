// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

#include <list>
#include <optional>

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
  bool OnMouseLButtonDown(DrawingBoard* board,
                          DrawingBoard::CoordinatePair const& mouse_pos) = 0;
  bool OnMouseLButtonUp(DrawingBoard* board,
                        DrawingBoard::CoordinatePair const& mouse_pos) = 0;
  bool OnMouseMove(DrawingBoard* board,
                   DrawingBoard::CoordinatePair const& mouse_pos) = 0;
  bool Contains(DrawingBoard::CoordinatePair const& point) override;
  bool IsVertex(DrawingBoard::CoordinatePair const& point) override;
  bool RequestRemoval(DrawingBoard::CoordinatePair const& point) = 0;
  bool AddVertex(DrawingBoard::CoordinatePair const& point,
                 Controller* controller) = 0;
  void OnControllerStateChanged(Controller* controller) = 0;

  DrawingBoard::CoordinatePair const& GetBegin() const { return begin_; }
  DrawingBoard::CoordinatePair const& GetEnd() const { return end_; }
  void SetBegin(DrawingBoard::CoordinatePair const& val);
  void SetEnd(DrawingBoard::CoordinatePair const& val);
  void MoveByVector(DrawingBoard::CoordinatePair const& vector);
  COLORREF GetColor() const { return color_; }
  COLORREF GetVertexColor() const { return vertex_color_; }

 private:
  void Recalculate();

  DrawingBoard::CoordinatePair begin_;
  DrawingBoard::CoordinatePair end_;
  COLORREF color_;
  COLORREF vertex_color_;

  std::list<DrawingBoard::CoordinatePair> line_points_;
};
}  // namespace gk
