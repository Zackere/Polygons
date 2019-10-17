// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

#include <memory>

#include "../drawing_board/drawing_board.hpp"
#include "../id_manager/id_manager.hpp"

namespace gk {
class PolygonController;

class Polygon {
 public:
  static std::unique_ptr<Polygon> CreateSamplePolygon(
      DrawingBoard* drawing_board);
  static std::unique_ptr<Polygon> Create(DrawingBoard* drawing_board,
                                         DrawingBoard::Point2d const& p1,
                                         DrawingBoard::Point2d const& p2,
                                         DrawingBoard::Point2d const& p3,
                                         COLORREF edge_color,
                                         COLORREF vertex_color);

  void Display();
  bool OnMouseLButtonDown(DrawingBoard::Point2d const& mouse_pos);
  bool OnMouseLButtonUp(DrawingBoard::Point2d const& mouse_pos);
  bool OnMouseMove(DrawingBoard::Point2d const& mouse_pos, bool move_whole);
  void OnControllerStateChanged(PolygonController* controller);
  bool AddVertex(DrawingBoard::Point2d const& pos);
  bool Remove(DrawingBoard::Point2d const& point);
  bool SetPerpendicular(DrawingBoard::Point2d const& p1,
                        DrawingBoard::Point2d const& p2);
  bool SetEqualLength(DrawingBoard::Point2d const& p1,
                      DrawingBoard::Point2d const& p2);

  std::unique_ptr<Polygon> Clone();
  bool Correct() { return body_->Correct(); }
  bool Active();

 private:
  class PolygonEdge {
   public:
    friend std::unique_ptr<Polygon> Polygon::CreateSamplePolygon(
        DrawingBoard* drawing_board);
    friend std::unique_ptr<Polygon> Polygon::Clone();

    ~PolygonEdge();
    PolygonEdge(DrawingBoard* drawing_board,
                DrawingBoard::Point2d const& begin,
                DrawingBoard::Point2d const& end,
                COLORREF edge_color,
                COLORREF vertex_color);
    PolygonEdge(PolygonEdge const& other);

    void Display();
    void AddAfter(PolygonEdge* edge);
    void AddBefore(PolygonEdge* point);
    PolygonEdge* Next() { return next_; }
    DrawingBoard::Point2d const& Begin() const { return begin_; }
    DrawingBoard::Point2d const& End() const { return end_; }
    double Length() const;
    bool Correct() { return correct_; }
    bool Active() { return is_clicked_; }

    bool OnMouseLButtonDown(DrawingBoard::Point2d const& mouse_pos);
    bool OnMouseLButtonUp(DrawingBoard::Point2d const& mouse_pos);
    bool OnMouseMove(DrawingBoard::Point2d const& mouse_pos,
                     DrawingBoard::Point2d const& prev_mouse_pos,
                     int max_calls);
    bool MoveWhole(DrawingBoard::Point2d const& mouse_pos,
                   DrawingBoard::Point2d const& prev_mouse_pos);
    void OnControllerStateChanged(PolygonController* controller);
    bool Split(DrawingBoard::Point2d const& mouse_pos);
    bool Remove(DrawingBoard::Point2d const& point,
                PolygonEdge** head,
                int max_calls);
    void SetBegin(DrawingBoard::Point2d const& begin, int max_calls);
    void SetEnd(DrawingBoard::Point2d const& end, int max_calls);
    void MoveByVector(DrawingBoard::Point2d const& vector, int max_calls);

    bool SetPerpendicular(PolygonEdge* edge, int max_calls);
    bool SetEqualLength(PolygonEdge* edge, int max_calls);
    void RemoveConstraint();

   private:
    void SetLengthByBegin(double length, int max_calls);
    void SetLengthByEnd(double length, int max_calls);
    void SetPerpendicularByBegin(PolygonEdge* edge, int max_calls);
    void SetPerpendicularByEnd(PolygonEdge* edge, int max_calls);

    void SetIncorrect();

    enum class Constraint {
      NONE,
      PERPENDICULAR,
      EQUAL_LENGTH,
    };

    DrawingBoard* drawing_board_;

    DrawingBoard::Point2d begin_, end_;
    PolygonEdge *next_ = this, *prev_ = this;
    COLORREF edge_color_ = 0, vertex_color_ = 0;

    bool is_edge_clicked_ = false;
    bool is_clicked_ = false;
    bool begin_clicked_ = false;

    Constraint constraint_ = Constraint::NONE;
    PolygonEdge* constrained_edge_ = nullptr;
    id_manager::ID constraint_id_ = 0;

    bool correct_ = true;
  };

  DrawingBoard* drawing_board_;

  std::unique_ptr<PolygonEdge> body_;
  unsigned int nverticies_ = 0;
};
}  // namespace gk
