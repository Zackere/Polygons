// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

#include <set>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "../drawing_board/drawing_board.hpp"
#include "../controller/controller.hpp"

namespace gk {
class Polygon;

class PolygonController : public Controller {
 public:
  PolygonController();
  // Overridden from Controller
  ~PolygonController() override;
  bool OnMouseLButtonDown(DrawingBoard* board,
                          DrawingBoard::CoordinatePair mouse_pos) override;
  bool OnMouseLButtonUp(DrawingBoard* board,
                        DrawingBoard::CoordinatePair mouse_pos) override;
  bool OnMouseLButtonDoubleClick(
      DrawingBoard* board,
      DrawingBoard::CoordinatePair mouse_pos) override;
  bool OnMouseMove(DrawingBoard* board,
                   DrawingBoard::CoordinatePair mouse_pos) override;
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
    TOTAL_STATES
  } state_ = State::FREE;

  std::optional<DrawingBoard::CoordinatePair> last_click_;
  std::vector<DrawingBoard::CoordinatePair> polygon_verticies_;
};
}  // namespace gk
