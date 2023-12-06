#include "src/decoder/opus_decoder.h"

namespace _LIB_NAMESPACE::decoder {

OpusDecoder::OpusDecoder(call::DecodeAudioSink* sink, uint32_t id)
    : sink_(sink), id_(id) {}
}  // namespace _LIB_NAMESPACE::decoder