// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

#include <memory>

#include "../drawing_board/drawing_board.hpp"

namespace gk {
class PolygonController;

class Polygon {
 public:
  static std::unique_ptr<Polygon> Create(DrawingBoard::CoordinatePair const& p1,
                                         DrawingBoard::CoordinatePair const& p2,
                                         DrawingBoard::CoordinatePair const& p3,
                                         COLORREF edge_color,
                                         COLORREF vertex_color);

  void Display(DrawingBoard* drawing_board);
  bool OnMouseLButtonDown(DrawingBoard* drawing_board,
                          DrawingBoard::CoordinatePair const& mouse_pos);
  bool OnMouseLButtonUp(DrawingBoard* drawing_board,
                        DrawingBoard::CoordinatePair const& mouse_pos);
  bool OnMouseMove(DrawingBoard* drawing_board,
                   DrawingBoard::CoordinatePair const& mouse_pos);
  void OnControllerStateChanged(PolygonController* controller);
  bool AddVertex(DrawingBoard::CoordinatePair const& pos);
  bool Remove(DrawingBoard::CoordinatePair const& point);

 private:
  class PolygonEdge {
   public:
    ~PolygonEdge();
    PolygonEdge(std::shared_ptr<DrawingBoard::CoordinatePair> begin,
                std::shared_ptr<DrawingBoard::CoordinatePair> end,
                COLORREF edge_color,
                COLORREF vertex_color);
    void Display(DrawingBoard* drawing_board);
    void AddAfter(PolygonEdge* edge);
    void AddBefore(PolygonEdge* point);
    PolygonEdge* Next() { return next_; }

    bool OnMouseLButtonDown(DrawingBoard::CoordinatePair const& mouse_pos);
    bool OnMouseLButtonUp(DrawingBoard::CoordinatePair const& mouse_pos);
    bool OnMouseMove(DrawingBoard::CoordinatePair const& mouse_pos,
                     DrawingBoard::CoordinatePair const& prev_mouse_pos);
    void OnControllerStateChanged(PolygonController* controller);
    bool Split(DrawingBoard::CoordinatePair const& mouse_pos);
    bool Remove(DrawingBoard::CoordinatePair const& point, PolygonEdge** head);

   private:
    std::shared_ptr<DrawingBoard::CoordinatePair> begin_, end_;
    PolygonEdge *next_ = this, *prev_ = this;
    COLORREF edge_color_ = 0, vertex_color_ = 0;

    bool is_edge_clicked_ = false;
    bool is_clicked_ = false;
    bool begin_clicked_ = false;
  };

  std::unique_ptr<PolygonEdge> body_;
};
}  // namespace gk
