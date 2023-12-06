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

  // aot lc ld 算法模式  lc aot == 2算法快 低复杂度
  // AAC-ELD的范围是最广的，包括
  // 1024，512，480，其次是AAC-LD范围为:512,480,那么对AAC-LC来说就只有1024了。(注意这里的长度是采样点的个数)。
  // type 0 是TT_MP4_RAW 2是TT_MP4_ADTS
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