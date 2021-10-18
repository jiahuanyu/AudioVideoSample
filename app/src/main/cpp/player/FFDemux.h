
#ifndef FFDEMUX_H
#define FFDEMUX_H

#include "IDemux.h"

struct AVFormatContext;

class FFDemux : public IDemux {
private:
    AVFormatContext *avFormatContext = nullptr;
public:
    // 打开文件
    virtual bool Open(const char *uri);

    // 读取一帧数据，数据由调用者清理
    virtual XData Read();

    FFDemux();
};
#endif
