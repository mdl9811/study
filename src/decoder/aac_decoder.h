//
// Copyright (c) 2023 mdl. All rights reserved.
//

#ifndef SRC_DECODER_AAC_DECODER_H_
#define SRC_DECODER_AAC_DECODER_H_
#include "src/base/audio/audio_format.h"
#include "src/call/audio/decode_sink.h"
#include "src/config/config_site.h"

struct AAC_DECODER_INSTANCE;
namespace _LIB_NAMESPACE::decoder {
class AACDecoder {
 public:
  // call sink  有可能多路流 区分回调id
  explicit AACDecoder(call::DecodeAudioSink* sink, uint32_t id);

  AACDecoder(const AACDecoder&) = delete;
  AACDecoder& operator=(const AACDecoder&) = delete;

  virtual ~AACDecoder();

  // aot lc ld 算法模式  lc aot == 2算法快 低复杂度
  // type 0 是TT_MP4_RAW 2是TT_MP4_ADTS
  bool Initialize(base::AudioFormat* format, uint16_t aot, uint16_t type);
  bool DecodeAudio(std::unique_ptr<base::Buffer> buffer);

  void Release();

 private:
  void Reset();
  void GetRawConfig(std::unique_ptr<uint8_t[]>* conf,
                    uint32_t* size,
                    base::AudioFormat* format,
                    uint8_t aot,
                    uint8_t type);

 private:
  uint32_t session_id_;

  struct AAC_DECODER_INSTANCE* decoder_handle_ = nullptr;

  std::unique_ptr<base::Buffer> output_buffer_;
  call::DecodeAudioSink* sink_;
  base::AudioFormat audio_format_;
  bool init_done_ = false;
};
}  // namespace _LIB_NAMESPACE::decoder

#endif