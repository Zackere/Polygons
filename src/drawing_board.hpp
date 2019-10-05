// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

#include <vector>

namespace gk {
class DrawingBoard {
 public:
  using SizeType = unsigned int;

  static bool RegisterWindowClass(HINSTANCE hInstance);
  DrawingBoard(SizeType posx,
               SizeType posy,
               SizeType width,
               SizeType height,
               SizeType pixel_size,
               HINSTANCE hInstance);
  virtual ~DrawingBoard();

  bool Show() { return ShowWindow(window_, SW_RESTORE); }
  bool Hide() { return ShowWindow(window_, SW_MINIMIZE); }
  void Display();

  SizeType GetPixelSize() const { return pixel_size_; }
  SizeType GetWidth() const { return drawing_board_width_; }
  SizeType GetHeight() const { return drawing_board_height_; }

  void SetPixel(SizeType x, SizeType y, COLORREF color);

 private:
  static LRESULT CALLBACK WndProc(HWND hWnd,
                                  UINT message,
                                  WPARAM wParam,
                                  LPARAM lParam);

  HWND window_;
  HDC window_hdc_;

  const SizeType pixel_size_;
  const SizeType drawing_board_width_;
  const SizeType drawing_board_height_;

  HDC hdc_mem_;
  HBITMAP off_screen_bitmap_;

  // Disallow copy and assign
  DrawingBoard& operator=(DrawingBoard&) = delete;
  DrawingBoard(DrawingBoard&) = delete;
};
}  // namespace gk
