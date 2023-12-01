//
// Copyright (c) 2023 mdl. All rights reserved.
//

#ifndef SRC_API_AUDIO_AUDIO_FORMAT_H_
#define SRC_API_AUDIO_AUDIO_FORMAT_H_
#include <stdint.h>
#include "src/config/config_site.h"

namespace _LIB_NAMESPACE::api {

struct AudioFormat {
  uint8_t bit_depth;     // 位深
  uint8_t channels;      // 声道数
  uint32_t sample_rate;  // 采样率
};

}  // namespace _LIB_NAMESPACE::api

#endif