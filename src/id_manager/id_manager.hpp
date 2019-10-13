// Copyright Wojciech Replin 2019

#pragma once

namespace gk {
namespace id_manager {
using ID = unsigned int;
ID Get();
void Release(ID id);
}  // namespace id_manager
}  // namespace gk
