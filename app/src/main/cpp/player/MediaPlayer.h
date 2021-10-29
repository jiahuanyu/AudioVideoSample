
#ifndef FFMEDIAPLAYER_H
#define FFMEDIAPLAYER_H

#include <jni.h>
#include <VideoDecoder.h>

#define JAVA_PLAYER_EVENT_CALLBACK_API_NAME "playerEventCallback"

class MediaPlayer {
private:
    VideoDecoder *m_VideoDecoder = nullptr;

    VideoRender *m_VideoRender = nullptr;

    JNIEnv *GetJNIEnv(bool *isAttach);

    jobject GetJavaObj();

    JavaVM *GetJavaVM();

    static void PostMessage(void *context, int msgType, float msgCode);

    JavaVM *m_JavaVM = nullptr;
    jobject m_JavaObj = nullptr;

public:
    MediaPlayer() = default;

    ~MediaPlayer() = default;

    void Init(JNIEnv *jniEnv, jobject obj, char *url);

    void Start();

    void Pause();

    void Stop();

    void OnSurfaceCreated();

    void OnSurfaceChanged(int width, int height);

    void OnDrawFrame();

    void UnInit();
};


#endif
