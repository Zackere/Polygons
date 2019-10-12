// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

#include <memory>

#include "../drawing_board/drawing_board.hpp"

namespace gk {
class PolygonController;

class Polygon {
 public:
  static std::unique_ptr<Polygon> Create(DrawingBoard::Point2d const& p1,
                                         DrawingBoard::Point2d const& p2,
                                         DrawingBoard::Point2d const& p3,
                                         COLORREF edge_color,
                                         COLORREF vertex_color);

  void Display(DrawingBoard* drawing_board);
  bool OnMouseLButtonDown(DrawingBoard* drawing_board,
                          DrawingBoard::Point2d const& mouse_pos);
  bool OnMouseLButtonUp(DrawingBoard* drawing_board,
                        DrawingBoard::Point2d const& mouse_pos);
  bool OnMouseMove(DrawingBoard* drawing_board,
                   DrawingBoard::Point2d const& mouse_pos);
  void OnControllerStateChanged(PolygonController* controller);
  bool AddVertex(DrawingBoard::Point2d const& pos);
  bool Remove(DrawingBoard::Point2d const& point);

 private:
  class PolygonEdge {
   public:
    ~PolygonEdge();
    PolygonEdge(DrawingBoard::Point2d const& begin,
                DrawingBoard::Point2d const& end,
                COLORREF edge_color,
                COLORREF vertex_color);
    void Display(DrawingBoard* drawing_board);
    void AddAfter(PolygonEdge* edge);
    void AddBefore(PolygonEdge* point);
    PolygonEdge* Next() { return next_; }

    bool OnMouseLButtonDown(DrawingBoard::Point2d const& mouse_pos);
    bool OnMouseLButtonUp(DrawingBoard::Point2d const& mouse_pos);
    bool OnMouseMove(DrawingBoard::Point2d const& mouse_pos,
                     DrawingBoard::Point2d const& prev_mouse_pos);
    void OnControllerStateChanged(PolygonController* controller);
    bool Split(DrawingBoard::Point2d const& mouse_pos);
    bool Remove(DrawingBoard::Point2d const& point, PolygonEdge** head);
    void SetBegin(DrawingBoard::Point2d const& begin);
    void SetEnd(DrawingBoard::Point2d const& end);

   private:
    DrawingBoard::Point2d begin_, end_;
    PolygonEdge *next_ = this, *prev_ = this;
    COLORREF edge_color_ = 0, vertex_color_ = 0;

    bool is_edge_clicked_ = false;
    bool is_clicked_ = false;
    bool begin_clicked_ = false;
  };

  std::unique_ptr<PolygonEdge> body_;
};
}  // namespace gk
