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
    return false;                                 \
  }  // namespace

const int kOutputBufferSize = 2048 * 16;
}  // namespace

namespace _LIB_NAMESPACE::decoder {
AACDecoder::AACDecoder(call::DecodeAudioSink* sink, uint32_t id)
    : sink_(sink),
      session_id_(id),
      output_buffer_(
          base::Buffer::New(kOutputBufferSize, base::Buffer::AUDIO_DECODED)) {
  DCHECK(sink);
  output_buffer_->session(session_id_);
}

AACDecoder::~AACDecoder() = default;

bool AACDecoder::Initialize(base::AudioFormat* format, uint16_t aot) {
  if (init_done_)
    return false;
  DCHECK(format);
  DCHECK(format->type == base::AudioFormat::kDecode);
  DCHECK(!decoder_handle_);

  uint8_t type = TT_MP4_ADTS;
  switch (aot) {
    case AOT_AAC_LC:
      type = TT_MP4_ADTS;
      break;
    case AOT_ER_AAC_LD:
      type = TT_MP4_RAW;
      break;
  }

  AAC_DECODER_ERROR h = AAC_DEC_OK;
  decoder_handle_ = aacDecoder_Open(static_cast<TRANSPORT_TYPE>(type), 1);
  if (!decoder_handle_)
    return false;

  AACENC_InfoStruct aac_info;
  if (!GetRawConfig(&aac_info, format, aot, type))
    return false;
  uint8_t* cb[] = {aac_info.confBuf};
  CHECK_AAC_DEC(aacDecoder_ConfigRaw(decoder_handle_, cb, &aac_info.confSize);
                , "aacDecoder_ConfigRaw faild");

  CHECK_AAC_DEC(aacDecoder_SetParam(decoder_handle_, AAC_CONCEAL_METHOD, 0),
                "AAC_CONCEAL_METHOD faild");

  init_done_ = true;
  return true;
}

bool AACDecoder::GetRawConfig(void* info,
                              base::AudioFormat* format,
                              uint8_t aot,
                              uint8_t type) {
  auto h = AACENC_OK;
  struct AACENCODER* encoder_handle = nullptr;
  CHECK_AAC_ENC(aacEncOpen(&encoder_handle, 0, format->decode.channels),
                "aacEncOpen failed");

  // 设置模式 lc ld
  CHECK_AAC_ENC(aacEncoder_SetParam(encoder_handle, AACENC_AOT, aot),
                "aacEncoder_SetParam failed");

  // 设置 采样率
  CHECK_AAC_ENC(aacEncoder_SetParam(encoder_handle, AACENC_SAMPLERATE,
                                    format->decode.sample_rate),
                "aacEncoder_SetParam failed");

  // 声道模式
  CHECK_AAC_ENC(aacEncoder_SetParam(encoder_handle, AACENC_CHANNELMODE,
                                    format->decode.channels),
                "aacEncoder_SetParam failed");

  // 设置 pcm数据格式
  CHECK_AAC_ENC(aacEncoder_SetParam(encoder_handle, AACENC_CHANNELORDER, 1),
                "aacEncoder_SetParam failed");

  CHECK_AAC_ENC(aacEncoder_SetParam(encoder_handle, AACENC_TRANSMUX, type),
                "aacEncoder_SetParam failed");

  // 初始化编码器
  CHECK_AAC_ENC(aacEncEncode(encoder_handle, NULL, NULL, NULL, NULL),
                "aacEncEncode failed");

  AACENC_InfoStruct aac_info;
  CHECK_AAC_ENC(aacEncInfo(encoder_handle, &aac_info), "aacEncInfo failed");

  CHECK_AAC_ENC(
      aacEncInfo(encoder_handle, reinterpret_cast<AACENC_InfoStruct*>(info)),
      "aacEncInfo failed");
  aacEncClose(&encoder_handle);
  encoder_handle = nullptr;
  return true;
}

bool AACDecoder::DecodeAudio(std::unique_ptr<base::Buffer> buffer) {
  if (!init_done_)
    return false;
  DCHECK(sink_);
  if (!buffer || (buffer->type() != base::Buffer::AUDIO_DECODED))
    return false;
  AAC_DECODER_ERROR h = AAC_DEC_OK;

  uint32_t valid = buffer->size();
  auto d = buffer->data<uint8_t>();
  auto size = buffer->size();
  auto output_buffer = output_buffer_->data();

  do {
    h = aacDecoder_Fill(decoder_handle_, &d, &size, &valid);
    d += (buffer->size() - valid);
    size -= (buffer->size() - valid);

    if (h != AAC_DEC_OK)
      return false;

    h = aacDecoder_DecodeFrame(decoder_handle_,
                               reinterpret_cast<INT_PCM*>(output_buffer),
                               kOutputBufferSize / sizeof(INT_PCM), 0);
    if (h == AAC_DEC_NOT_ENOUGH_BITS)
      continue;
    if (h != AAC_DEC_OK)
      return false;

    auto info = aacDecoder_GetStreamInfo(decoder_handle_);

    sink_->OnDecodeAudio(output_buffer_.get(),
                         info->numChannels * info->frameSize * sizeof(INT_PCM));
  } while (size > 0);

  return true;
}

void AACDecoder::Reset() {
  decoder_handle_ ? aacDecoder_Close(decoder_handle_) : void();
}

void AACDecoder::Release() {
  if (!init_done_)
    return;
  Reset();
  init_done_ = false;
}

}  // namespace _LIB_NAMESPACE::decoder
