//
// Copyright (c) 2023 mdl. All rights reserved.
//

#include <fstream>
#include "base/run_loop.h"
#include "base/threading/thread.h"
#include "base/time/time.h"
#include "src/api/audio/encode_sink.h"
#include "src/base/log.h"
#include "src/config/config_list.h"
#include "src/decoder/aac_decoder.h"
#include "src/encoder/aac_encoder.h"
class AACTest : public study::api::EncodeAudioSink, public base::Thread {
 public:
  explicit AACTest() : aac_encoder_(this, 0), base::Thread("AAC_THREAD") {
    StartWithOptions(base::Thread::Options(base::MessagePumpType::DEFAULT, 0));
  }
  ~AACTest() override = default;

  void EncodeAudio(const char* data, size_t size) {
    auto buffer = study::base::Buffer::New(data, size,
                                           study::base::Buffer::AUDIO_ENCODED);

    task_runner()->PostTask(
        FROM_HERE, base::BindOnce(&AACTest::DispatchAACEncode,
                                  base::Unretained(this), std::move(buffer)));
  }

 private:
  void Init() override {
    study::api::AudioFormat format;
    format.channels = 2;         // 声道数
    format.bit_depth = 16;       // 位深
    format.sample_rate = 48000;  // 采样率
    aac_encoder_.Initialize(&format, 64000, 23, 512);
  }
  void CleanUp() override { aac_encoder_.Release(); }
  void DispatchAACEncode(std::unique_ptr<study::base::Buffer> buffer) {
    aac_encoder_.EncodeAudio(std::move(buffer));
  }

  void OnEncodeAudio(std::unique_ptr<study::base::Buffer> buffer) override {
    LOG(INFO) << "EncodeAudio size: " << buffer->size();
  }

 private:
  study::encoder::AACEcoder aac_encoder_;
};

int main(int argc, char* argv[]) {
  AACTest aac_test;

  std::ifstream in(study::kAudioWriteFilePath, std::ios::in | std::ios::binary);

  aac_test.Start();

  if (!in)
    return 0;

  while (!in.eof()) {
    char* data = new char[512 * 4];
    in.read(data, 512 * 4);
    aac_test.EncodeAudio(data, 512 * 4);
    delete[] data;
  }

  aac_test.Stop();

  base::RunLoop().Run();

  in.close();
  return 0;
}