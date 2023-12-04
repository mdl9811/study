//
// Copyright (c) 2023 mdl. All rights reserved.
//

#ifndef SRC_CALL_VIDEO_DECODE_SINK_H_
#define SRC_CALL_VIDEO_DECODE_SINK_H_
#include <stdint.h>
#include "src/base/buffer.h"
#include "src/config/config_site.h"

namespace _LIB_NAMESPACE::call {

class DecodeVideoSink {
 public:
  virtual ~DecodeVideoSink() {}

  // 接收 encode 后的数据
  virtual void OnDecodeVideo(std::unique_ptr<base::Buffer>) = 0;
};

}  // namespace _LIB_NAMESPACE::call

#endif