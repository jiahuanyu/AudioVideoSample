#ifndef FFDECODE_H
#define FFDECODE_H

#include "FFParameter.h"
#include "FFPacketData.h"
#include "FFFrameData.h"
#include "IObservable.h"
#include "IObserver.h"
#include "XThread.h"
#include <list>
#include <mutex>

struct AVCodecContext;
struct AVFrame;

class FFDecode : public IObserver<FFPacketData>, public IObservable<FFFrameData>, public XThread  {
public:
    virtual bool Open(FFParameter parameter);

    virtual bool SendPacket(FFPacketData data);

    virtual FFFrameData ReadFrame();

    virtual void Update(FFPacketData data);

    int maxList = 100;

protected:
    MediaType mediaType = MEDIA_TYPE_UNKNOWN;
    AVCodecContext *avCodecContext = nullptr;
    AVFrame *avFrame = nullptr;
    // 读取缓冲
    std::list<FFPacketData> packs;
    std::mutex packsMutex;
    virtual void Main();
};


#endif
