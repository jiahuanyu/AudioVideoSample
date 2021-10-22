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

    m_MsgCallback(m_MsgContext, MSG_DECODER_READY, 0);
}

void VideoDecoder::OnDecoderDone() {

}

void VideoDecoder::OnFrameAvailable(AVFrame *frame) {
    m_VideoRender->RenderFrame(frame);
}

int VideoDecoder::GetVideoWidth() const {
    return m_VideoWidth;
}

int VideoDecoder::GetVideoHeight() const {
    return m_VideoHeight;
}
