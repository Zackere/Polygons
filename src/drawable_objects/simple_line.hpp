// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

#include <list>
#include <optional>

#include "../drawing_board.hpp"
#include "line.hpp"

namespace gk {
class SimpleLine : public Line {
 public:
  SimpleLine(DrawingBoard::CoordinatePair const& begin,
             DrawingBoard::CoordinatePair const& end,
             COLORREF color);
  SimpleLine(DrawingBoard::CoordinatePair const& begin,
             DrawingBoard::CoordinatePair const& end,
             COLORREF color,
             COLORREF vertex_color);
  ~SimpleLine() override;

  // Overridden from DrawingBoard::DrawableObject
  void Display(DrawingBoard* board) override;
  bool OnMouseLButtonDown(
      DrawingBoard* board,
      DrawingBoard::CoordinatePair const& mouse_pos) override;
  bool OnMouseLButtonUp(DrawingBoard* board,
                        DrawingBoard::CoordinatePair const& mouse_pos) override;
  bool OnMouseMove(DrawingBoard* board,
                   DrawingBoard::CoordinatePair const& mouse_pos) override;
  bool IsVertex(DrawingBoard::CoordinatePair const& point) override;
  bool RequestRemoval(DrawingBoard::CoordinatePair const& point) override;
  bool AddVertex(DrawingBoard::CoordinatePair const& point,
                 Controller* controller) override;
  void OnControllerStateChanged(Controller* controller) override;

 private:
  bool vertex_clicked_ = false;
  bool begin_clicked_ = false;
  bool line_clicked_ = false;

  std::optional<DrawingBoard::CoordinatePair> last_mouse_pos_;
};
}  // namespace gk
