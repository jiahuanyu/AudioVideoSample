#ifndef XPACKETDATA_H
#define XPACKETDATA_H

#include "macro.h"

struct AVPacket;

struct FFPacketData {
    MediaType mediaType = MEDIA_TYPE_UNKNOWN;
    AVPacket *avPacket = nullptr;

    void Drop();
};


#endif
