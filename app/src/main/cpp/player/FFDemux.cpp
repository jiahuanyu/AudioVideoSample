#include "FFDemux.h"
#include "XLog.h"

extern "C" {
#include <libavformat/avformat.h>
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
//    this->totalMs = avFormatContext->duration / (AV_TIME_BASE / 1000);
//    XLOGI("total ms = %ld", this->totalMs);
    return true;
}

FFPacketData FFDemux::ReadPacket() {
    if (avFormatContext == nullptr) {
        return {};
    }
    AVPacket *avPacket = av_packet_alloc();
    FFPacketData packetData;
    int result = av_read_frame(avFormatContext, avPacket);
    if (result != 0) {
        av_packet_free(&avPacket);
        return {};
    }
    packetData.avPacket = avPacket;
    if (avPacket->stream_index == audioStreamIndex) {
        packetData.mediaType = MEDIA_TYPE_AUDIO;
    } else if (avPacket->stream_index == videoStreamIndex) {
        packetData.mediaType = MEDIA_TYPE_VIDEO;
    } else {
        packetData.mediaType = MEDIA_TYPE_UNKNOWN;
    }
    return packetData;
}

FFParameter FFDemux::GetVPara() {
    if (avFormatContext == nullptr) {
        XLOGE("avFormatContext is nullptr");
        return {};
    }
    // 获取视频流的索引
    this->videoStreamIndex = av_find_best_stream(avFormatContext,
                                                 AVMEDIA_TYPE_VIDEO,
                                                 -1,
                                                 -1,
                                                 nullptr,
                                                 0);
    if (videoStreamIndex < 0) {
        XLOGE("find video stream fail");
        return {};
    }
    FFParameter xParameter;
    xParameter.parameters = avFormatContext->streams[videoStreamIndex]->codecpar;
    return xParameter;
}

FFParameter FFDemux::GetAPara() {
    if (avFormatContext == nullptr) {
        XLOGE("avFormatContext is nullptr");
        return {};
    }
    // 获取视频流的索引
    this->audioStreamIndex = av_find_best_stream(avFormatContext,
                                                 AVMEDIA_TYPE_AUDIO,
                                                 -1,
                                                 -1,
                                                 nullptr,
                                                 0);
    if (audioStreamIndex < 0) {
        XLOGE("find video stream fail");
        return {};
    }
    FFParameter xParameter;
    xParameter.parameters = avFormatContext->streams[audioStreamIndex]->codecpar;
    return xParameter;
}

void FFDemux::Main() {
    while (!isExist) {
        FFPacketData data = ReadPacket();
        if (data.avPacket != nullptr) {
            NotifyValueChanged(data);
        }
    }
}
