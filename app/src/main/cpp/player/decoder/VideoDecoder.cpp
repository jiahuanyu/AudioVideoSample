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

}

void VideoDecoder::OnDecoderDone() {

}

void VideoDecoder::OnFrameAvailable(AVFrame *frame) {
    m_VideoRender->RenderFrame(frame);
}
