#include "FFDecode.h"
#include "XLog.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

bool FFDecode::Open(XParameter parameter) {
    if (parameter.parameters == nullptr) {
        return false;
    }
    AVCodecParameters *p = parameter.parameters;
    const AVCodec *avCodec = avcodec_find_decoder(p->codec_id);
    if (avCodec == nullptr) {
        XLOGE("avcodec_find_decoder fail %d", p->codec_id);
        return false;
    }
    XLOGI("avcodec_find_decoder success");
    avCodecContext = avcodec_alloc_context3(avCodec);
    avcodec_parameters_to_context(avCodecContext, p);

    int result = avcodec_open2(avCodecContext, avCodec, nullptr);
    if (result != 0) {
        char buffer[1024];
        av_strerror(result, buffer, sizeof(buffer));
        XLOGE("avcodec_open fail");
        return false;
    }
    XLOGI("avcodec_open success");
    return true;
}
