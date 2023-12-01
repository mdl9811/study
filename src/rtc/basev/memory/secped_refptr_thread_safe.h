//
// Copyright (c) 2023 mdl. All rights reserved.
//

#ifndef SRC_BASE_MEMORY_SCOPED_REFPTR_SAFE_H_
#define SRC_BASE_MEMORY_SCOPED_REFPTR_SAFE_H_
#include <atomic>
#include "src/config/config_site.h"

namespace _LIB_NAMESPACE::base {
struct SecpedRefThreadSafe {
  void AddRef() { ref_count_.fetch_add(1, std::memory_order_relaxed); }

  void Release() {
    if (ref_count_.fetch_sub(1, std::memory_order_acq_rel) == 1) {
      delete[] reinterpret_cast<const char*>(this);
    }
  }

 protected:
  std::atomic_int ref_count_ = 0;
};
}  // namespace _LIB_NAMESPACE::base

#endif