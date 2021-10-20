#include "FFDecode.h"
#include "XLog.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

bool FFDecode::Open(FFParameter parameter) {
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
    // 多线程解码
    avCodecContext->thread_count = 8;
    // 打开解码器
    int result = avcodec_open2(avCodecContext, avCodec, nullptr);
    if (result != 0) {
        char buffer[1024];
        av_strerror(result, buffer, sizeof(buffer));
        XLOGE("avcodec_open fail");
        return false;
    }

    if (avCodecContext->codec_type == AVMEDIA_TYPE_VIDEO) {
        this->mediaType = MEDIA_TYPE_VIDEO;
    } else if (avCodecContext->codec_type == AVMEDIA_TYPE_AUDIO) {
        this->mediaType = MEDIA_TYPE_AUDIO;
    } else {
        this->mediaType = MEDIA_TYPE_UNKNOWN;
    }
    XLOGI("avcodec_open success");
    return true;
}

bool FFDecode::SendPacket(FFPacketData data) {
    if (avCodecContext == nullptr || data.avPacket == nullptr) {
        return false;
    }
    int result = avcodec_send_packet(avCodecContext, data.avPacket);
    if (result != 0) {
        return false;
    }
    return true;
}


FFFrameData FFDecode::ReadFrame() {
    if (avCodecContext == nullptr) {
        return {};
    }
    if (avFrame == nullptr) {
        avFrame = av_frame_alloc();
    }
    int result = avcodec_receive_frame(avCodecContext, avFrame);
    if (result != 0) {
        // 解码失败
        return {};
    }
    // 解码成功
    FFFrameData data;
    data.avFrame = avFrame;
    if (avCodecContext->codec_type == AVMEDIA_TYPE_VIDEO) {
//        data.size = (avFrame->linesize[0] + avFrame->linesize[1] + avFrame->linesize[2]) *
//                    avFrame->height;
    } else if (avCodecContext->codec_type == AVMEDIA_TYPE_AUDIO) {
//        data.size =
//                av_get_bytes_per_sample(
//                        (AVSampleFormat) avFrame->format) * avFrame->nb_samples *
//                avFrame->channels;
    }
//    data.datas[0] = new unsigned char[avFrame->width * avFrame->height];
//    data.datas[1] = new unsigned char[avFrame->width * avFrame->height / 4];
//    data.datas[2] = new unsigned char[avFrame->width * avFrame->height / 4];
//    memcpy(data.datas[0], avFrame->data[0], avFrame->width * avFrame->height);
//    memcpy(data.datas[1], avFrame->data[1], avFrame->width * avFrame->height / 4);
//    memcpy(data.datas[2], avFrame->data[2], avFrame->width * avFrame->height / 4);
//    memcpy(data.datas, avFrame->data, sizeof(data.datas));
    return data;
}


void FFDecode::Main() {
    while (!isExist) {
        packsMutex.lock();
        if (packs.empty()) {
            packsMutex.unlock();
            XSleep(1);
            continue;
        }
        // 取出
        FFPacketData packetData = packs.front();
        packs.pop_front();
        if (this->SendPacket(packetData)) {
            while (!isExist) {
                FFFrameData frameData = ReadFrame();
                if (frameData.avFrame == nullptr) {
                    break;
                }
                NotifyValueChanged(frameData);
            }
        }
        packetData.Drop();
        packsMutex.unlock();
    }
}

void FFDecode::Update(FFPacketData data) {
    if (data.mediaType != mediaType) {
        return;
    }

    while (!isExist) {
        packsMutex.lock();
        if (packs.size() < maxList) {
            packs.push_back(data);
            packsMutex.unlock();
            break;
        }
        packsMutex.unlock();
        XSleep(1);
    }
}

