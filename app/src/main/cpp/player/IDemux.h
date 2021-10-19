#ifndef IDEMUX_H
#define IDEMUX_H

#include "XData.h"
#include "IObserver.h"
#include "XParameter.h"

class IDemux: public IObserver {
public:
    // 打开文件
    virtual bool Open(const char *uri) = 0;

    // 解封获取视频流参数
    virtual XParameter GetVPara() = 0;

    // 解封获取音频流参数
    virtual XParameter GetAPara() = 0;

    // 读取一帧数据，数据由调用者清理
    virtual XData Read() = 0;

    // 封装总时长（毫秒）
    long totalMs = 0;

protected:
    // 解封
    virtual void Main();
};


#endif