//
// Copyright (c) 2023 mdl. All rights reserved.
//

#include <fstream>
#include "base/run_loop.h"
#include "base/threading/thread.h"
#include "base/time/time.h"
#include "src/base/log.h"
#include "src/call/audio/encode_sink.h"
#include "src/config/config_list.h"
#include "src/decoder/aac_decoder.h"
#include "src/encoder/aac_encoder.h"

#include "base/at_exit.h"
#include "base/task/single_thread_task_executor.h"

class AACTest : protected base::Thread,
                public study::call::EncodeAudioSink,
                public study::call::DecodeAudioSink {
 public:
  explicit AACTest(uint32_t sample_bits)
      : base::Thread("AAC_THREAD"),
        aac_encoder_(this, 0),
        aac_decoder_(this, 0),
        sample_bits_(sample_bits) {
    StartWithOptions(base::Thread::Options(base::MessagePumpType::DEFAULT, 0));
  }
  ~AACTest() override { Stop(); }

  void EncodeAudio(const char* data, size_t size) {
    auto buffer = study::base::Buffer::New(data, size,
                                           study::base::Buffer::AUDIO_ENCODED);

    task_runner()->PostTask(
        FROM_HERE, base::BindOnce(&AACTest::DispatchAACEncode,
                                  base::Unretained(this), std::move(buffer)));
  }

 private:
  void Init() override {
    study::base::AudioFormat eformat(study::base::AudioFormat::kEncode);
    eformat.encode.channels = 2;         // 声道数
    eformat.encode.bits = 16;            // 位深
    eformat.encode.sample_rate = 48000;  // 采样率
    aac_encoder_.Initialize(&eformat, 64000, 23, sample_bits_);

    study::base::AudioFormat dformat(study::base::AudioFormat::kDecode);
    dformat.decode.channels = 2;
    dformat.decode.sample_rate = 48000;
    aac_decoder_.Initialize(&dformat, 23);
  }

  void CleanUp() override {
    aac_encoder_.Release();
    aac_decoder_.Release();
  }
  void DispatchAACEncode(std::unique_ptr<study::base::Buffer> buffer) {
    aac_encoder_.EncodeAudio(std::move(buffer));
  }

  void OnEncodeAudio(const study::base::Buffer* buffer, size_t size) override {
    LOG(INFO) << "EncodeAudio size: " << size;

    auto ptr = const_cast<study::base::Buffer*>(buffer)->data();
    auto buf =
        study::base::Buffer::New(ptr, size, study::base::Buffer::AUDIO_DECODED);
    aac_decoder_.DecodeAudio(std::move(buf));
  }

  void OnDecodeAudio(const study::base::Buffer* buffer, size_t size) override {
    LOG(INFO) << "OnDecodeAudio size: " << size;
  }

 private:
  uint32_t sample_bits_;
  study::encoder::AACEcoder aac_encoder_;
  study::decoder::AACDecoder aac_decoder_;
};

int main(int argc, char* argv[]) {
  logging::LoggingSettings settings;
  base::AtExitManager at_exit_manager;
  base::SingleThreadTaskExecutor io_task_executor(base::MessagePumpType::IO);
  settings.logging_dest = logging::LOG_TO_FILE | logging::LOG_TO_STDERR;
  settings.log_file_path = L".";
  logging::InitLogging(settings);

  // 采样点数
  const uint32_t kSampleBits = 512;
  AACTest aac_test(kSampleBits);

  std::ifstream in(study::kAudioWriteFilePath, std::ios::in | std::ios::binary);

  if (!in)
    return 0;

  while (!in.eof()) {
    char* data = new char[kSampleBits * 4];
    in.read(data, kSampleBits * 4);
    aac_test.EncodeAudio(data, kSampleBits * 4);
    delete[] data;
  }

  in.close();
  return 0;
}