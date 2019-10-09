// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

#include <list>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "../drawable_objects/drawable_object.hpp"
#include "../drawing_board.hpp"
#include "controller.hpp"

namespace gk {
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
  void AddObject(std::unique_ptr<DrawableObject> object) override;

 private:
  std::list<std::unique_ptr<DrawableObject>> objects_;
  enum class State {
    FREE,
    CREATE_POINT,
    CREATE_VERTEX,
    CREATE_LINE,
    CREATE_POLYGON,
    PURE_DESTRUCTION,
    TOTAL_STATES
  } state_ = State::FREE;

  std::optional<DrawingBoard::CoordinatePair> last_click_;
  std::vector<DrawingBoard::CoordinatePair> polygon_verticies_;
};
}  // namespace gk
