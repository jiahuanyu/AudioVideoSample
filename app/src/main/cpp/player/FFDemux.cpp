#include "FFDemux.h"
#include "XLog.h"

extern "C" {
#include <libavformat/avformat.h>
}

bool FFDemux::Open(const char *uri) {
    XLOGI("FFDemux Open uri %s begin", uri);
    int result = avformat_open_input(&avFormatContext, uri, nullptr, nullptr);
    if (result != 0) {
        char buffer[1024];
        av_strerror(result, buffer, sizeof(buffer));
        XLOGE("FFDemux avformat_open_input %s fail, with error %s", uri, buffer);
        return false;
    }
    XLOGI("FFDemux open %s success.", uri);

    // 读取文件信息
    result = avformat_find_stream_info(avFormatContext, nullptr);
    if (result != 0) {
        char buffer[1024];
        av_strerror(result, buffer, sizeof(buffer));
        XLOGE("FFDemux avformat_find_stream_info %s fail, with error %s", uri, buffer);
        return false;
    }
    this->totalMs = avFormatContext->duration / (AV_TIME_BASE / 1000);
    XLOGI("total ms = %ld", this->totalMs);
    return true;
}

XData FFDemux::Read() {
    if(avFormatContext == nullptr) {
        return {};
    }
    XData d;
    AVPacket *avPacket = av_packet_alloc();
    int result = av_read_frame(avFormatContext, avPacket);
    if(result != 0) {
        // fail
        av_packet_free(&avPacket);
        return {};
    }
    d.data = reinterpret_cast<unsigned char *>(avPacket);
    d.size = avPacket->size;
    return d;
}

FFDemux::FFDemux() {
    static bool isFirst = true;
    if (isFirst) {
        isFirst = false;
        // 初始化网络
        avformat_network_init();
        XLOGI("register ffmpeg");
    }
}
