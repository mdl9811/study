#include "src/base/thread/thread_win.h"
#include "src/base/logging.h"

namespace {
inline std::wstring StringToWString(const std::string& str) {
  int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
  wchar_t* wide = new wchar_t[len + 1];
  memset(wide, '\0', sizeof(wchar_t) * (len + 1));
  MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, wide, len);
  std::wstring w_str(wide);
  delete[] wide;
  return w_str;
}

#define CHECK_OK(h, result)                              \
  if (!h) {                                              \
    Reset();                                             \
    LOG(ERROR) << result << GetLastError() << std::endl; \
    return;                                              \
  }  // namespace

}  // namespace

namespace _LIB_NAMESPACE::base {

ThreadWin::ThreadWin(const std::string& name) : ThreadMessageLoop(name) {}
ThreadWin::~ThreadWin() {
  Stop();
}

void ThreadWin::Start() {
  if (init_done_)
    return;
  auto mutex = CreateMutexA(NULL, FALSE, NULL);
  CHECK_OK(mutex, "CreateMutexA failed: ");

  auto event = CreateEventA(NULL, TRUE, FALSE, NULL);
  CHECK_OK(event, "CreateEventA failed: ");

  auto handle = CreateThread(NULL, 0, &ThreadWin::ThreadRunWin, this, 0, NULL);
  CHECK_OK(handle, "CreateThread failed: ");

  SetThreadDescription(handle, StringToWString(name_).c_str());
  thread_handle_ = handle;
  mutex_ = mutex;
  event_ = event;
  init_done_ = true;
}

void ThreadWin::Stop() {
  if (!init_done_)
    return;
  is_running_ = false;
  SetEvent(event_);
  ReleaseMutex(mutex_);
  if (thread_handle_ &&
      (WaitForSingleObject(thread_handle_, 10) == WAIT_OBJECT_0)) {
    Reset();
  }
  thread_handle_ = nullptr;
  init_done_ = false;
}

void ThreadWin::Reset() {
  thread_handle_ ? CloseHandle(thread_handle_) : 100;
  mutex_ ? CloseHandle(mutex_) : 100;
  event_ ? CloseHandle(event_) : 100;
}

void ThreadWin::PostTask(Task task) {
  if (!init_done_)
    return;
  if (WaitForSingleObject(mutex_, 1) == WAIT_OBJECT_0) {
    task_queue_.emplace(std::move(task));
    ReleaseMutex(mutex_);
  }
  SetEvent(event_);
}

DWORD WINAPI ThreadWin::ThreadRunWin(_In_ LPVOID lpParameter) {
  ThreadWin* run_loop = reinterpret_cast<ThreadWin*>(lpParameter);
  Task task;
  while (run_loop->is_running_) {
    if (WaitForSingleObject(run_loop->event_, INFINITE) == WAIT_OBJECT_0) {
      while (!run_loop->task_queue_.empty()) {
        if (WaitForSingleObject(run_loop->mutex_, 1) == WAIT_OBJECT_0) {
          auto tk = run_loop->task_queue_.front();
          task = std::move(tk);
          run_loop->task_queue_.pop();
          ReleaseMutex(run_loop->mutex_);
        }
        task();
      }
      ResetEvent(run_loop->event_);
    }
  }
  return 0;
}

}  // namespace _LIB_NAMESPACE::base