
#ifndef IDECODE_H
#define IDECODE_H

#include "XParameter.h"
#include "IObserver.h"
#include <list>
#include "macro.h"

class IDecode : public IObserver {
public:
    // 打开解码器
    virtual bool Open(XParameter parameter) = 0;

    //
    virtual bool SendPacket(XData data) = 0;

    virtual XData RecvFrame() = 0;

    virtual void Update(XData data);

    MediaType mediaType = MEDIA_TYPE_UNKNOWN;

    int maxList = 100;
protected:
    // 读取缓冲
    std::list<XData> packs;

    std::mutex packsMutex;

    virtual void Main();
};


#endif
