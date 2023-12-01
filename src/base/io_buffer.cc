#include "src/base/io_buffer.h"

namespace _LIB_NAMESPACE::base {

scoped_refptr<IOBuffer> IOBuffer::New(const void* p, size_t n, Type type) {
  IOBuffer* buffer =
      reinterpret_cast<IOBuffer*>(new char[sizeof(IOBuffer) + n]);
  buffer->type_ = type;
  buffer->size_ = n;
  buffer->ref_count_ = 0;
  memcpy(buffer->data(), p, n);
  return scoped_refptr<IOBuffer>(buffer);
}

scoped_refptr<IOBuffer> IOBuffer::New(size_t n, Type type) {
  IOBuffer* buffer =
      reinterpret_cast<IOBuffer*>(new char[sizeof(IOBuffer) + n]);
  buffer->type_ = type;
  buffer->size_ = n;
  buffer->ref_count_ = 0;
  return scoped_refptr<IOBuffer>(buffer);
}

}  // namespace _LIB_NAMESPACE::base
