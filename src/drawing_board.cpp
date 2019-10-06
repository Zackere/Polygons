// Copyright Wojciech Replin 2019

#include "drawing_board.hpp"

#include <string>
#include <utility>

#include "controllers/controller.hpp"

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
      controller_(std::move(controller)) {
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
  window_hdc_ = GetDC(window_);

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
  RECT rect = {};
  GetClientRect(window_, &rect);
  controller_->Draw(this);
  StretchBlt(window_hdc_, rect.left, rect.top, rect.right - rect.left,
             rect.bottom - rect.top, hdc_mem_, 0, 0, drawing_board_width_,
             drawing_board_height_, SRCCOPY);
}

void DrawingBoard::Clear() {
  RECT rect = {0, 0, drawing_board_width_, drawing_board_height_};
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
                     ANTIALIASED_QUALITY, VARIABLE_PITCH, TEXT("arial"));
  HFONT old_font = reinterpret_cast<HFONT>(SelectObject(hdc_mem_, hFont));
  COLORREF old_color = SetTextColor(hdc_mem_, color);
  DrawTextW(hdc_mem_, text.data(), text.length(), &rect, DT_NOCLIP);
  SetTextColor(hdc_mem_, old_color);
  SelectObject(hdc_mem_, old_font);
  DeleteObject(hFont);
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
            std::make_pair(LOWORD(lParam) / window->GetPixelSize(),
                           HIWORD(lParam) / window->GetPixelSize()));

      return 0;
    case WM_LBUTTONDOWN:
      if (window)
        window->OnMouseLButtonDown(
            std::make_pair(LOWORD(lParam) / window->GetPixelSize(),
                           HIWORD(lParam) / window->GetPixelSize()));
      return 0;
    case WM_LBUTTONUP:
      if (window)
        window->OnMouseLButtonUp(
            std::make_pair(LOWORD(lParam) / window->GetPixelSize(),
                           HIWORD(lParam) / window->GetPixelSize()));

      return 0;
    case WM_MOUSEMOVE:
      if (window)
        window->OnMouseMove(
            std::make_pair(LOWORD(lParam) / window->GetPixelSize(),
                           HIWORD(lParam) / window->GetPixelSize()));

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

void DrawingBoard::OnMouseLButtonDown(CoordinatePair mouse_pos) {
  if (controller_->OnMouseLButtonDown(this, mouse_pos)) {
    Clear();
    Display();
  }
}

void DrawingBoard::OnMouseLButtonUp(CoordinatePair mouse_pos) {
  if (controller_->OnMouseLButtonUp(this, mouse_pos)) {
    Clear();
    Display();
  }
}

void DrawingBoard::OnMouseMove(CoordinatePair mouse_pos) {
  if (controller_->OnMouseMove(this, mouse_pos)) {
    Clear();
    Display();
  }
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

void DrawingBoard::OnMouseLButtonDoubleClick(CoordinatePair mouse_pos) {
  if (controller_->OnMouseLButtonDoubleClick(this, mouse_pos)) {
    Clear();
    Display();
  }
}
}  // namespace gk
