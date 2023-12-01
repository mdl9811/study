//
// Copyright (c) 2023 mdl. All rights reserved.
//

#ifndef SRC_BASE_THREAD_THREAD_RUNLOOP_H_
#define SRC_BASE_THREAD_THREAD_RUNLOOP_H_
#include <stdint.h>
#include <functional>
#include <string>
#include "src/config/config_site.h"

namespace _LIB_NAMESPACE::base {

using Task = std::function<void()>;

class ThreadMessageLoop {
 public:
  explicit ThreadMessageLoop(const std::string& name) : name_(name) {}
  virtual ~ThreadMessageLoop() {}

  virtual void PostTask(Task task) = 0;
  virtual void Start() = 0;
  virtual void Stop() = 0;

 protected:
  std::string name_;
};

}  // namespace _LIB_NAMESPACE::base

#endif