
#ifndef IDECODE_H
#define IDECODE_H

#include "XParameter.h"
#include "IObserver.h"

class IDecode : IObserver {
public:
    // 打开解码器
    virtual bool Open(XParameter parameter) = 0;
};


#endif
