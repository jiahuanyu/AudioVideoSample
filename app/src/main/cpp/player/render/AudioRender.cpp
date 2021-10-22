#include "AudioRender.h"
#include "../../util/log_util.h"

extern "C" {
#include <libswresample/swresample.h>
#include <libavutil/opt.h>
#include <libavutil/samplefmt.h>
#include <libavutil/audio_fifo.h>
#include <libavformat/avformat.h>
}


//AudioRender::AudioRender(AVCodecParameters *parameters) {
//    // - 重采样
//    swrContext = swr_alloc();
//
//    av_opt_set_int(swrContext, "in_channel_layout", value.avFrame->channel_layout, 0);
//    av_opt_set_int(swrContext, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
//
//    av_opt_set_int(swrContext, "in_sample_rate", value.avFrame->sample_rate, 0);
//    av_opt_set_int(swrContext, "out_sample_rate", 44100, 0);
//
//    av_opt_set_sample_fmt(swrContext, "in_sample_fmt", (AVSampleFormat) value.avFrame->format, 0);
//    av_opt_set_sample_fmt(swrContext, "out_sample_fmt", AV_SAMPLE_FMT_S16, 0);
//
//    swr_init(swrContext);
//}
//
//
//void AudioRender::Update(AVFrameData value) {
//
//
////    if (data.avFrame == nullptr) {
////        return {};
////    }
////
////    AudioResampledData ret;
////    int size = 2 *
////               data.avFrame->nb_samples *
////               av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
////    if (size <= 0) {
////        return {};
////    }
////    ret.Alloc(size);
////    uint8_t *outArr[2] = {nullptr};
////    int len = swr_convert(swrContext,
////                          outArr,
////                          data.avFrame->nb_samples,
////                          (const uint8_t **) data.avFrame->data,
////                          data.avFrame->nb_samples);
////    if (len <= 0) {
////        ret.Drop();
////        return {};
////    }
//}
