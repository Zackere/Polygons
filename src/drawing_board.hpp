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
  using SizeType = unsigned int;

  static bool RegisterWindowClass(HINSTANCE hInstance);
  DrawingBoard(SizeType posx,
               SizeType posy,
               SizeType width,
               SizeType height,
               SizeType pixel_size,
               HINSTANCE hInstance,
               std::unique_ptr<Controller> controller);
  virtual ~DrawingBoard();

  bool Show() { return ShowWindow(window_, SW_RESTORE); }
  bool Hide() { return ShowWindow(window_, SW_MINIMIZE); }
  void Display();

  SizeType GetPixelSize() const { return pixel_size_; }
  SizeType GetWidth() const { return drawing_board_width_; }
  SizeType GetHeight() const { return drawing_board_height_; }

  void Clear();
  void SetPixel(SizeType x, SizeType y, COLORREF color);
  void DrawTxt(SizeType posx,
               SizeType posy,
               std::wstring_view text,
               SizeType font_size,
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

  const SizeType pixel_size_;
  const SizeType drawing_board_width_;
  const SizeType drawing_board_height_;

  HDC hdc_mem_;
  HBITMAP off_screen_bitmap_;

  std::unique_ptr<Controller> controller_;

  // Disallow copy and assign
  DrawingBoard& operator=(DrawingBoard&) = delete;
  DrawingBoard(DrawingBoard&) = delete;
};
}  // namespace gk
