#include "FFResample.h"

extern "C" {
#include <libswresample/swresample.h>
#include <libavcodec/avcodec.h>
}

#include "XLog.h"


void FFResample::Update(FFPacketData data) {
    FFAudioData audioData = Resample(data);
    NotifyValueChanged(audioData);
}

bool FFResample::Open(FFParameter in, FFParameter out) {
    swrContext = swr_alloc();
    swrContext = swr_alloc_set_opts(swrContext,
                                    av_get_default_channel_layout(2),
                                    AV_SAMPLE_FMT_S16,
                                    in.parameters->sample_rate,
                                    av_get_default_channel_layout(in.parameters->channels),
                                    static_cast<AVSampleFormat>(in.parameters->format),
                                    in.parameters->sample_rate,
                                    0,
                                    nullptr

    );


    int result = swr_init(swrContext);
    if (result != 0) {
        XLOGE("swr_init fail");
        return false;
    }
    XLOGI("swr_init success");
    return true;
}

FFAudioData FFResample::Resample(FFFrameData data) {
    if (data.avFrame == nullptr) {
        return {};
    }

    FFAudioData ret;
    int size = 2 *
               data.avFrame->nb_samples *
               av_get_bytes_per_sample(AV_SAMPLE_FMT_S16);
    if (size <= 0) {
        return {};
    }
    ret.Alloc(size);
    uint8_t *outArr[2] = {nullptr};
    int len = swr_convert(swrContext,
                          outArr,
                          data.avFrame->nb_samples,
                          (const uint8_t **) data.avFrame->data,
                          data.avFrame->nb_samples);
    if (len <= 0) {
        ret.Drop();
        return {};
    }
    return ret;
}
