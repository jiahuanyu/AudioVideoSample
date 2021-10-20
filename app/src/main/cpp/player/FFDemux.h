
#ifndef FFDEMUX_H
#define FFDEMUX_H

#include "IDemux.h"

struct AVFormatContext;
struct AVPacket;

class FFDemux : public IDemux {
private:
    AVFormatContext *avFormatContext = nullptr;
    int videoStreamIndex = 0;
    int audioStreamIndex = 1;
public:
    // 打开文件
    virtual bool Open(const char *uri);

    // 获取视频流参数
    virtual XParameter GetVPara();

    // 获取音频流参数
    virtual XParameter GetAPara();

    // 读取一帧数据，数据由调用者清理
    virtual XPacketData ReadPacket();

    FFDemux();
};

#endif
