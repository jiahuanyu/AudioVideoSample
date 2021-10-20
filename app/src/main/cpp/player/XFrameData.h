#ifndef XFRAMEDATA_H
#define XFRAMEDATA_H

#include "macro.h"

struct XFrameData {
    MediaType mediaType = MEDIA_TYPE_UNKNOWN;
    unsigned char *data = nullptr;
    unsigned char *datas[8] = {nullptr};
    int size = 0;
    int width = 0;
    int height = 0;
};


#endif
