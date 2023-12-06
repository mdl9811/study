#include "src/encoder/aac_encoder.h"
#include <functional>
#include "src/base/log.h"

extern "C" {
#include "third_party/aac/libAACenc/include/aacenc_lib.h"
}

namespace {
#define CHECK_AAC_ENC(fun, result)                \
  h = fun;                                        \
  if ((h) != AACENC_OK) {                         \
    Reset();                                      \
    LOG(ERROR) << result << " error coed: " << h; \
    return false;                                 \
  }  // namespace

const int kOutputBufferSize = 1024 * 16;
}  // namespace

namespace _LIB_NAMESPACE::encoder {

AACEcoder::AACEcoder(call::EncodeAudioSink* sink, uint32_t id)
    : sink_(sink),
      session_id_(id),
      output_buffer_(
          base::Buffer::New(kOutputBufferSize, base::Buffer::AUDIO_ENCODED)) {
  output_buffer_->session(session_id_);
}

AACEcoder::~AACEcoder() = default;

bool AACEcoder::Initialize(base::AudioFormat* format,
                           uint32_t bitrate,
                           uint16_t aot,
                           uint16_t frame_len) {
  if (init_done_ || !format)
    return false;

  if (format && (format->type != base::AudioFormat::kEncode))
    return false;
  auto h = AACENC_OK;
  // 打开 aac编码器
  CHECK_AAC_ENC(aacEncOpen(&aac_handle_, 0, format->encode.channels),
                "aacEncOpen failed");

  // 设置模式 lc ld
  CHECK_AAC_ENC(aacEncoder_SetParam(aac_handle_, AACENC_AOT, aot),
                "aacEncoder_SetParam failed");

  // 设置 输入帧长
  CHECK_AAC_ENC(
      aacEncoder_SetParam(aac_handle_, AACENC_GRANULE_LENGTH, frame_len),
      "aacEncoder_SetParam failed");

  // 设置 采样率
  CHECK_AAC_ENC(aacEncoder_SetParam(aac_handle_, AACENC_SAMPLERATE,
                                    format->encode.sample_rate),
                "aacEncoder_SetParam failed");

  // 声道模式
  CHECK_AAC_ENC(aacEncoder_SetParam(aac_handle_, AACENC_CHANNELMODE,
                                    format->encode.channels),
                "aacEncoder_SetParam failed");

  // 设置 pcm数据格式
  CHECK_AAC_ENC(aacEncoder_SetParam(aac_handle_, AACENC_CHANNELORDER, 1),
                "aacEncoder_SetParam failed");

  // 设置 编码码率
  CHECK_AAC_ENC(aacEncoder_SetParam(aac_handle_, AACENC_BITRATE, bitrate),
                "aacEncoder_SetParam failed");

  // 设置 编码帧是ADTS AAC-LC[TT_MP4_ADTS] AAC_LD[TT_MP4_RAW]
  uint8_t type = TT_MP4_ADTS;
  switch (aot) {
    case AOT_AAC_LC:
      type = TT_MP4_ADTS;
      break;
    case AOT_ER_AAC_LD:
      type = TT_MP4_RAW;
      break;
  }

  CHECK_AAC_ENC(aacEncoder_SetParam(aac_handle_, AACENC_TRANSMUX, type),
                "aacEncoder_SetParam failed");

  // 初始化编码器
  CHECK_AAC_ENC(aacEncEncode(aac_handle_, NULL, NULL, NULL, NULL),
                "aacEncEncode failed");

  AACENC_InfoStruct aac_info;
  CHECK_AAC_ENC(aacEncInfo(aac_handle_, &aac_info), "aacEncInfo failed");

  init_done_ = true;
  audio_format_ = *format;

  // TT_MAP_RAW 头有点特殊

  return true;
}

void AACEcoder::Reset() {
  aac_handle_ ? aacEncClose(&aac_handle_) : AACENC_ERROR();
  aac_handle_ = nullptr;
}

void AACEcoder::Release() {
  if (!init_done_)
    return;
  Reset();
  init_done_ = false;
}

bool AACEcoder::EncodeAudio(std::unique_ptr<base::Buffer> buffer) {
  if (!init_done_)
    return false;

  if (!buffer || (buffer->type() != base::Buffer::AUDIO_ENCODED))
    return false;

  HandleEncode(std::move(buffer));
  return true;
}

void AACEcoder::HandleEncode(std::unique_ptr<base::Buffer> buffer) {
  if (!init_done_ || !sink_)
    return;
  struct {
    AACENC_BufDesc in_buf, out_buf;
    AACENC_InArgs in_args;
    AACENC_OutArgs out_args;
  } var;

  memset(&var, 0, sizeof(var));

  int in_identifier = IN_AUDIO_DATA;
  int in_elem_size = 2;
  int in_size = buffer->size();

  int out_identifier = OUT_BITSTREAM_DATA;
  int out_size = kOutputBufferSize;
  int out_elem_size = 1;

  const char* input_ptr = buffer->data();

  var.in_args.numInSamples = buffer->size() / 2;
  var.in_buf.numBufs = 1;
  var.in_buf.bufs = reinterpret_cast<void**>(const_cast<char**>(&input_ptr));
  var.in_buf.bufferIdentifiers = &in_identifier;
  var.in_buf.bufSizes = &in_size;
  var.in_buf.bufElSizes = &in_elem_size;

  void* ptr = output_buffer_->data();
  var.out_buf.numBufs = 1;
  var.out_buf.bufs = &ptr;
  var.out_buf.bufferIdentifiers = &out_identifier;
  var.out_buf.bufSizes = &out_size;
  var.out_buf.bufElSizes = &out_elem_size;

  if (aacEncEncode(aac_handle_, &var.in_buf, &var.out_buf, &var.in_args,
                   &var.out_args) != AACENC_OK) {
    LOG(ERROR) << "Encoding error";
    return;
  }

  sink_->OnEncodeAudio(output_buffer_.get(), var.out_args.numOutBytes);
}

}  // namespace _LIB_NAMESPACE::encoder