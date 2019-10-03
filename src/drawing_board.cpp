// Copyright Wojciech Replin 2019

#include "drawing_board.hpp"

#include <functional>
#include <string>
#include <unordered_map>

#include "pixel.hpp"

namespace gk {
namespace {
constexpr wchar_t kDrawingBoardClassName[] = L"gk::DrawingBoard";

std::string GetErrorCodeString(int const error_code) {
  if (error_code == 0)
    return std::string();
  LPSTR message_buffer = nullptr;
  const auto size = FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
          FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, error_code, LANG_SYSTEM_DEFAULT,
      reinterpret_cast<LPSTR>(&message_buffer), 0, NULL);
  const std::string message(message_buffer, size);
  LocalFree(message_buffer);
  return message;
}
}  // namespace

/* static */
bool DrawingBoard::RegisterWindowClass(HINSTANCE hInstance) {
  WNDCLASSW window_class = {};

  window_class.lpszClassName = kDrawingBoardClassName;
  window_class.hInstance = hInstance;
  window_class.lpfnWndProc = DrawingBoard::WndProc;
  window_class.hCursor = LoadCursor(NULL, (LPTSTR)IDC_CROSS);

  return RegisterClassW(&window_class);
}

DrawingBoard::DrawingBoard(SizeType posx,
                           SizeType posy,
                           SizeType width,
                           SizeType height,
                           SizeType pixel_size,
                           HINSTANCE hInstance)
    : pixel_size_(pixel_size),
      drawing_board_width_(width),
      drawing_board_height_(height) {
  if (!(width > 0 && height > 0 && pixel_size > 0)) {
    MessageBox(NULL,
               "One of parameters is incorrect. (gk::DrawingBoard constructor)",
               nullptr, MB_ICONWARNING);
    PostQuitMessage(1);
    return;
  }

  constexpr DWORD window_style =
      WS_OVERLAPPEDWINDOW ^ (WS_THICKFRAME | WS_MAXIMIZEBOX);

  RECT window_rect = {static_cast<LONG>(posx), static_cast<LONG>(posy),
                      static_cast<LONG>(posx + width * pixel_size),
                      static_cast<LONG>(posy + height * pixel_size)};
  AdjustWindowRect(&window_rect, window_style, false);

  window_ = CreateWindowW(
      kDrawingBoardClassName, L"Polygons", window_style, window_rect.left, posy,
      window_rect.right - window_rect.left,
      window_rect.bottom - window_rect.top, NULL, NULL, hInstance, nullptr);
  if (!window_) {
    MessageBox(NULL, GetErrorCodeString(GetLastError()).c_str(), nullptr,
               MB_ICONWARNING);
    PostQuitMessage(GetLastError());
    return;
  }
  SetWindowLongPtr(window_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

  hdc_mem_ = CreateCompatibleDC(GetDC(window_));
  off_screen_bitmap_ = CreateCompatibleBitmap(GetDC(window_), width, height);
  SelectObject(hdc_mem_, off_screen_bitmap_);

  drawing_board_.resize(width);
  for (auto i = 0u; i < width; ++i) {
    drawing_board_[i].reserve(height);
    for (auto j = 0u; j < height; ++j) {
      drawing_board_[i].emplace_back(
          std::complex<unsigned int>{i, j},
          (i + j) % 2 ? RGB(255, 255, 255) : RGB(0, 0, 0));
    }
  }
  UpdateBitmap();
}

DrawingBoard::~DrawingBoard() {
  DeleteObject(off_screen_bitmap_);
  DeleteDC(hdc_mem_);
  DestroyWindow(window_);
}

void DrawingBoard::Display() {
  UpdateBitmap();
  RECT rect = {};
  GetClientRect(window_, &rect);
  StretchBlt(GetDC(window_), rect.left, rect.top, rect.right - rect.left,
             rect.bottom - rect.top, hdc_mem_, 0, 0, drawing_board_width_,
             drawing_board_height_, SRCCOPY);
}

void DrawingBoard::SetPixel(SizeType x, SizeType y, COLORREF color) {
  if (x < drawing_board_width_ && y < drawing_board_height_)
    drawing_board_[x][y].SetColor(color);
}

/* static */
LRESULT DrawingBoard::WndProc(HWND hWnd,
                              UINT message,
                              WPARAM wParam,
                              LPARAM lParam) {
  auto* window =
      reinterpret_cast<DrawingBoard*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
  switch (message) {
    case WM_PAINT: {
      if (window)
        window->Display();
      return 0;
    }
    case WM_ERASEBKGND:
      return 1;
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    default:
      return DefWindowProcW(hWnd, message, wParam, lParam);
  }
}

void DrawingBoard::UpdateBitmap() {
  for (auto i = 0u; i < drawing_board_.size(); ++i) {
    for (auto j = 0u; j < drawing_board_[i].size(); ++j) {
      auto& pixel = drawing_board_[i][j];
      if (!pixel.IsUptoDate()) {
        ::SetPixel(hdc_mem_, pixel.GetPos().real(), pixel.GetPos().imag(),
                   pixel.GetColor());
        pixel.OnDraw();
      }
    }
  }
}
}  // namespace gk
