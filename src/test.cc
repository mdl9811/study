//
// Copyright (c) 2023 mdl. All rights reserved.
//

#include <Windows.h>
#include <iostream>
#include "base/at_exit.h"
#include "base/log.h"
#include "base/run_loop.h"
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
  LOG(ERROR) << "ss" << base::Time::Now().ToJavaTime();

  auto task_runner = base::SequencedTaskRunner::GetCurrentDefault();

  task_runner->PostTask(FROM_HERE, base::BindOnce([]() { LOG(INFO) << "ss"; }));

  base::RunLoop().Run();

  return 0;
}