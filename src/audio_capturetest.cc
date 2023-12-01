//
// Copyright (c) 2023 mdl. All rights reserved.
//

#include <fstream>
#include <tuple>
#include "base/at_exit.h"
#include "base/log.h"
#include "base/run_loop.h"
#include "base/task/single_thread_task_executor.h"
#include "base/task/thread_pool/thread_pool_instance.h"
#include "base/threading/thread.h"
#include "base/time/time.h"
#include "src/base/log.h"
#include "src/capture/audio_record.h"
#include "src/config/config_list.h"

namespace {

// 单位ms
const uint32_t kAudioCollectionTime = 1'000 * 10;
}  // namespace

class AudioRecordTest : public study::api::CaptureAudioSink,
                        public base::Thread {
 public:
  AudioRecordTest() : audio_record_(this), base::Thread("AUDIO_RECORD_THREAD") {
    StartWithOptions(base::Thread::Options(base::MessagePumpType::DEFAULT, 0));
  }
  ~AudioRecordTest() override = default;

  void GetAudioFormat(study::api::AudioFormat* format) {
    audio_record_.GetAudioFormat(format);
  }

  void Start() {
    base::Thread::Start();
    audio_record_.Start();

    task_runner()->PostDelayedTask(
        FROM_HERE,
        base::BindOnce(&AudioRecordTest::DispatchAudioRecord,
                       base::Unretained(this)),
        base::Milliseconds(2));
  }

  void Stop() {
    audio_record_.Stop();
    base::Thread::Stop();
  }

 private:
  // 最好拷贝一份出去

  void Init() override {
    out.open(study::kAudioWriteFilePath,
             std::ios::out | std::ios::binary | std::ios::app);
    study::api::AudioFormat format;
    format.channels = 2;         // 声道数
    format.bit_depth = 16;       // 位深
    format.sample_rate = 48000;  // 采样率
    audio_record_.Initialize(&format);
    return;
  }

  void CleanUp() override {
    audio_record_.Release();
    out.close();
  }

  void DispatchAudioRecord() {
    audio_record_.HandleReader();
    task_runner()->PostDelayedTask(
        FROM_HERE,
        base::BindOnce(&AudioRecordTest::DispatchAudioRecord,
                       base::Unretained(this)),
        base::Milliseconds(2));
  }

  void OnCaptureAudio(std::unique_ptr<study::base::Buffer> buffer) override {
    LOG(INFO) << "OnAudioData:" << buffer->size() << std::endl;
    out.write(buffer->data(), buffer->size());
  }

 private:
  std::ofstream out;
  study::collection::AudioRecord audio_record_;
};

namespace {
constexpr size_t kMaxNumForegroundThreads = 6;

void InitializeThreadPool(const char* name) {
  base::ThreadPoolInstance::Create(name);

  base::ThreadPoolInstance::InitParams init_params(kMaxNumForegroundThreads);
#if BUILDFLAG(IS_WIN)
  init_params.common_thread_pool_environment = base::ThreadPoolInstance::
      InitParams::CommonThreadPoolEnvironment::COM_MTA;
#endif
  base::ThreadPoolInstance::Get()->Start(init_params);
}
}  // namespace

int main(int argc, char* argv[]) {
  logging::LoggingSettings settings;
  base::AtExitManager at_exit_manager;
  base::SingleThreadTaskExecutor io_task_executor(base::MessagePumpType::IO);
  settings.logging_dest = logging::LOG_TO_FILE | logging::LOG_TO_STDERR;
  settings.log_file_path = L".";
  logging::InitLogging(settings);

  // base::RunLoop().Run();

  base::RunLoop run_loop;
  run_loop.Run();

  AudioRecordTest audio_record_test;
  audio_record_test.Start();
  study::api::AudioFormat format;
  audio_record_test.GetAudioFormat(&format);

  auto task_runner = base::SequencedTaskRunner::GetCurrentDefault();

  // task_runner->PostDelayedTask(
  //     FROM_HERE,
  //     base::BindOnce([](base::RunLoop* run_loop) { run_loop->Quit(); },
  //                    base::Unretained(&run_loop)),
  //     base::Milliseconds(kAudioCollectionTime));

  audio_record_test.Stop();
  run_loop.Run();

  return 0;
}
