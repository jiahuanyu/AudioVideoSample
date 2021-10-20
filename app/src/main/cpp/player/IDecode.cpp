#include "IDecode.h"
#include "XLog.h"

void IDecode::Main() {
    while (!isExist) {
        packsMutex.lock();
        if (packs.empty()) {
            packsMutex.unlock();
            XSleep(1);
            continue;
        }
        // 取出
        XPacketData packetData = packs.front();
        packs.pop_front();
        if (this->SendPacket(packetData)) {
            while (!isExist) {
                XFrameData frameData = ReadFrame();
                if (frameData.data == nullptr) {
                    break;
                }
                NotifyValueChanged(frameData);
            }
        }
        packetData.Drop();
        packsMutex.unlock();
    }
}

void IDecode::Update(XPacketData data) {
    if (data.mediaType != mediaType) {
        return;
    }

    while (!isExist) {
        packsMutex.lock();
        if (packs.size() < maxList) {
            packs.push_back(data);
            packsMutex.unlock();
            break;
        }
        packsMutex.unlock();
        XSleep(1);
    }
}
