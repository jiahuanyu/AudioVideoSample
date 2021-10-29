#include "VideoDecoder.h"
#include "../../util/log_util.h"

VideoDecoder::VideoDecoder(VideoRender *videoRender, char *url) {
    m_VideoRender = videoRender;
    Init(url, AVMEDIA_TYPE_VIDEO);
}

VideoDecoder::~VideoDecoder() {
    UnInit();
}

void VideoDecoder::OnDecoderReady() {
    m_VideoWidth = GetCodecContext()->width;
    m_VideoHeight = GetCodecContext()->height;
    if (m_MsgContext && m_MsgCallback) {
        m_MsgCallback(m_MsgContext, MSG_DECODER_READY, 0);
    }
}

void VideoDecoder::OnDecoderDone() {

}

void VideoDecoder::OnFrameAvailable(AVFrame *frame) {
    if(m_VideoRender != nullptr && frame != nullptr) {
        NativeImage image;
        image.format = IMAGE_FORMAT_I420;
        image.width = frame->width;
        image.height = frame->height;
        image.pLineSize[0] = frame->linesize[0];
        image.pLineSize[1] = frame->linesize[1];
        image.pLineSize[2] = frame->linesize[2];
        image.ppPlane[0] = frame->data[0];
        image.ppPlane[1] = frame->data[1];
        image.ppPlane[2] = frame->data[2];
        if(frame->data[0] && frame->data[1] && !frame->data[2] && frame->linesize[0] == frame->linesize[1] && frame->linesize[2] == 0) {
            // on some android device, output of h264 mediacodec decoder is NV12 兼容某些设备可能出现的格式不匹配问题
            image.format = IMAGE_FORMAT_NV12;
        }
        m_VideoRender->RenderFrame(&image);
    }
    if (m_MsgContext && m_MsgCallback) {
        m_MsgCallback(m_MsgContext, MSG_REQUEST_RENDER, 0);
    }
}
