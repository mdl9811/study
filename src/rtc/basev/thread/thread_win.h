//
// Copyright (c) 2023 mdl. All rights reserved.
//

#ifndef SRC_BASE_THREAD_THREAD_WIN_H_
#define SRC_BASE_THREAD_THREAD_WIN_H_

#include <Windows.h>

#include <queue>
#include "src/base/thread/thread_runloop.h"

namespace _LIB_NAMESPACE::base {
class ThreadWin : public ThreadMessageLoop {
 public:
  explicit ThreadWin(const std::string& name);
  virtual ~ThreadWin();

  void PostTask(Task task) override;
  void Start() override;
  void Stop() override;

 private:
  void Reset();

  static DWORD WINAPI ThreadRunWin(_In_ LPVOID lpParameter);

 private:
  bool is_running_ = true;
  bool init_done_ = false;

  HANDLE event_ = nullptr;
  HANDLE mutex_ = nullptr;
  HANDLE thread_handle_ = nullptr;

  std::queue<Task> task_queue_;
};
}  // namespace _LIB_NAMESPACE::base

#endif