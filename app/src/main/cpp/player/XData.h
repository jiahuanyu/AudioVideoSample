
#ifndef XDATA_H
#define XDATA_H

#include "macro.h"

struct XData {
    unsigned char *data = nullptr;
    int size = 0;
    MediaType mediaType = MEDIA_TYPE_UNKNOWN;

    void Drop();
};


#endif
