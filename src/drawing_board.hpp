// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

#include <vector>

namespace gk {
class Pixel;

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
  void SetPixel(SizeType x, SizeType y, COLORREF color);

 private:
  static LRESULT CALLBACK WndProc(HWND hWnd,
                                  UINT message,
                                  WPARAM wParam,
                                  LPARAM lParam);
  void UpdateBitmap();

  HWND window_;
  std::vector<std::vector<Pixel>> drawing_board_;

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
