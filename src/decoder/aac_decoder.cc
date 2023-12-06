#include "src/decoder/aac_decoder.h"
#include "src/base/log.h"

extern "C" {
#include "third_party/aac/libAACdec/include/aacdecoder_lib.h"
#include "third_party/aac/libAACenc/include/aacenc_lib.h"
}

namespace {
#define CHECK_AAC_DEC(fun, result)                \
  h = fun;                                        \
  if ((h) != AAC_DEC_OK) {                        \
    Reset();                                      \
    LOG(ERROR) << result << " error coed: " << h; \
    return false;                                 \
  }  // namespace

#define CHECK_AAC_ENC(fun, result)                \
  h = fun;                                        \
  if ((h) != AACENC_OK) {                         \
    aacEncClose(&encoder_handle);                 \
    LOG(ERROR) << result << " error coed: " << h; \
    return;                                       \
  }  // namespace

const int kOutputBufferSize = 2048 * 16;
}  // namespace

namespace _LIB_NAMESPACE::decoder {
AACDecoder::AACDecoder(call::DecodeAudioSink* sink, uint32_t id)
    : sink_(sink),
      session_id_(id),
      output_buffer_(
          base::Buffer::New(kOutputBufferSize, base::Buffer::AUDIO_DECODED)) {
  output_buffer_->session(session_id_);
}

AACDecoder::~AACDecoder() = default;

bool AACDecoder::Initialize(base::AudioFormat* format,
                            uint16_t aot,
                            uint16_t type) {
  if (init_done_)
    return false;
  if (format && (format->type != base::AudioFormat::kDecode))
    return false;

  AAC_DECODER_ERROR h = AAC_DEC_OK;
  decoder_handle_ = aacDecoder_Open(static_cast<TRANSPORT_TYPE>(type), 1);
  if (!decoder_handle_)
    return false;

  std::unique_ptr<uint8_t[]> conf;
  uint32_t conf_size = 0;
  GetRawConfig(&conf, &conf_size, format, aot, type);

  if (!conf.get())
    return false;
  uint8_t* cb[] = {conf.get()};
  CHECK_AAC_DEC(aacDecoder_ConfigRaw(decoder_handle_, cb, &conf_size);
                , "aacDecoder_ConfigRaw faild");

  CHECK_AAC_DEC(aacDecoder_SetParam(decoder_handle_, AAC_CONCEAL_METHOD, 1),
                "AAC_CONCEAL_METHOD faild");
  CHECK_AAC_DEC(aacDecoder_SetParam(decoder_handle_, AAC_PCM_LIMITER_ENABLE, 0),
                "AAC_PCM_LIMITER_ENABLE faild");

  init_done_ = true;
  return true;
}

void AACDecoder::GetRawConfig(std::unique_ptr<uint8_t[]>* conf,
                              uint32_t* size,
                              base::AudioFormat* format,
                              uint8_t aot,
                              uint8_t type) {
  auto h = AACENC_OK;
  struct AACENCODER* encoder_handle = nullptr;
  // 打开 aac编码器
  CHECK_AAC_ENC(aacEncOpen(&encoder_handle, 0, format->decode.channels),
                "aacEncOpen failed");

  // 设置模式 lc ld
  CHECK_AAC_ENC(aacEncoder_SetParam(encoder_handle, AACENC_AOT, aot),
                "aacEncoder_SetParam failed");

  CHECK_AAC_ENC(aacEncoder_SetParam(encoder_handle, AACENC_SAMPLERATE,
                                    format->decode.sample_rate),
                "aacEncoder_SetParam failed");
  // 声道模式
  CHECK_AAC_ENC(aacEncoder_SetParam(encoder_handle, AACENC_CHANNELMODE,
                                    format->decode.channels),
                "aacEncoder_SetParam failed");

  CHECK_AAC_ENC(aacEncoder_SetParam(encoder_handle, AACENC_TRANSMUX, type),
                "aacEncoder_SetParam failed");

  // 初始化编码器
  CHECK_AAC_ENC(aacEncEncode(encoder_handle, NULL, NULL, NULL, NULL),
                "aacEncEncode failed");

  AACENC_InfoStruct aac_info;
  CHECK_AAC_ENC(aacEncInfo(encoder_handle, &aac_info), "aacEncInfo failed");
  *size = aac_info.confSize;
  auto buffer = new uint8_t[*size];
  memcpy(buffer, aac_info.confBuf, *size);
  (*conf).reset(buffer);
}

bool AACDecoder::DecodeAudio(std::unique_ptr<base::Buffer> buffer) {
  if (!init_done_ || !sink_)
    return false;

  if (!buffer || (buffer->type() != base::Buffer::AUDIO_DECODED))
    return false;
  AAC_DECODER_ERROR h = AAC_DEC_OK;

  uint32_t valid = buffer->size();
  auto d = buffer->data<uint8_t>();
  auto size = buffer->size();

  CHECK_AAC_DEC(aacDecoder_Fill(decoder_handle_, &d, &size, &valid),
                "aacDecoder_Fill failed");

  CHECK_AAC_DEC(
      aacDecoder_DecodeFrame(decoder_handle_,
                             reinterpret_cast<INT_PCM*>(output_buffer_->data()),
                             kOutputBufferSize / sizeof(INT_PCM), 0),
      "aacDecoder_DecodeFrame failed");

  auto info = aacDecoder_GetStreamInfo(decoder_handle_);

  sink_->OnDecodeAudio(output_buffer_.get(),
                       info->numChannels * info->frameSize * sizeof(INT_PCM));

  return true;
}

void AACDecoder::Reset() {
  decoder_handle_ ? aacDecoder_Close(decoder_handle_) : void();
}

void AACDecoder::Release() {
  if (!init_done_)
    return;
  Reset();
}

}  // namespace _LIB_NAMESPACE::decoder
