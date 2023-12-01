//
// Copyright (c) 2023 mdl. All rights reserved.
//

#ifndef SRC_API_AUDIO_DECODE_SINK_H_
#define SRC_API_AUDIO_DECODE_SINK_H_
#include <stdint.h>
#include "src/base/buffer.h"
#include "src/config/config_site.h"

namespace _LIB_NAMESPACE::api {

class DecodeAudioSink {
 public:
  virtual ~DecodeAudioSink() {}

  // 接收 encode 后的数据
  virtual void OnDecodeAudio(std::unique_ptr<base::Buffer>) = 0;
};

}  // namespace _LIB_NAMESPACE::api

#endif