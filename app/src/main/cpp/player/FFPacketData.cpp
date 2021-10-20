#include "FFPacketData.h"

extern "C" {
#include <libavformat/avformat.h>
}

void FFPacketData::Drop() {
    if (avPacket == nullptr) {
        return;
    }
    av_packet_free(&avPacket);
    avPacket = nullptr;
}
