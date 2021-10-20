#ifndef GLVIDEOVIEW_H
#define GLVIDEOVIEW_H


#include "XData.h"
#include "IVideoView.h"

class XTexture;

class GLVideoView: public IVideoView {
public:
    virtual void SetRender(void *window);

    virtual void Render(XFrameData data);

protected:
    void *view = nullptr;
    XTexture *texture = nullptr;
};


#endif
