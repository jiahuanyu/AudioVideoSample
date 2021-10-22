#ifndef VIDEODECODER_H
#define VIDEODECODER_H

#include "BaseDecoder.h"
#include "VideoRender.h"

class VideoDecoder : public BaseDecoder {
public:
    VideoDecoder(VideoRender *videoRender,char *url);

    virtual ~VideoDecoder();
private:

    VideoRender *m_VideoRender = nullptr;

    virtual void OnDecoderReady();

    virtual void OnDecoderDone();

    virtual void OnFrameAvailable(AVFrame *frame);
};

#endif
