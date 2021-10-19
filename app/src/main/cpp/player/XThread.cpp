#include "XThread.h"
#include "XLog.h"
#include <thread>

using namespace std;

void XSleep(int mis) {
    chrono::milliseconds duration(mis);
    this_thread::sleep_for(duration);
}

void XThread::Stop() {
    isExist = true;
    for (int i = 0; i < 200; i++) {
        if (!isRunning) {
            XLOGI("stop thread success");
            return;
        }
        XSleep(1);
    }
    XLOGE("stop thread timeout");
}

void XThread::Start() {
    isExist = false;
    isRunning = false;
    thread thread(&XThread::ThreadMain, this);
    thread.detach();
}

void XThread::ThreadMain() {
    isRunning = true;
    XLOGI("ThreadMain Start");
    Main();
    XLOGI("ThreadMain End");
    isRunning = false;
}
