//
// Copyright (c) 2023 mdl. All rights reserved.
//

#ifndef SRC_API_AUDIO_ENCODE_SINK_H_
#define SRC_API_AUDIO_ENCODE_SINK_H_
#include <stdint.h>
#include "src/base/buffer.h"
#include "src/config/config_site.h"

namespace _LIB_NAMESPACE::api {

class EncodeAudioSink {
 public:
  virtual ~EncodeAudioSink() {}

  // 接收 encode 后的数据
  virtual void OnEncodeAudio(std::unique_ptr<base::Buffer>) = 0;
};

}  // namespace _LIB_NAMESPACE::api

#endif