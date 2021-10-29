#ifndef VIDEODECODER_H
#define VIDEODECODER_H

#include "BaseDecoder.h"
#include "VideoRender.h"

class VideoDecoder : public BaseDecoder {
public:
    VideoDecoder(VideoRender *videoRender, char *url);

    virtual ~VideoDecoder();

private:
    // 渲染视频的宽度
    int m_VideoWidth = 0;
    // 渲染视频的高度
    int m_VideoHeight = 0;

    VideoRender *m_VideoRender = nullptr;

    virtual void OnDecoderReady();

    virtual void OnDecoderDone();

    virtual void OnFrameAvailable(AVFrame *frame);
};

#endif
