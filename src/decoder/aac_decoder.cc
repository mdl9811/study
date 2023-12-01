#include "src/decoder/aac_decoder.h"

extern "C" {
#include "third_party/aac/libAACdec/include/aacdecoder_lib.h"
}

namespace _LIB_NAMESPACE::decoder {
AACDecoder::AACDecoder(api::DecodeAudioSink* sink, uint32_t id)
    : sink_(sink), session_id_(id) {}
AACDecoder::~AACDecoder() = default;

bool AACDecoder::Initialize(api::AudioFormat* format,
                            uint32_t bitrate,
                            uint16_t aot,
                            uint16_t frame_len) {
  return true;
}

}  // namespace _LIB_NAMESPACE::decoder
