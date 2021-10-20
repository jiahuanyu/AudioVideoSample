
#ifndef FFDEMUX_H
#define FFDEMUX_H

#include "FFPacketData.h"
#include "XThread.h"
#include "IObservable.h"
#include "FFParameter.h"

struct AVFormatContext;
struct AVPacket;

class FFDemux : public IObservable<FFPacketData>, public XThread {
private:
    AVFormatContext *avFormatContext = nullptr;
    int videoStreamIndex = 0;
    int audioStreamIndex = 1;

    virtual void Main();

public:
    // 打开文件
    virtual bool Open(const char *uri);

    // 获取视频流参数
    virtual FFParameter GetVPara();

    // 获取音频流参数
    virtual FFParameter GetAPara();

    // 读取一帧数据，数据由调用者清理
    virtual FFPacketData ReadPacket();

    FFDemux();
};

#endif
