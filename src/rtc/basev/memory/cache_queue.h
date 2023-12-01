//
// Copyright (c) 2023 mdl. All rights reserved.
//

#ifndef SRC_BASE_MEMORY_CACHE_QUEUE_H_
#define SRC_BASE_MEMORY_CACHE_QUEUE_H_

#include <stdio.h>
#include <atomic>
#include <vector>
#include "src/base/io_buffer.h"
#include "src/config/config_site.h"

namespace _LIB_NAMESPACE::base {
class CacheQueue final {
 public:
  explicit CacheQueue(uint16_t size, uint16_t len);
  ~CacheQueue();
  scoped_refptr<base::IOBuffer> AcquireBuffer();
  scoped_refptr<base::IOBuffer> AcquireBuffer(const void* const data, int size);
  void Resize(uint16_t size);
  int size() { return cache_size_; }

 private:
  void Initialize(uint16_t size, uint16_t len);
  void Release();

 private:
  std::vector<scoped_refptr<base::IOBuffer>> cache_queue_;
  std::atomic<int> cache_index_;
  std::atomic<int> cache_size_;
  std::atomic<int> buffer_len_;
};

}  // namespace _LIB_NAMESPACE::base

#endif