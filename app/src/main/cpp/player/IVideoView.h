#ifndef IVIDEOVIEW_H
#define IVIDEOVIEW_H


#include "IObserver.h"
#include "FFFrameData.h"

class IVideoView: public IObserver<FFFrameData> {
public:
    virtual void SetRender(void *window) = 0;

    virtual void Render(FFFrameData data) = 0;

    virtual void Update(FFFrameData data);
};


#endif
