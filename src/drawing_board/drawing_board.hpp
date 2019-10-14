// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

#undef min
#undef max

#include <algorithm>
#include <complex>
#include <memory>
#include <string_view>
#include <utility>

namespace gk {
class Controller;

class DrawingBoard {
 public:
  using Size = int;
  using Coordinate = double;
  struct Point2d {
    static constexpr double kVerySmallValue = 0.001;
    Point2d(Coordinate x, Coordinate y) : x(x), y(y) {}
    bool operator==(Point2d const& p) const {
      return RelativeDifference(x, p.x) < kVerySmallValue &&
             RelativeDifference(y, p.y) < kVerySmallValue;
    }
    Point2d operator-(Point2d const& p) const { return {x - p.x, y - p.y}; }
    Point2d operator+(Point2d const& p) const { return {x + p.x, y + p.y}; }
    Point2d operator/(Coordinate c) const { return {x / c, y / c}; }
    Point2d operator*(Coordinate c) const { return {x * c, y * c}; }
    Point2d operator*(Point2d const& p) const {
      return {x * p.x - y * p.y, x * p.y + y * p.x};
    }
    Point2d operator/(Point2d const& p) const {
      return Point2d(x * p.x + y * p.y, y * p.x - x * p.y) /
             (p.x * p.x + p.y * p.y);
    }
    Coordinate x;
    Coordinate y;

   private:
    static double RelativeDifference(double a, double b) {
      const auto d = std::max(std::abs(a), std::abs(b));
      return d == 0.0 ? 0.0 : std::abs(a - b) / d;
    }
  };

  static bool RegisterWindowClass(HINSTANCE hInstance);
  DrawingBoard(Size posx,
               Size posy,
               Size width,
               Size height,
               Size pixel_size,
               HINSTANCE hInstance,
               std::unique_ptr<Controller> controller);
  virtual ~DrawingBoard();

  bool Show() { return ShowWindow(window_, SW_RESTORE); }
  bool Hide() { return ShowWindow(window_, SW_MINIMIZE); }
  void Display();

  Size GetPixelSize() const { return pixel_size_; }
  Size GetWidth() const { return drawing_board_width_; }
  Size GetHeight() const { return drawing_board_height_; }

  void Clear();
  void SetPixel(Coordinate x, Coordinate y, COLORREF color);
  void DrawTxt(Coordinate posx,
               Coordinate posy,
               std::wstring_view text,
               Size font_size,
               COLORREF color);
  void ShowError(std::wstring_view error_message, bool fatal);
  void SetTitle(std::wstring_view new_title);
  Point2d const& GetPreviousMousePos() const { return last_mouse_pos_; }
  bool GetKeyState(int key_id) const {
    return GetAsyncKeyState(key_id) & 1 << (sizeof(SHORT) * 8 - 1);
  }

 private:
  static LRESULT CALLBACK WndProc(HWND hWnd,
                                  UINT message,
                                  WPARAM wParam,
                                  LPARAM lParam);
  void OnMouseLButtonDown(Point2d const& mouse_pos);
  void OnMouseLButtonUp(Point2d const& mouse_pos);
  void OnMouseLButtonDoubleClick(Point2d const& mouse_pos);
  void OnMouseMove(Point2d const& mouse_pos);
  void OnKeyDown(WPARAM key_code, bool was_down);
  void OnKeyUp(WPARAM key_code);

  HWND window_;
  HDC window_hdc_;

  const Size pixel_size_;
  const Size drawing_board_width_;
  const Size drawing_board_height_;

  HDC hdc_mem_;
  HBITMAP off_screen_bitmap_;

  Point2d last_mouse_pos_;

  std::unique_ptr<Controller> controller_;

  // Disallow copy and assign
  DrawingBoard& operator=(DrawingBoard&) = delete;
  DrawingBoard(DrawingBoard&) = delete;
};
}  // namespace gk
