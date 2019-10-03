// Copyright Wojciech Replin 2019

#include <Windows.h>

#include "drawing_board.hpp"

namespace {
void RunMessageLoop() {
  MSG message;
  while (GetMessage(&message, NULL, 0, 0)) {
    TranslateMessage(&message);
    DispatchMessage(&message);
  }
}
}  // namespace

int WINAPI wWinMain(HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    PWSTR pCmdLine,
                    int nCmdShow) {
  // Please, change these.
  constexpr gk::DrawingBoard::SizeType kPosx = 0, kPosy = 0;
  constexpr gk::DrawingBoard::SizeType kPixelSize = 4;
  constexpr gk::DrawingBoard::SizeType kWidth = 1600;
  constexpr gk::DrawingBoard::SizeType kHeight = 1000;

  // Do not change these.
  gk::DrawingBoard::RegisterWindowClass(hInstance);
  gk::DrawingBoard window(kPosx, kPosy, kWidth / kPixelSize,
                          kHeight / kPixelSize, kPixelSize, hInstance);
  window.Show();
  RunMessageLoop();
  return 0;
}
