// Copyright Wojciech Replin 2019

#include <Windows.h>

#include <string>
#include <vector>

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

std::vector<std::wstring> SplitString(std::wstring const& s,
                                      wchar_t delimiter,
                                      unsigned int max_tokens) {
  std::vector<std::wstring> tokens;
  tokens.reserve(max_tokens);
  std::wstring token;
  std::wistringstream tokenStream(s);
  while (std::getline(tokenStream, token, delimiter)) {
    tokens.emplace_back(token);
    if (--max_tokens == 0)
      return tokens;
  }
  return tokens;
}
}  // namespace

int WINAPI wWinMain(HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    PWSTR pCmdLine,
                    int nCmdShow) {
  gk::DrawingBoard::Size Posx = 0, Posy = 0;
  gk::DrawingBoard::Size PixelSize = 2;
  gk::DrawingBoard::Size Width = 800;
  gk::DrawingBoard::Size Height = 400;

  const auto args = SplitString(pCmdLine, ' ', 5);
  switch (args.size()) {
    default:
    case 5:
      Posy = std::stoi(args[4], nullptr);
    case 4:
      Posx = std::stoi(args[3], nullptr);
    case 3:
      Height = std::stoi(args[2], nullptr);
    case 2:
      Width = std::stoi(args[1], nullptr);
    case 1:
      PixelSize = std::stoi(args[0], nullptr);
      if (PixelSize <= 0)
        PixelSize = 1;
    case 0:
      break;
  }
  gk::DrawingBoard::RegisterWindowClass(hInstance);
  gk::DrawingBoard window(Posx, Posy, Width / PixelSize, Height / PixelSize,
                          PixelSize, hInstance,
                          std::make_unique<gk::PolygonController>());
  window.Show();
  RunMessageLoop();
  return 0;
}
