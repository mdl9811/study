//
// Copyright (c) 2023 mdl. All rights reserved.
//

#ifndef SRC_ENCODER_AAC_ENCODER_H_
#define SRC_ENCODER_AAC_ENCODER_H_

#include <memory>
#include "src/base/audio/audio_format.h"
#include "src/base/io_buffer.h"
#include "src/call/audio/encode_sink.h"
#include "src/config/config_site.h"

#include <atomic>
#if BUILDFLAG(IS_WIN)
struct AACENCODER;
#endif
// 单线程 异步 阻塞 这样写主要是为了可以嵌入到其他项目中
namespace _LIB_NAMESPACE::encoder {
class AACEcoder {
 public:
  // call sink  有可能多路流 区分回调id
  explicit AACEcoder(call::EncodeAudioSink* sink, uint32_t id);

  AACEcoder(const AACEcoder&) = delete;
  AACEcoder& operator=(const AACEcoder&) = delete;

  virtual ~AACEcoder();

  // aot lc ld 算法模式  lc aot == 2算法快 低复杂度
  // AAC-ELD的范围是最广的，包括
  // 1024，512，480，其次是AAC-LD范围为:512,480,那么对AAC-LC来说就只有1024了。(注意这里的长度是采样点的个数)。
  // type 0 是TT_MP4_RAW 2是TT_MP4_ADTS
  bool Initialize(base::AudioFormat* format,
                  uint32_t bitrate,
                  uint16_t aot,
                  uint16_t frame_len);
  bool EncodeAudio(std::unique_ptr<base::Buffer> buffer);

  void Release();

 private:
  void Reset();
  void HandleEncode(std::unique_ptr<base::Buffer> buffer);

 private:
  uint32_t session_id_;
  struct AACENCODER* aac_handle_ = nullptr;
  call::EncodeAudioSink* sink_;

  std::unique_ptr<base::Buffer> output_buffer_;
  bool init_done_ = false;
};
}  // namespace _LIB_NAMESPACE::encoder

#endif