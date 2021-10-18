#ifndef XLOG_H
#define XLOG_H

#include <android/log.h>

class XLog {

};

#define LOG_TAG "AudioVideoSample"

#define XLOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define XLOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define XLOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#endif
