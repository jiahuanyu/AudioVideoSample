#include "GLVideoView.h"
#include "XTexture.h"
extern "C" {
#include <libavformat/avformat.h>
}

void GLVideoView::SetRender(void *window) {
    view = window;
}

void GLVideoView::Render(FFFrameData data) {
    if (view == nullptr) {
        return;
    }
    if (texture == nullptr) {
        texture = XTexture::Create();
        texture->Init(view);
    }
    texture->Draw(data.avFrame->data, data.avFrame->width, data.avFrame->height);
}
