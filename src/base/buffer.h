//
// Copyright (c) 2023 mdl. All rights reserved.
//

#ifndef SRC_BASE_BUFFER_H_
#define SRC_BASE_BUFFER_H_
#include <stdint.h>
#include <memory>
#include <string>
#include "src/config/config_site.h"

namespace _LIB_NAMESPACE::base {
class Buffer {
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

  static std::unique_ptr<Buffer> New(const void* p, size_t n, Type type = DATA);
  static std::unique_ptr<Buffer> New(const std::string& str, Type type = DATA) {
    return New(str.data(), str.size(), type);
  }
  static std::unique_ptr<Buffer> New(size_t size, Type type = DATA);
  static std::unique_ptr<Buffer> New(Buffer* buf, size_t n, Type type = DATA) {
    return New(buf->data(), n, type);
  }

  template <typename T = char>
  T* data() {
    return reinterpret_cast<T*>(this + 1);
  }

  uint32_t size() const { return size_; }
  Type type() const { return type_; }

  uint32_t session() const { return id_; }
  void session(uint32_t id) { id_ = id; }

  void timestamp(uint64_t ts) { *reinterpret_cast<uint64_t*>(ts_) = ts; }
  uint64_t timestamp() const { return *reinterpret_cast<const uint64_t*>(ts_); }

 private:
  Buffer() = delete;
  Buffer(const Buffer&) = delete;
  Buffer& operator=(const Buffer&) = delete;

 private:
  Type type_;
  uint32_t id_;
  uint32_t ts_[2];
  uint32_t size_;
};
}  // namespace _LIB_NAMESPACE::base
#endif