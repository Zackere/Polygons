// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

#include <memory>
#include <string_view>

#include "controllers/controller.hpp"

namespace gk {
class DrawingBoard {
 public:
  class DrawableObject {
   public:
    virtual void Display(DrawingBoard* board) = 0;
    virtual bool OnMouseLButtonDown(DrawingBoard* board, POINT mouse_pos) = 0;
    virtual bool OnMouseLButtonUp(DrawingBoard* board, POINT mouse_pos) = 0;
    virtual bool OnMouseMove(DrawingBoard* board, POINT mouse_pos) = 0;
  };
  using Size = unsigned int;
  using Coordinate = int;

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

 private:
  static LRESULT CALLBACK WndProc(HWND hWnd,
                                  UINT message,
                                  WPARAM wParam,
                                  LPARAM lParam);
  void OnMouseLButtonDown(POINT mouse_pos);
  void OnMouseLButtonUp(POINT mouse_pos);
  void OnMouseLButtonDoubleClick(POINT mouse_pos);
  void OnMouseMove(POINT mouse_pos);
  void OnKeyDown(WPARAM key_code, bool was_down);
  void OnKeyUp(WPARAM key_code);

  HWND window_;
  HDC window_hdc_;

  const Size pixel_size_;
  const Size drawing_board_width_;
  const Size drawing_board_height_;

  HDC hdc_mem_;
  HBITMAP off_screen_bitmap_;

  std::unique_ptr<Controller> controller_;

  // Disallow copy and assign
  DrawingBoard& operator=(DrawingBoard&) = delete;
  DrawingBoard(DrawingBoard&) = delete;
};
}  // namespace gk
