// Copyright Wojciech Replin 2019

#include "drawing_board.hpp"

#include <string>
#include <utility>

#include "../controller/controller.hpp"

namespace gk {
namespace {
constexpr wchar_t kDrawingBoardClassName[] = L"gk::DrawingBoard";

std::wstring GetErrorCodeString(const int error_code) {
  if (error_code == 0)
    return std::wstring();
  LPWSTR message_buffer = nullptr;
  const auto size = FormatMessageW(
      FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
          FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL, error_code, LANG_SYSTEM_DEFAULT,
      reinterpret_cast<LPWSTR>(&message_buffer), 0, NULL);
  const std::wstring message(message_buffer, size);
  LocalFree(message_buffer);
  return message;
}

POINT GetCursorPosInWindow(HWND hWnd) {
  POINT p;
  GetCursorPos(&p);
  ScreenToClient(hWnd, &p);
  return p;
}
}  // namespace

/* static */
bool DrawingBoard::RegisterWindowClass(HINSTANCE hInstance) {
  WNDCLASSW window_class = {};

  window_class.lpszClassName = kDrawingBoardClassName;
  window_class.hInstance = hInstance;
  window_class.lpfnWndProc = DrawingBoard::WndProc;
  window_class.hCursor = LoadCursor(NULL, (LPTSTR)IDC_CROSS);
  window_class.style = CS_DBLCLKS;

  return RegisterClassW(&window_class);
}

DrawingBoard::DrawingBoard(Size posx,
                           Size posy,
                           Size width,
                           Size height,
                           Size pixel_size,
                           HINSTANCE hInstance,
                           std::unique_ptr<Controller> controller)
    : window_(NULL),
      window_hdc_(NULL),
      pixel_size_(pixel_size),
      drawing_board_width_(width),
      drawing_board_height_(height),
      hdc_mem_(NULL),
      off_screen_bitmap_(NULL),
      last_mouse_pos_({0, 0}),
      controller_(std::move(controller)) {
  if (!(width > 0 && height > 0 && pixel_size > 0)) {
    ShowError(L"One of parameters is incorrect. (gk::DrawingBoard constructor)",
              true);
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
    ShowError(GetErrorCodeString(GetLastError()), true);
    return;
  }
  SetWindowLongPtr(window_, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
  window_hdc_ = GetDC(window_);
  const auto mouse_pos = GetCursorPosInWindow(window_);
  last_mouse_pos_ = Point2d(mouse_pos.x, mouse_pos.y);

  hdc_mem_ = CreateCompatibleDC(window_hdc_);
  off_screen_bitmap_ = CreateCompatibleBitmap(window_hdc_, width, height);
  SelectObject(hdc_mem_, off_screen_bitmap_);
  SetBkMode(hdc_mem_, TRANSPARENT);
}

DrawingBoard::~DrawingBoard() {
  ReleaseDC(window_, window_hdc_);
  DeleteObject(off_screen_bitmap_);
  DeleteDC(hdc_mem_);
  DestroyWindow(window_);
}

void DrawingBoard::Display() {
  const RECT rect = {0, 0, drawing_board_width_ * pixel_size_,
                     drawing_board_height_ * pixel_size_};
  controller_->Draw(this);
  StretchBlt(window_hdc_, rect.left, rect.top, rect.right - rect.left,
             rect.bottom - rect.top, hdc_mem_, 0, 0, drawing_board_width_,
             drawing_board_height_, SRCCOPY);
}

void DrawingBoard::Clear() {
  const RECT rect = {0, 0, drawing_board_width_, drawing_board_height_};
  FillRect(hdc_mem_, &rect,
           reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
}

void DrawingBoard::SetPixel(Coordinate x, Coordinate y, COLORREF color) {
  ::SetPixel(hdc_mem_, x, y, color);
}

void DrawingBoard::DrawTxt(Coordinate posx,
                           Coordinate posy,
                           std::wstring_view text,
                           Size font_size,
                           COLORREF color) {
  RECT rect{posx, posy, drawing_board_width_, drawing_board_height_};
  HFONT hFont;
  hFont = CreateFont(font_size, 0, 0, 0, FW_THIN, FALSE, FALSE, FALSE,
                     DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                     NONANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("arial"));
  HFONT old_font = reinterpret_cast<HFONT>(SelectObject(hdc_mem_, hFont));
  COLORREF old_color = SetTextColor(hdc_mem_, color);
  DrawTextW(hdc_mem_, text.data(), text.length(), &rect, DT_NOCLIP);
  SetTextColor(hdc_mem_, old_color);
  SelectObject(hdc_mem_, old_font);
  DeleteObject(hFont);
}

void DrawingBoard::ShowError(std::wstring_view error_message, bool fatal) {
  MessageBoxW(window_, error_message.data(), nullptr,
              (fatal ? MB_ICONERROR : MB_ICONWARNING) | MB_SETFOREGROUND);
  if (fatal)
    PostQuitMessage(1);
}

void DrawingBoard::SetTitle(std::wstring_view new_title) {
  SetWindowTextW(window_, new_title.data());
}

/* static */
LRESULT DrawingBoard::WndProc(HWND hWnd,
                              UINT message,
                              WPARAM wParam,
                              LPARAM lParam) {
  auto* window =
      reinterpret_cast<DrawingBoard*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
  switch (message) {
    case WM_PAINT:
      if (window)
        window->Display();
      return 0;
    case WM_KEYDOWN:
      if (window)
        window->OnKeyDown(wParam, (lParam >> 30) & 1);
      return 0;
    case WM_KEYUP:
      if (window)
        window->OnKeyUp(wParam);
      return 0;
    case WM_LBUTTONDBLCLK:
      if (window)
        window->OnMouseLButtonDoubleClick(
            Point2d(LOWORD(lParam), HIWORD(lParam)) / window->GetPixelSize());
      return 0;
    case WM_LBUTTONDOWN:
      if (window)
        window->OnMouseLButtonDown(Point2d(LOWORD(lParam), HIWORD(lParam)) /
                                   window->GetPixelSize());
      return 0;
    case WM_LBUTTONUP:
      if (window)
        window->OnMouseLButtonUp(Point2d(LOWORD(lParam), HIWORD(lParam)) /
                                 window->GetPixelSize());
      return 0;
    case WM_MOUSEMOVE:
      if (window) {
        Point2d pos =
            Point2d(LOWORD(lParam), HIWORD(lParam)) / window->GetPixelSize();
        window->OnMouseMove(pos);
      }
      return 0;
    case WM_ERASEBKGND:
      return 1;
    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;
    default:
      return DefWindowProcW(hWnd, message, wParam, lParam);
  }
}

void DrawingBoard::OnMouseLButtonDown(Point2d const& mouse_pos) {
  if (controller_->OnMouseLButtonDown(this, mouse_pos)) {
    Clear();
    Display();
  }
}

void DrawingBoard::OnMouseLButtonUp(Point2d const& mouse_pos) {
  if (controller_->OnMouseLButtonUp(this, mouse_pos)) {
    Clear();
    Display();
  }
}

void DrawingBoard::OnMouseMove(Point2d const& mouse_pos) {
  if (controller_->OnMouseMove(this, mouse_pos)) {
    Clear();
    Display();
  }
  last_mouse_pos_ = mouse_pos;
}

void DrawingBoard::OnKeyDown(WPARAM key_code, bool was_down) {
  if (controller_->OnKeyDown(this, key_code, was_down)) {
    Clear();
    Display();
  }
}

void DrawingBoard::OnKeyUp(WPARAM key_code) {
  if (controller_->OnKeyUp(this, key_code)) {
    Clear();
    Display();
  }
}

void DrawingBoard::OnMouseLButtonDoubleClick(Point2d const& mouse_pos) {
  if (controller_->OnMouseLButtonDoubleClick(this, mouse_pos)) {
    Clear();
    Display();
  }
}
}  // namespace gk
