//
// Copyright (c) 2023 mdl. All rights reserved.
//

#ifndef SRC_ENCODER_OPUS_ENCODER_H_
#define SRC_ENCODER_OPUS_ENCODER_H_

#include "src/call/audio/encode_sink.h"
#include "src/config/config_site.h"

namespace _LIB_NAMESPACE::encoder {

class OpusEncoder {
 public:
  // call sink  有可能多路流 区分回调id
  explicit OpusEncoder(call::EncodeAudioSink* sink, uint32_t id);

  OpusEncoder(const OpusEncoder&) = delete;
  OpusEncoder& operator=(const OpusEncoder&) = delete;

  virtual ~OpusEncoder() = default;

  // duration 采样时间 具体看opus.h OPUS_FRAMESIZE_10_MS
  bool Initialize(uint32_t bitrate,
                  uint32_t sample_rate,
                  uint8_t channels,
                  uint8_t depth,
                  uint32_t duration);
  bool EncodeAudio(std::unique_ptr<base::Buffer> buffer);

  void Release();

 private:
  void Reset();

 private:
  uint32_t id_;

  call::EncodeAudioSink* sink_;

  void* encoder_handle_ = nullptr;
  bool init_done_ = false;
};
}  // namespace _LIB_NAMESPACE::encoder

#endif