//
// Copyright (c) 2023 mdl. All rights reserved.
//

#ifndef SRC_CALL_AUDIO_DECODE_SINK_H_
#define SRC_CALL_AUDIO_DECODE_SINK_H_
#include <stdint.h>
#include "src/base/buffer.h"
#include "src/config/config_site.h"

namespace _LIB_NAMESPACE::call {

class DecodeAudioSink {
 public:
  virtual ~DecodeAudioSink() {}

  // 接收 encode 后的数据
  virtual void OnDecodeAudio(const study::base::Buffer*, size_t size) = 0;
};

}  // namespace _LIB_NAMESPACE::call

#endif