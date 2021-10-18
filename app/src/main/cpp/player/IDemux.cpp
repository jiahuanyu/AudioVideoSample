#include "IDemux.h"
#include "XLog.h"

void IDemux::Main() {
    while (!isExist) {
        XData data = Read();
//        XLOGI("IDemux Read %d", data.size);
        if (data.size > 0) {
            Notify(data);
        } else {
            break;
        }
    }
}
