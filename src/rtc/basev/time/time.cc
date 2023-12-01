#include "src/base/time/time.h"
#include <chrono>

namespace _LIB_NAMESPACE::base {
Time::Time() = default;
Time::~Time() = default;

uint64_t Time::Now() {
  auto epoch_time = std::chrono::system_clock::now().time_since_epoch();
  return std::chrono::duration_cast<std::chrono::milliseconds>(epoch_time)
      .count();
}

}  // namespace _LIB_NAMESPACE::base