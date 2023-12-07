#include "src/encoder/opus_encoder.h"
#include "src/base/log.h"

extern "C" {
#include "third_party/opus/include/opus.h"
}

namespace _LIB_NAMESPACE::encoder {

OpusEncoder::OpusEncoder(call::EncodeAudioSink* sink, uint32_t id)
    : sink_(sink), id_(id) {
  DCHECK(sink);
}

bool OpusEncoder::Initialize(base::AudioFormat* format) {
  if (init_done_)
    return false;
  DCHECK(format);
  DCHECK(format->type == base::AudioFormat::kDecode);

  int err = 0;

  init_done_ = true;
  return true;
}

bool OpusEncoder::EncodeAudio(std::unique_ptr<base::Buffer> buffer) {
  if (!init_done_)
    return false;
  return true;
}

void OpusEncoder::Release() {
  if (!init_done_)
    return;
  init_done_ = false;
  return;
}

}  // namespace _LIB_NAMESPACE::encoder