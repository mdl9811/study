#include "src/encoder/opus_encoder.h"
#include "src/base/log.h"

extern "C" {
#include "third_party/opus/include/opus.h"
}

namespace {
#define CHECK_OPUS_ENC(fun, result)               \
  h = fun;                                        \
  if ((h) != OPUS_OK) {                           \
    Reset();                                      \
    LOG(ERROR) << result << " error coed: " << h; \
    return false;                                 \
  }  // namespace
}  // namespace

namespace _LIB_NAMESPACE::encoder {

OpusEncoder::OpusEncoder(call::EncodeAudioSink* sink, uint32_t id)
    : sink_(sink), id_(id) {
  DCHECK(sink);
}

bool OpusEncoder::Initialize(uint32_t bitrate,
                             uint32_t sample_rate,
                             uint8_t channels,
                             uint8_t depth,
                             uint32_t duration) {
  if (init_done_)
    return false;
  DCHECK_GE(duration, OPUS_FRAMESIZE_10_MS);
  DCHECK(!encoder_handle_);

  int err = 0, h = 0;
  auto encoder =
      opus_encoder_create(sample_rate, channels, OPUS_APPLICATION_AUDIO, &err);

  if (err != OPUS_OK || encoder == NULL)
    return false;
  CHECK_OPUS_ENC(opus_encoder_ctl(encoder, OPUS_SET_BITRATE(depth)),
                 "Set bitrate bps faild");

  CHECK_OPUS_ENC(opus_encoder_ctl(encoder, OPUS_SET_FORCE_CHANNELS(channels)),
                 "Set channels faild");

  // CHECK_OPUS_ENC(
  //     opus_encoder_ctl(encoder, OPUS_SET_FORCE_CHANNELS(format->encode.)),
  //     "Set channels faild");

  CHECK_OPUS_ENC(opus_encoder_ctl(encoder, OPUS_SET_INBAND_FEC(1)),
                 "Enable fec faild");

  init_done_ = true;
  return true;
}

bool OpusEncoder::EncodeAudio(std::unique_ptr<base::Buffer> buffer) {
  if (!init_done_)
    return false;
  return true;
}

void OpusEncoder::Reset() {
  encoder_handle_
      ? opus_encoder_destroy(reinterpret_cast<::OpusEncoder*>(encoder_handle_))
      : void();
  encoder_handle_ = NULL;
}

void OpusEncoder::Release() {
  if (!init_done_)
    return;

  Reset();
  init_done_ = false;
  return;
}

}  // namespace _LIB_NAMESPACE::encoder