// Copyright Wojciech Replin 2019

#include <Windows.h>

#include "./controller/polygon_controller.hpp"
#include "./drawing_board/drawing_board.hpp"

namespace {
void RunMessageLoop() {
  MSG message = {};
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
  constexpr gk::DrawingBoard::Size kPosx = 0, kPosy = 0;
  constexpr gk::DrawingBoard::Size kPixelSize = 2;
  constexpr gk::DrawingBoard::Size kWidth = 1600;
  constexpr gk::DrawingBoard::Size kHeight = 800;

  // Do not change these.
  gk::DrawingBoard::RegisterWindowClass(hInstance);
  gk::DrawingBoard window(kPosx, kPosy, kWidth / kPixelSize,
                          kHeight / kPixelSize, kPixelSize, hInstance,
                          std::make_unique<gk::PolygonController>());
  window.Show();
  RunMessageLoop();
  return 0;
}
