#include "src/base/memory/weak_ptr.h"
#include "src/base/memory/memory_order.h"

namespace _LIB_NAMESPACE::base {

WeakPtrFlag::WeakPtrFlag() = default;
WeakPtrFlag::~WeakPtrFlag() = default;

bool WeakPtrFlag::Invalid() {
  return memmory_acquire(flag_);
}

void WeakPtrFlag::Set() {
  memmory_release(flag_, true);
}

void WeakPtrFlag::ReleaseFlag() {
  memmory_release(flag_, false);
}

}  // namespace _LIB_NAMESPACE::base