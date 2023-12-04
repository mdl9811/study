#include "src/decoder/aac_decoder.h"

extern "C" {
#include "third_party/aac/libAACdec/include/aacdecoder_lib.h"
}

namespace _LIB_NAMESPACE::decoder {
AACDecoder::AACDecoder(call::DecodeAudioSink* sink, uint32_t id)
    : sink_(sink), session_id_(id) {}
AACDecoder::~AACDecoder() = default;

bool AACDecoder::Initialize(base::AudioFormat* format,
                            uint16_t aot,
                            uint16_t mode) {
  auto handle = aacDecoder_Open(static_cast<TRANSPORT_TYPE>(mode), 1);

  switch (mode) {
    case TRANSPORT_TYPE::TT_MP4_RAW: {
      return false;
    } break;
  }

  decoder_handle_ = handle;
  return true;
}

}  // namespace _LIB_NAMESPACE::decoder
