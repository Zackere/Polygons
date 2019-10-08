// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

#include <utility>

#include "../drawing_board.hpp"
#include "drawable_object.hpp"
#include "line.hpp"

namespace gk {
class Polygon : public DrawableObject {
 public:
  Polygon(DrawingBoard::CoordinatePair const& p1,
          DrawingBoard::CoordinatePair const& p2,
          DrawingBoard::CoordinatePair const& p3,
          COLORREF color,
          COLORREF vertex_color);
  ~Polygon() override;

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
  void OnControllerStateChanged(Controller* controller) override;

 private:
  class PolygonLine : public Line {};

  COLORREF color_;
  COLORREF vertex_color_;

  bool clicked_ = false;
};
}  // namespace gk
