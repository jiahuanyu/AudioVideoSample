#ifndef GLVIDEOVIEW_H
#define GLVIDEOVIEW_H


#include "IVideoView.h"

class XTexture;

class GLVideoView: public IVideoView {
public:
    virtual void SetRender(void *window);

    virtual void Render(FFFrameData data);

protected:
    void *view = nullptr;
    XTexture *texture = nullptr;
};


#endif
