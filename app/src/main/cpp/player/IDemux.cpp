#include "IDemux.h"
#include "XLog.h"

void IDemux::Main() {
    while (!isExist) {
        XPacketData data = ReadPacket();
        if (data.size > 0) {
            NotifyValueChanged(data);
        }
    }
}
