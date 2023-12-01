//
// Copyright (c) 2023 mdl. All rights reserved.
//

#ifndef SRC_BASE_IO_BUFFER_H_
#define SRC_BASE_IO_BUFFER_H_
#include <stdint.h>
#include <atomic>
#include <string>
#include "src/base/memory/scoped_refptr.h"
#include "src/config/config_site.h"

namespace _LIB_NAMESPACE::base {
class IOBuffer {
 public:
  enum Type : uint8_t {
    DATA = 0,
    AUDIO_CAPTURE,
    AUDIO_ENCODED,
    AUDIO_DECODED,
    VIDOE_CAPTURE,
    VIDEO_ENCODED,
    VIDEO_DECODED,
  };
  
  static scoped_refptr<IOBuffer> New(const void* p, size_t n, Type type = DATA);
  static scoped_refptr<IOBuffer> New(size_t n, Type type = DATA);

  static scoped_refptr<IOBuffer> New(const std::string& str, Type type = DATA) {
    return New(str.data(), str.size(), type);
  }

  void AddRef() { ref_count_.fetch_add(1, std::memory_order_relaxed); }
  void Release() {
    if (ref_count_.fetch_sub(1, std::memory_order_acq_rel) == 1)
      delete[] reinterpret_cast<const char*>(this);
  }

  template <typename T = char>
  T* data() {
    return reinterpret_cast<T*>(this + 1);
  }

  uint32_t size() const { return size_; }
  Type type() const { return type_; }

  void timestamp(uint64_t ts) { *reinterpret_cast<uint64_t*>(ts_) = ts; }
  uint64_t timestamp() const { return *reinterpret_cast<const uint64_t*>(ts_); }

 private:
  IOBuffer() = delete;
  IOBuffer(const IOBuffer&) = delete;
  IOBuffer& operator=(const IOBuffer&) = delete;

 private:
  Type type_;
  std::atomic_int ref_count_;
  uint32_t ts_[2];
  uint32_t size_;
};

}  // namespace _LIB_NAMESPACE::base
#endif