// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

#include <memory>
#include <optional>
#include <set>
#include <utility>
#include <vector>

#include "../controller/controller.hpp"
#include "../drawing_board/drawing_board.hpp"

namespace gk {
class Polygon;

class PolygonController : public Controller {
 public:
  PolygonController();
  // Overridden from Controller
  ~PolygonController() override;
  bool OnMouseLButtonDown(DrawingBoard* board,
                          DrawingBoard::Point2d mouse_pos) override;
  bool OnMouseLButtonUp(DrawingBoard* board,
                        DrawingBoard::Point2d mouse_pos) override;
  bool OnMouseLButtonDoubleClick(DrawingBoard* board,
                                 DrawingBoard::Point2d mouse_pos) override;
  bool OnMouseMove(DrawingBoard* board,
                   DrawingBoard::Point2d mouse_pos) override;
  bool OnKeyDown(DrawingBoard* board, WPARAM key_code, bool was_down) override;
  bool OnKeyUp(DrawingBoard* board, WPARAM key_code) override;
  void Draw(DrawingBoard* board) override;

 private:
  std::set<std::unique_ptr<Polygon>> polygons_;
  enum class State {
    FREE,
    CREATE_VERTEX,
    CREATE_POLYGON,
    PURE_DESTRUCTION,
    SET_PERPENDICULAR,
    SET_EQUAL_LENGTH,
    TOTAL_STATES,
  } state_ = State::FREE;
  void SetState(State state, DrawingBoard* board);

  std::optional<DrawingBoard::Point2d> last_click_;
  std::vector<DrawingBoard::Point2d> polygon_verticies_;
};
}  // namespace gk
