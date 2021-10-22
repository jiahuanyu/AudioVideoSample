
#ifndef FFMEDIAPLAYER_H
#define FFMEDIAPLAYER_H

#include <jni.h>
#include <VideoDecoder.h>

class MediaPlayer {
private:
    VideoDecoder *m_VideoDecoder = nullptr;

    VideoRender *m_VideoRender = nullptr;
public:
    MediaPlayer() {};

    ~MediaPlayer() {};

    void Init(char *url);

    void Start();

    void Pause();

    void Stop();

    void surfaceCreated(JNIEnv *env, jobject jSurface);

    void UnInit();
};


#endif
