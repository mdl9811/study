//
// Copyright (c) 2023 mdl. All rights reserved.
//

#ifndef SRC_BASE_MEMORY_MEMORY_ORDER_H_
#define SRC_BASE_MEMORY_MEMORY_ORDER_H_

#define memmory_acquire(acquire) (acquire.load(std::memory_order_acquire))
#define memmory_release(release, val) \
  (release.store(val, std::memory_order_release))

#define memmory_acq_rel_add(acq_rel, val) \
  (acq_rel.fetch_add(val, std::memory_order_acq_rel))

#define memmory_acq_rel_sub(acq_rel, val) \
  (acq_rel.fetch_sub(val, std::memory_order_acq_rel))

#endif