//
// Copyright (c) 2023 mdl. All rights reserved.
//

#ifndef SRC_DECODER_AAC_DECODER_H_
#define SRC_DECODER_AAC_DECODER_H_
#include "src/api/audio/decode_sink.h"
#include "src/config/config_site.h"

namespace _LIB_NAMESPACE::decoder {
class AACDecoder {
 public:
  // call sink  有可能多路流 区分回调id
  explicit AACDecoder(api::DecodeAudioSink* sink, uint32_t id);

  AACDecoder(const AACDecoder&) = delete;
  AACDecoder& operator=(const AACDecoder&) = delete;

  virtual ~AACDecoder();

  bool Initialize(api::AudioFormat* format,
                  uint32_t bitrate,
                  uint16_t aot,
                  uint16_t frame_len);
  bool DecodeAudio(std::unique_ptr<base::Buffer> buffer);

  void Release();

 private:
  void Reset();
  void HandleDecode(std::unique_ptr<base::Buffer> buffer);

 private:
  uint32_t session_id_;

  api::DecodeAudioSink* sink_;
  api::AudioFormat audio_format_;
  bool init_done_ = false;
};
}  // namespace _LIB_NAMESPACE::decoder

#endif