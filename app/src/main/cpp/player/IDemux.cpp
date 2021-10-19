#include "IDemux.h"
#include "XLog.h"

void IDemux::Main() {
    while (!isExist) {
        XData data = Read();
        if (data.size > 0) {
            Notify(data);
        }
    }
}
