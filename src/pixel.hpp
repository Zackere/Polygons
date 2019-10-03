// Copyright Wojciech Replin 2019

#pragma once

#include <Windows.h>

#include <complex>

namespace gk {
class Pixel {
 public:
  Pixel(std::complex<unsigned int> const& position, COLORREF color);
  virtual ~Pixel() = default;

  void SetColor(COLORREF color) {
    is_up_to_date_ = color_ == color;
    color_ = color;
  }
  COLORREF GetColor() const { return color_; }
  unsigned char R() const { return GetRValue(color_); }
  unsigned char G() const { return GetGValue(color_); }
  unsigned char B() const { return GetBValue(color_); }
  std::complex<unsigned int> const& GetPos() const { return position_; }

  bool IsUptoDate() const { return is_up_to_date_; }
  void OnDraw() { is_up_to_date_ = true; }

 private:
  const std::complex<unsigned int> position_;
  COLORREF color_;
  bool is_up_to_date_ = false;
};
}  // namespace gk
