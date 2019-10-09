// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

#include <memory>
#include <optional>

#include "../drawing_board.hpp"
#include "line.hpp"

namespace gk {
class Polygon;

class PolygonLine : public Line {
 public:
  static std::unique_ptr<PolygonLine> Create(
      DrawingBoard::CoordinatePair const& p1,
      DrawingBoard::CoordinatePair const& p2,
      DrawingBoard::CoordinatePair const& p3,
      COLORREF color,
      COLORREF vertex_color);

  ~PolygonLine() override;

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

  PolygonLine* GetNext() { return next_; }

 private:
  PolygonLine(DrawingBoard::CoordinatePair const& begin,
              DrawingBoard::CoordinatePair const& end,
              COLORREF color);
  PolygonLine(DrawingBoard::CoordinatePair const& begin,
              DrawingBoard::CoordinatePair const& end,
              COLORREF color,
              COLORREF vertex_color);

  void AddBefore(PolygonLine const& line);
  void AddAfter(PolygonLine const& line);
  void DeletePrev();

  PolygonLine* next_ = this;
  PolygonLine* prev_ = this;

  bool vertex_clicked_ = false;
  bool begin_clicked_ = false;
  bool line_clicked_ = false;

  std::optional<DrawingBoard::CoordinatePair> last_mouse_pos_;
};
}  // namespace gk
