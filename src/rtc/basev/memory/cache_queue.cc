#include "src/base/memory/cache_queue.h"
#include "src/base/memory/memory.h"
#include "src/base/memory/memory_order.h"
namespace _LIB_NAMESPACE::base {

CacheQueue::CacheQueue(uint16_t size, uint16_t len) {
  Initialize(size, len);
}
CacheQueue::~CacheQueue() {
  Release();
}

void CacheQueue::Initialize(uint16_t size, uint16_t len) {
  cache_queue_.resize(size);

  int cache_size = memmory_acquire(cache_size_);
  memmory_release(cache_size_, size);
  memmory_release(buffer_len_, len);

  if (cache_size >= size)
    return;

  for (int i = cache_size; i < size; i++)
    cache_queue_[i] = base::IOBuffer::New(len);
}

void CacheQueue::Release() {
  cache_queue_.clear();
}

scoped_refptr<base::IOBuffer> CacheQueue::AcquireBuffer(const void* const data,
                                                        int size) {
  scoped_refptr<base::IOBuffer> o = AcquireBuffer();
  memcpy(o->data(), reinterpret_cast<const uint8_t*>(data), size);
  return o;
}

scoped_refptr<base::IOBuffer> CacheQueue::AcquireBuffer() {
  // ѭ������ �����ǰindex����size ȡ��õ���ǰ����
  memmory_release(cache_index_,
                  memmory_acquire(cache_index_) % memmory_acquire(cache_size_));

  scoped_refptr<base::IOBuffer> buffer =
      cache_queue_[memmory_acq_rel_add(cache_index_, 1)];

  bzero(buffer->data(), buffer->size());
  return buffer;
}

void CacheQueue::Resize(uint16_t size) {
  Initialize(size, memmory_acquire(buffer_len_));
}

}  // namespace _LIB_NAMESPACE::base