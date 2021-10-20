
#ifndef XDATA_H
#define XDATA_H

#include "macro.h"

struct XData {
    unsigned char *data = nullptr;
    unsigned char *datas[8] = {nullptr};
    int size = 0;
    MediaType mediaType = MEDIA_TYPE_UNKNOWN;
    int width = 0;
    int height = 0;

    void Drop();
};


#endif
