#include "src/encoder/aac_encoder.h"
#include <functional>
#include "src/base/logging.h"
#include "third_party/aac/libAACenc/include/aacenc_lib.h"

extern "C" {
#include "third_party/aac/libAACenc/include/aacenc_lib.h"
}

#if BUILDFLAG(IS_WIN)
#include "src/base/thread/thread_win.h"
#endif

namespace {
#define CHECK_AAC(fun, result)                                 \
  h = fun;                                                     \
  if ((h) != AACENC_OK) {                                      \
    Release();                                                 \
    LOG(ERROR) << result << " error coed: " << h << std::endl; \
    return false;                                              \
  }  // namespace

}  // namespace

namespace _LIB_NAMESPACE::encoder {

const int kOutputBufferSize = 1024 * 16;

AACEcoder::AACEcoder(api::EncodeAudioSink* sink, uint32_t id)
    : sink_(sink),
      session_id_(id),
      output_buffer_(new char[kOutputBufferSize]),
      run_loop_(std::make_unique<RunLoop>("__ENCODER_AUDIO__")) {}
AACEcoder::~AACEcoder() = default;

bool AACEcoder::Initialize(api::AudioFormat* format,
                           uint32_t bitrate,
                           uint16_t aot,
                           uint16_t frame_len) {
  if (init_done_ || !format)
    return false;
  auto h = AACENC_OK;
  // 打开 aac编码器
  CHECK_AAC(aacEncOpen(&aac_handle_, 0, format->channels), "aacEncOpen failed");

  // 设置模式 lc ld
  CHECK_AAC(aacEncoder_SetParam(aac_handle_, AACENC_AOT, aot),
            "aacEncoder_SetParam failed");

  // 设置 输入帧长
  CHECK_AAC(aacEncoder_SetParam(aac_handle_, AACENC_GRANULE_LENGTH, frame_len),
            "aacEncoder_SetParam failed");

  // 设置 采样率
  CHECK_AAC(
      aacEncoder_SetParam(aac_handle_, AACENC_SAMPLERATE, format->sample_rate),
      "aacEncoder_SetParam failed");

  // 声道模式
  CHECK_AAC(
      aacEncoder_SetParam(aac_handle_, AACENC_CHANNELMODE, format->channels),
      "aacEncoder_SetParam failed");

  // 设置 pcm数据格式
  CHECK_AAC(aacEncoder_SetParam(aac_handle_, AACENC_CHANNELORDER, 1),
            "aacEncoder_SetParam failed");

  // 设置 编码码率
  CHECK_AAC(aacEncoder_SetParam(aac_handle_, AACENC_BITRATE, bitrate),
            "aacEncoder_SetParam failed");

  // 设置 编码帧是ADTS AAC-LC[TT_MP4_ADTS] AAC_LD[TT_MP4_RAW]
  uint8_t mode = TT_MP4_ADTS;
  switch (aot) {
    case AOT_AAC_LC:
      mode = TT_MP4_ADTS;
      break;
    case AOT_ER_AAC_LD:
      mode = TT_MP4_RAW;
      break;
  }

  CHECK_AAC(aacEncoder_SetParam(aac_handle_, AACENC_TRANSMUX, mode),
            "aacEncoder_SetParam failed");

  // 初始化编码器
  CHECK_AAC(aacEncEncode(aac_handle_, NULL, NULL, NULL, NULL),
            "aacEncEncode failed");

  AACENC_InfoStruct aac_info;
  CHECK_AAC(aacEncInfo(aac_handle_, &aac_info), "aacEncInfo failed");

  frame_len_ = frame_len;

  init_done_ = true;
  audio_format_ = *format;
  run_loop_->Start();
  return true;
}

void AACEcoder::Release() {
  aac_handle_ ? aacEncClose(&aac_handle_) : AACENC_ERROR();

  aac_handle_ = nullptr;
  if (!init_done_)
    return;
  run_loop_->Stop();
  init_done_ = false;
}

bool AACEcoder::EncodeAudio(const char* data, size_t size) {
  if (!init_done_)
    return false;

  auto buffer = base::IOBuffer::New(data, size, base::IOBuffer::AUDIO_ENCODED);
  run_loop_->PostTask(
      std::bind(&AACEcoder::HandleEncode, this, std::move(buffer)));
  return true;
}

void AACEcoder::HandleEncode(scoped_refptr<base::IOBuffer> buffer) {
  if (!init_done_)
    return;
  struct {
    AACENC_BufDesc in_buf, out_buf;
    AACENC_InArgs in_args;
    AACENC_OutArgs out_args;
  } var;

  memset(&var, 0, sizeof(var));

  int in_identifier = IN_AUDIO_DATA;
  int in_elem_size = audio_format_.bit_depth / 8;
  int in_size = buffer->size();

  int out_identifier = OUT_BITSTREAM_DATA;
  int out_size = kOutputBufferSize;
  int out_elem_size = 1;

  char* input_ptr = buffer->data();

  var.in_args.numInSamples = buffer->size() / in_elem_size;
  var.in_buf.numBufs = 1;
  var.in_buf.bufs = reinterpret_cast<void**>(&input_ptr);
  var.in_buf.bufferIdentifiers = &in_identifier;
  var.in_buf.bufSizes = &in_size;
  var.in_buf.bufElSizes = &in_elem_size;

  void* ptr = output_buffer_.get();
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

  if (sink_) {
    auto buf = base::Buffer::New(output_buffer_.get(), var.out_args.numOutBytes,
                                 base::Buffer::AUDIO_ENCODED);
    buf->session(session_id_);
    sink_->OnEncodeAudio(std::move(buf));
  }
}

}  // namespace _LIB_NAMESPACE::encoder