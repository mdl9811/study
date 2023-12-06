#include "src/decoder/opus_decoder.h"

namespace _LIB_NAMESPACE::decoder {

OpusDecoder::OpusDecoder(call::DecodeAudioSink* sink, uint32_t id)
    : sink_(sink), id_(id) {}

bool OpusDecoder::Initialize(base::AudioFormat* format) {
  if (init_done_)
    return false;

  init_done_ = true;
  return true;
}
bool OpusDecoder::DecodeAudio(std::unique_ptr<base::Buffer> buffer) {
  if (!init_done_)
    return false;
  return true;
}

void OpusDecoder::Release() {
  if (!init_done_)
    return;
  init_done_ = false;
}

}  // namespace _LIB_NAMESPACE::decoder