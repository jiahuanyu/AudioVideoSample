#include "GLVideoView.h"
#include "XTexture.h"

void GLVideoView::SetRender(void *window) {
    view = window;
}

void GLVideoView::Render(XFrameData data) {
    if (view == nullptr) {
        return;
    }
    if (texture == nullptr) {
        texture = XTexture::Create();
        texture->Init(view);
    }
    texture->Draw(data.datas, data.width, data.height);
}
