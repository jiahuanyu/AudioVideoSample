#ifndef IVIDEOVIEW_H
#define IVIDEOVIEW_H


#include "XData.h"
#include "IObserver.h"
#include "XFrameData.h"

class IVideoView: public IObserver<XFrameData> {
public:
    virtual void SetRender(void *window) = 0;

    virtual void Render(XFrameData data) = 0;

    virtual void Update(XFrameData data);
};


#endif
