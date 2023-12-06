#include "src/base/buffer.h"

namespace _LIB_NAMESPACE::base {

std::unique_ptr<Buffer> Buffer::New(const void* p, size_t n, Type type) {
  Buffer* buffer = reinterpret_cast<Buffer*>(new char[sizeof(Buffer) + n]);
  buffer->type_ = type;
  buffer->size_ = n;
  buffer->id_ = -1;
  memcpy(buffer->data(), p, n);
  return std::unique_ptr<Buffer>(buffer);
}

std::unique_ptr<Buffer> Buffer::New(size_t n, Type type) {
  Buffer* buffer = reinterpret_cast<Buffer*>(new char[sizeof(Buffer) + n]);
  buffer->type_ = type;
  buffer->size_ = n;
  buffer->id_ = -1;
  return std::unique_ptr<Buffer>(buffer);
}

}  // namespace _LIB_NAMESPACE::base