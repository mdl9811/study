#include "src/capture/audio_record.h"
#include "src/base/log.h"

#if BUILDFLAG(IS_WIN)
#include <Audioclient.h>
#include <Mmdeviceapi.h>
#endif
namespace {
#if BUILDFLAG(IS_WIN)
const IID kIIDIMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const CLSID kCLSIDMMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID kIIDIAudioClient = __uuidof(IAudioClient);

#define TEST_OUT_LOG(flag, debug_val, ...) \
  if (flag) {                              \
    Reset();                               \
    LOG(INFO) << debug_val << __VA_ARGS__; \
    return false;                          \
  }

#endif
// 一百纳秒为单位
//  10ms default
#define REFTIMES_PER_SEC 10000000
#define REFTIMES_PER_MILLISEC 10000

}  // namespace

namespace _LIB_NAMESPACE::collection {

AudioRecord::AudioRecord(call::CaptureAudioSink* sink) : sink_(sink) {}

AudioRecord::~AudioRecord() = default;

bool AudioRecord::Initialize(const base::AudioFormat* format) {
  if (init_done_)
    return false;
  if (format && (format->type != base::AudioFormat::kCapture))
    return false;

#if BUILDFLAG(IS_WIN)
  CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
  auto hr = CoCreateInstance(kCLSIDMMDeviceEnumerator, NULL, CLSCTX_ALL,
                             kIIDIMMDeviceEnumerator,
                             (void**)&imm_device_enumerator_);
  TEST_OUT_LOG(FAILED(hr), "CoCreateInstance Faild result:", hr);

  hr = imm_device_enumerator_->GetDefaultAudioEndpoint(eCapture, eConsole,
                                                       &imm_device_);

  TEST_OUT_LOG(FAILED(hr), "GetDefaultAudioEndpoint Faild result:", hr);
  // TODO: check

  hr = imm_device_->Activate(kIIDIAudioClient, CLSCTX_ALL, NULL,
                             (void**)&audio_client_);
  TEST_OUT_LOG(FAILED(hr), "Activate Faild result:", hr);

  format_.wFormatTag = WAVE_FORMAT_PCM;
  format_.nChannels = format->capture.channels;
  format_.nSamplesPerSec = format->capture.sample_rate;
  format_.wBitsPerSample = format->capture.bits;
  format_.nBlockAlign = format_.nChannels * (format_.wBitsPerSample / 8);
  format_.nAvgBytesPerSec = format_.nSamplesPerSec * format_.nBlockAlign;
  format_.cbSize = 0;

  hr = audio_client_->Initialize(
      AUDCLNT_SHAREMODE_SHARED,
      AUDCLNT_STREAMFLAGS_EVENTCALLBACK | AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM,
      REFTIMES_PER_SEC, 0, &format_, NULL);
  TEST_OUT_LOG(FAILED(hr), "AudioClient Initialize Faild result:", hr);

  auto handle = CreateEventA(NULL, TRUE, FALSE, NULL);
  TEST_OUT_LOG(!handle, "CreateEventA Faild result:", GetLastError());

  event_handle_ = handle;
  hr = audio_client_->SetEventHandle(handle);

  TEST_OUT_LOG(FAILED(hr), "AudioClient SetEventHandle Faild result:", hr);

  hr = audio_client_->GetService(_uuidof(IAudioCaptureClient),
                                 (void**)&iaudio_capture_client_);
  TEST_OUT_LOG(FAILED(hr), "AudioClient GetService Faild result:", hr);

#endif
  init_done_ = true;
  return true;
}

bool AudioRecord::Start() {
  if (!init_done_ || start_capture_)
    return false;
  auto hr = audio_client_->Start();
  TEST_OUT_LOG(FAILED(hr), "AudioClient Start Faild result:", hr);

  start_capture_.store(true);
  return true;
}

void AudioRecord::Stop() {
  if (!init_done_ || !start_capture_)
    return;
  audio_client_->Stop();
  start_capture_.store(false);
  SetEvent(event_handle_);
}

void AudioRecord::HandleReader(uint64_t block, bool* done) {
  if (!start_capture_.load()) {
    *done = true;
    return;
  }
  WaitForSingleObject(event_handle_, block) == WAIT_OBJECT_0
      ? HandleAudioBuffer()
      : void();
}

void AudioRecord::HandleAudioBuffer() {
  if (!init_done_)
    return;
  DCHECK(sink_);
#if BUILDFLAG(IS_WIN)
  DWORD flags = -1;

  UINT32 packet_size = 0;
  auto hr = iaudio_capture_client_->GetNextPacketSize(&packet_size);
  if (FAILED(hr)) {
    LOG(ERROR) << "IAudioCaptureClient GetNextPacketSize Faild result:" << hr;
    return;
  }

  while (packet_size != 0) {
    char* data = nullptr;
    uint32_t buffer_size = 0;
    hr = iaudio_capture_client_->GetBuffer((BYTE**)&data, &buffer_size, &flags,
                                           NULL, NULL);
    if (FAILED(hr)) {
      LOG(ERROR) << "IAudioCaptureClient GetBuffer Faild result:" << hr;
      return;
    }

    if (flags & AUDCLNT_BUFFERFLAGS_SILENT) {
      return;  //
    }

    uint16_t copy_size = buffer_size * format_.nBlockAlign;
    auto buffer =
        base::Buffer::New(data, copy_size, base::Buffer::AUDIO_CAPTURE);

    hr = iaudio_capture_client_->ReleaseBuffer(buffer_size);
    if (FAILED(hr)) {
      LOG(ERROR) << "IAudioCaptureClient ReleaseBuffer Faild result:" << hr;
      return;
    }

    hr = iaudio_capture_client_->GetNextPacketSize(&packet_size);
    if (FAILED(hr)) {
      LOG(ERROR) << "IAudioCaptureClient GetNextPacketSize Faild result:" << hr;
      return;
    }

    sink_->OnCaptureAudio(std::move(buffer));
  }

#endif
  return;
}

void AudioRecord::Reset() {
#if BUILDFLAG(IS_WIN)
  ULONG hr = 0;
  hr = imm_device_enumerator_ ? imm_device_enumerator_->Release() : 100;
  hr = imm_device_collection_ ? imm_device_collection_->Release() : 100;
  hr = iaudio_capture_client_ ? iaudio_capture_client_->Release() : 100;
  hr = imm_device_ ? imm_device_->Release() : 100;
  hr = audio_client_ ? audio_client_->Release() : 100;
  event_handle_ ? CloseHandle(event_handle_) : 100;

#endif
  event_handle_ = nullptr;
}

void AudioRecord::Release() {
  if (!init_done_)
    return;
  init_done_ = false;
  Reset();
}

}  // namespace _LIB_NAMESPACE::collection