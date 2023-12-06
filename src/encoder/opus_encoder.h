//
// Copyright (c) 2023 mdl. All rights reserved.
//

#ifndef SRC_ENCODER_OPUS_ENCODER_H_
#define SRC_ENCODER_OPUS_ENCODER_H_

#include "src/base/audio/audio_format.h"
#include "src/call/audio/encode_sink.h"
#include "src/config/config_site.h"

namespace _LIB_NAMESPACE::encoder {

class OpusEcoder {
 public:
  // call sink  有可能多路流 区分回调id
  explicit OpusEcoder(call::EncodeAudioSink* sink, uint32_t id);

  OpusEcoder(const OpusEcoder&) = delete;
  OpusEcoder& operator=(const OpusEcoder&) = delete;

  virtual ~OpusEcoder() = default;

  bool Initialize(base::AudioFormat* format);
  bool EncodeAudio(std::unique_ptr<base::Buffer> buffer);

  void Release();

 private:
  uint32_t id_;

  call::EncodeAudioSink* sink_;

  bool init_done_ = false;
};
}  // namespace _LIB_NAMESPACE::encoder

#endif