#include "IVideoView.h"

void IVideoView::Update(XFrameData data) {
    this->Render(data);
}
