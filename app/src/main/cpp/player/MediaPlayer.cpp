#include "MediaPlayer.h"
#include <jni.h>
#include "../util/log_util.h"

void MediaPlayer::Init(char *url) {
    m_VideoRender = new VideoRender();
    m_VideoDecoder = new VideoDecoder(m_VideoRender, url);
}

void MediaPlayer::surfaceCreated(JNIEnv *env, jobject jSurface) {
    if (m_VideoRender) {
        m_VideoRender->surfaceCreated(env, jSurface);
    }
}

void MediaPlayer::Start() {
    LOGCATI("MediaPlayer::Start");
    if (m_VideoDecoder) {
        m_VideoDecoder->Start();
    }
}

void MediaPlayer::Pause() {
    LOGCATI("MediaPlayer::Pause");
    if (m_VideoDecoder) {
        m_VideoDecoder->Pause();
    }
}

void MediaPlayer::Stop() {
    LOGCATI("MediaPlayer::Stop");
    if (m_VideoDecoder) {
        m_VideoDecoder->Stop();
    }
}

void MediaPlayer::UnInit() {
    if (m_VideoDecoder) {
        delete m_VideoDecoder;
        m_VideoDecoder = nullptr;
    }

    if (m_VideoRender) {
        delete m_VideoRender;
        m_VideoRender = nullptr;
    }
}
