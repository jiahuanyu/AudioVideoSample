#ifndef FFDECODE_H
#define FFDECODE_H

#include "XParameter.h"
#include "IDecode.h"

struct AVCodecContext;
struct AVFrame;

class FFDecode : public IDecode {
public:
    virtual bool Open(XParameter parameter);

    virtual bool SendPacket(XData data);

    virtual XData RecvFrame();

protected:
    AVCodecContext *avCodecContext = nullptr;
    AVFrame *avFrame = nullptr;
};


#endif
