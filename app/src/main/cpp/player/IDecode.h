
#ifndef IDECODE_H
#define IDECODE_H

#include "XParameter.h"
#include "IObserver.h"
#include "IObservable.h"
#include <list>
#include <mutex>
#include "macro.h"
#include "XPacketData.h"
#include "XFrameData.h"

class IDecode : public IObserver<XPacketData>, public IObservable<XFrameData>, public XThread {
public:
    // 打开解码器
    virtual bool Open(XParameter parameter) = 0;

    // 发送 Packet 数据到解码器中
    virtual bool SendPacket(XPacketData data) = 0;

    // 从解码器中读取 Frame 数据
    virtual XFrameData ReadFrame() = 0;

    virtual void Update(XPacketData data);

    MediaType mediaType = MEDIA_TYPE_UNKNOWN;

    int maxList = 100;
protected:
    // 读取缓冲
    std::list<XPacketData> packs;
    //
    std::mutex packsMutex;

    virtual void Main();
};


#endif
