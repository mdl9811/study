//
// Copyright (c) 2023 mdl. All rights reserved.
//

#ifndef SRC_BASE_AUDIO_AUDIO_FORMAT_H_
#define SRC_BASE_AUDIO_AUDIO_FORMAT_H_
#include <stdint.h>
#include "src/config/config_site.h"

namespace _LIB_NAMESPACE::base {

struct AudioFormat {
  enum AudioType : uint8_t {
    kNone = 0,
    kRaw,
    kCapture,
    kEncode,
    kDecode,
  };

  AudioFormat(AudioType tp);
  AudioFormat() = default;
  AudioFormat(const AudioFormat&) = default;
  AudioFormat& operator=(const AudioFormat&) = default;
  ~AudioFormat() = default;

  union {
    struct {
    } raw;
    struct {
      uint8_t bits;          // 位深
      uint8_t channels;      // 声道数
      uint32_t sample_rate;  // 采样率
    } capture;
    struct {
      uint8_t bits;          // 位深
      uint8_t channels;      // 声道数
      uint32_t sample_rate;  // 采样率
    } encode;
    struct {
      uint8_t channels;      // 声道数
      uint32_t sample_rate;  // 采样率
    } decode;
  };  // namespace _LIB_NAMESPACE::base

  AudioType type = AudioType::kNone;

  bool enabled_copy = false;
};

}  // namespace _LIB_NAMESPACE::base

#endif