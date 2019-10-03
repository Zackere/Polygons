// Copyright Wojciech Replin 2019

#include "pixel.hpp"

namespace gk {
Pixel::Pixel(std::complex<unsigned int> const& position,
             COLORREF color)
    : position_(position), color_(color) {}
}  // namespace gk
