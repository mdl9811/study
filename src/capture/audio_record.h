//
// Copyright (c) 2023 mdl. All rights reserved.
//

#ifndef SRC_CAPTURE_AUDIO_RECORD_H_
#define SRC_CAPTURE_AUDIO_RECORD_H_

#include <atomic>
#include <memory>
#include "src/api/audio/audio_format.h"
#include "src/api/audio/capture_sink.h"
#include "src/config/config_site.h"

#if BUILDFLAG(IS_WIN)
#include <windows.h>

#include <mmsystem.h>

class IMMDeviceEnumerator;
class IMMDeviceCollection;
class IAudioCaptureClient;
class IMMDevice;
class IAudioClient;
#endif

namespace _LIB_NAMESPACE::collection {
class AudioRecord {
 public:
  explicit AudioRecord(api::CaptureAudioSink* callback);

  AudioRecord(const AudioRecord&) = delete;
  AudioRecord& operator=(const AudioRecord&) = delete;
  virtual ~AudioRecord();
  bool Start();
  void Stop();

  bool Initialize(const api::AudioFormat* format);

  void Release();

  void GetAudioFormat(api::AudioFormat* format);

  void HandleReader(uint64_t block, bool* done);

 private:
  void HandleAudioBuffer();
  void Reset();

 private:
#if BUILDFLAG(IS_WIN)
  IMMDeviceEnumerator* imm_device_enumerator_ = nullptr;
  IMMDeviceCollection* imm_device_collection_ = nullptr;
  IAudioCaptureClient* iaudio_capture_client_ = nullptr;
  IMMDevice* imm_device_ = nullptr;

  IAudioClient* audio_client_ = nullptr;
  WAVEFORMATEX format_;
#endif

  api::CaptureAudioSink* sink_ = nullptr;

  void* event_handle_ = nullptr;
  bool init_done_ = false;
  std::atomic_bool start_capture_ = false;

  api::AudioFormat audio_format_;
};

}  // namespace _LIB_NAMESPACE::collection

#endif