#include "XPacketData.h"

void XPacketData::Drop() {
    if (data == nullptr) {
        return;
    }
//    av_packet_free(reinterpret_cast<AVPacket **>(&data));
    data = nullptr;
    size = 0;
}
