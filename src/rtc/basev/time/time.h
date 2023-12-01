//
// Copyright (c) 2023 mdl. All rights reserved.
//

// TODO: (mdl)
#ifndef SRC_BASE_TIME_TIME_H_
#define SRC_BASE_TIME_TIME_H_
#include <stdint.h>
#include "src/config/config_site.h"

namespace _LIB_NAMESPACE::base {

class Time {
 public:
  Time();
  ~Time();

  static uint64_t Now();
};

}  // namespace _LIB_NAMESPACE::base

#endif