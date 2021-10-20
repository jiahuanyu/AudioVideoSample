#ifndef XPACKETDATA_H
#define XPACKETDATA_H

#include "macro.h"

struct XPacketData {
    MediaType mediaType = MEDIA_TYPE_UNKNOWN;

    unsigned char *data = nullptr;
    int size = 0;

    // 只针对视频存在，默认为0
    int width = 0;
    int height = 0;

    void Drop();
};


#endif
