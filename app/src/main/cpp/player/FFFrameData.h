#ifndef XFRAMEDATA_H
#define XFRAMEDATA_H

#include "macro.h"

struct AVFrame;

struct FFFrameData {
    MediaType mediaType = MEDIA_TYPE_UNKNOWN;
    AVFrame *avFrame = nullptr;
};


#endif
