//
// Copyright (c) 2023 mdl. All rights reserved.
//

#ifndef SRC_DECODER_OPUS_DECODER_H_
#define SRC_DECODER_OPUS_DECODER_H_

#include "src/base/audio/audio_format.h"
#include "src/call/audio/decode_sink.h"
#include "src/config/config_site.h"

namespace _LIB_NAMESPACE::decoder {

class OpusDecoder {
 public:
  // call sink  有可能多路流 区分回调id
  explicit OpusDecoder(call::DecodeAudioSink* sink, uint32_t id);

  OpusDecoder(const OpusDecoder&) = delete;
  OpusDecoder& operator=(const OpusDecoder&) = delete;

  virtual ~OpusDecoder() = default;

  bool Initialize(base::AudioFormat* format);
  bool DecodeAudio(std::unique_ptr<base::Buffer> buffer);

  void Release();

 private:
  call::DecodeAudioSink* sink_;
  uint32_t id_;
  bool init_done_ = false;
};
}  // namespace _LIB_NAMESPACE::decoder

#endif