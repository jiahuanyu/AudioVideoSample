#include "IVideoView.h"

void IVideoView::Update(FFFrameData data) {
    this->Render(data);
}
