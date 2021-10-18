#ifndef FFDECODE_H
#define FFDECODE_H

#include "XParameter.h"
#include "IDecode.h"

struct AVCodecContext;

class FFDecode : public IDecode{
public:
    virtual bool Open(XParameter parameter);

protected:
    AVCodecContext *avCodecContext = nullptr;
};


#endif
