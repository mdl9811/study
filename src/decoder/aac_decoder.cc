#include "src/decoder/aac_decoder.h"

extern "C" {
#include "third_party/aac/libAACdec/include/aacdecoder_lib.h"
}

namespace _LIB_NAMESPACE::decoder {
AACDecoder::AACDecoder(api::DecodeAudioSink* sink, uint32_t id)
    : sink_(sink), session_id_(id) {}
AACDecoder::~AACDecoder() = default;

}  // namespace _LIB_NAMESPACE::decoder
