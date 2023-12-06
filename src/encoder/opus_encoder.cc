#include "src/encoder/opus_encoder.h"

namespace _LIB_NAMESPACE::encoder {

OpusEcoder::OpusEcoder(call::EncodeAudioSink* sink, uint32_t id)
    : sink_(sink), id_(id) {}

bool OpusEcoder::Initialize(base::AudioFormat* format) {
  if (init_done_)
    return false;
  init_done_ = true;
  return true;
}
bool OpusEcoder::EncodeAudio(std::unique_ptr<base::Buffer> buffer) {
  if (!init_done_)
    return false;
  return true;
}

void OpusEcoder::Release() {
  if (!init_done_)
    return;
  init_done_ = false;
  return;
}

}  // namespace _LIB_NAMESPACE::encoder