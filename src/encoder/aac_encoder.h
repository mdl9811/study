//
// Copyright (c) 2023 mdl. All rights reserved.
//

#ifndef SRC_ENCODER_AAC_ENCODER_H_
#define SRC_ENCODER_AAC_ENCODER_H_

#include <memory>
#include "src/api/audio/audio_format.h"
#include "src/api/audio/encode_sink.h"
#include "src/base/io_buffer.h"
#include "src/base/thread/thread_runloop.h"
#include "src/config/config_site.h"

#include <atomic>

#if BUILDFLAG(IS_WIN)
#include "src/base/thread/thread_win.h"
using RunLoop = _LIB_NAMESPACE::base::ThreadWin;
#endif

struct AACENCODER;

namespace _LIB_NAMESPACE::encoder {
class AACEcoder {
 public:
  explicit AACEcoder(api::EncodeAudioSink* sink, uint32_t id);

  AACEcoder(const AACEcoder&) = delete;
  AACEcoder& operator=(const AACEcoder&) = delete;

  virtual ~AACEcoder();

  // aot lc ld 算法模式  lc aot == 2算法快 低复杂度
  // AAC-ELD的范围是最广的，包括
  // 1024，512，480，其次是AAC-LD范围为:512,480,那么对AAC-LC来说就只有1024了。(注意这里的长度是采样点的个数)。
  bool Initialize(api::AudioFormat* format,
                  uint32_t bitrate,
                  uint16_t aot,
                  uint16_t frame_len);
  bool EncodeAudio(const char* data, size_t size);

  void Release();

 private:
  void HandleEncode(scoped_refptr<base::IOBuffer> buffer);

 private:
  uint32_t session_id_;
  struct AACENCODER* aac_handle_ = nullptr;

  api::EncodeAudioSink* sink_;
  std::unique_ptr<RunLoop> run_loop_;
  api::AudioFormat audio_format_;

  uint16_t frame_len_ = 0;
  std::unique_ptr<char[]> output_buffer_;
  size_t buffer_size_ = 0;

  size_t size_ = 0;
  bool init_done_ = false;
};
}  // namespace _LIB_NAMESPACE::encoder

#endif