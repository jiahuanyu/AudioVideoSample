#ifndef IDEMUX_H
#define IDEMUX_H

#include "XData.h"
#include "IObserver.h"

class IDemux: public IObserver {
public:
    // 打开文件
    virtual bool Open(const char *uri) = 0;

    // 读取一帧数据，数据由调用者清理
    virtual XData Read() = 0;

    // 总时长（毫秒）
    long totalMs = 0;

protected:
    virtual void Main();
};


#endif
