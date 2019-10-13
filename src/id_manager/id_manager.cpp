// Copyright Wojciech Replin 2019

#include "id_manager.hpp"

#include <algorithm>
#include <set>

namespace gk {
namespace id_manager {
namespace {
std::set<ID> used_ids_;
}  // namespace
ID Get() {
  ID ret = 0;
  for (auto id : used_ids_)
    if (id == ret) {
      ++ret;
    } else {
      used_ids_.insert(ret);
      return ret;
    }
  used_ids_.insert(ret);
  return ret;
}

void Release(ID id) {
  auto it = std::find(used_ids_.begin(), used_ids_.end(), id);
  if (it != used_ids_.end())
    used_ids_.erase(it);
}
}  // namespace id_manager
}  // namespace gk
