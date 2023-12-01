//
// Copyright (c) 2023 mdl. All rights reserved.
//

#include <fstream>
#include <tuple>
#include "src/base/logging.h"
#include "src/base/time/time.h"
#include "src/capture/audio_record.h"
#include "src/config/config_list.h"
namespace {

// 单位ms
const uint32_t kAudioCollectionTime = 1'000 * 10;
}  // namespace

class AudioRecordTest : public study::api::CaptureAudioSink {
 public:
  AudioRecordTest() : audio_record_(this) {}
  ~AudioRecordTest() override {
    out.close();
    audio_record_.Relase();
  }

  bool Init() {
    out.open(study::kAudioWriteFilePath,
             std::ios::out | std::ios::binary | std::ios::app);
    study::api::AudioFormat format;
    format.channels = 2;         // 声道数
    format.bit_depth = 16;       // 位深
    format.sample_rate = 48000;  // 采样率
    return audio_record_.Initialize(&format);
  }
  void GetAudioFormat(study::api::AudioFormat* format) {
    audio_record_.GetAudioFormat(format);
  }

  void Start() { audio_record_.Start(); }

  void Stop() { audio_record_.Stop(); }

 private:
  // 最好拷贝一份出去

  void OnCaptureAudio(std::unique_ptr<study::base::Buffer> buffer) override {
    LOG(INFO) << "OnAudioData:" << buffer->size() << std::endl;
    out.write(buffer->data(), buffer->size());
  }

 private:
  std::ofstream out;
  study::collection::AudioRecord audio_record_;
};

int main(int argc, char* argv[]) {
  AudioRecordTest audio_record_test;

  bool ret = audio_record_test.Init();

  audio_record_test.Start();
  study::api::AudioFormat format;
  audio_record_test.GetAudioFormat(&format);
  auto start_time = study::base::Time::Now();

  LOG(INFO) << "audio recording start time:" << start_time << std::endl;
  while (1) {
    if (study::base::Time::Now() - start_time >= kAudioCollectionTime)
      break;
    Sleep(3);
  }
  audio_record_test.Stop();
  return 0;
}
