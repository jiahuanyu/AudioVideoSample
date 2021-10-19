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
        XData packData = packs.front();
        packs.pop_front();
        if (this->SendPacket(packData)) {
            while (!isExist) {
                XData frameData = RecvFrame();
                if (frameData.data == nullptr) {
                    break;
                }
                XLOGI("RecvFrame %d", frameData.size);
                this->Notify(frameData);
            }
        }
        packData.Drop();
        packsMutex.unlock();
    }
}

void IDecode::Update(XData data) {
    if (data.isAudio != isAudio) {
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
