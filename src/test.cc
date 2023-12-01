//
// Copyright (c) 2023 mdl. All rights reserved.
//

#include <Windows.h>
#include <iostream>
#include "base/at_exit.h"
#include "base/logging.h"
#include "base/task/single_thread_task_executor.h"
#include "base/time/time.h"

int main(int argc, char* argv[]) {
  auto a = base::Time::Now();

  logging::LoggingSettings settings;
  base::AtExitManager at_exit_manager;
  base::SingleThreadTaskExecutor io_task_executor(base::MessagePumpType::IO);
  settings.logging_dest = logging::LOG_TO_FILE | logging::LOG_TO_STDERR;
  settings.log_file_path = L".";
  logging::InitLogging(settings);
  LOG(INFO) << "ss" << base::Time::Now().ToJavaTime() << std::endl;

  return 0;
}