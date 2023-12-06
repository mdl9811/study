//
// Copyright (c) 2023 mdl. All rights reserved.
//

#ifndef SRC_CALL_AUDIO_ENCODE_SINK_H_
#define SRC_CALL_AUDIO_ENCODE_SINK_H_
#include <stdint.h>
#include "src/base/buffer.h"
#include "src/config/config_site.h"

namespace _LIB_NAMESPACE::call {

class EncodeAudioSink {
 public:
  virtual ~EncodeAudioSink() {}

  // 接收 encode 后的数据
  virtual void OnEncodeAudio(const study::base::Buffer*, size_t size) = 0;
};

}  // namespace _LIB_NAMESPACE::call

#endif