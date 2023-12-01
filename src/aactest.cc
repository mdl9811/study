//
// Copyright (c) 2023 mdl. All rights reserved.
//

#include <fstream>
#include "src/api/audio/encode_sink.h"
#include "src/base/functional/callback.h"
#include "src/base/logging.h"
#include "src/base/time/time.h"
#include "src/config/config_list.h"
#include "src/decoder/aac_decoder.h"
#include "src/encoder/aac_encoder.h"

class AACTest : public study::api::EncodeAudioSink {
 public:
  explicit AACTest() : aac_encoder_(this, 0) {}
  ~AACTest() override { aac_encoder_.Release(); }

  void Init() {
    study::api::AudioFormat format;
    format.channels = 2;         // 声道数
    format.bit_depth = 16;       // 位深
    format.sample_rate = 48000;  // 采样率
    aac_encoder_.Initialize(&format, 64000, 23, 512);
  }

  bool EncodeAudio(const char* data, size_t size) {
    return aac_encoder_.EncodeAudio(data, size);
  }

 private:
  void OnEncodeAudio(std::unique_ptr<study::base::Buffer> buffer) override {
    LOG(INFO) << "EncodeAudio size: " << buffer->size() << std::endl;
  }

 private:
  study::encoder::AACEcoder aac_encoder_;
};

int main(int argc, char* argv[]) {
  AACTest aactest;

  std::ifstream in(study::kAudioWriteFilePath, std::ios::in | std::ios::binary);

  aactest.Init();
  if (!in)
    return 0;

  while (!in.eof()) {
    char* data = new char[512 * 4];
    in.read(data, 512 * 4);
    aactest.EncodeAudio(data, 512 * 4);
    delete[] data;
  }
  while (true) {
    Sleep(1000);
  }

  in.close();
  return 0;
}