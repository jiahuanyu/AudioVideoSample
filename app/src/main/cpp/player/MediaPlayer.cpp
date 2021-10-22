#include "MediaPlayer.h"
#include <jni.h>
#include "../util/log_util.h"

void MediaPlayer::Init(JNIEnv *jniEnv, jobject obj, char *url) {
    jniEnv->GetJavaVM(&m_JavaVM);
    m_JavaObj = jniEnv->NewGlobalRef(obj);

    m_VideoRender = new VideoRender();
    m_VideoDecoder = new VideoDecoder(m_VideoRender, url);

    m_VideoDecoder->SetMessageCallback(this, PostMessage);
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

    bool isAttach = false;
    GetJNIEnv(&isAttach)->DeleteGlobalRef(m_JavaObj);
    if (isAttach)
        GetJavaVM()->DetachCurrentThread();
}

int MediaPlayer::GetVideoWidth() {
    if (m_VideoDecoder) {
        return m_VideoDecoder->GetVideoWidth();
    }
    return 0;
}

int MediaPlayer::GetVideoHeight() {
    if (m_VideoDecoder) {
        return m_VideoDecoder->GetVideoHeight();
    }
    return 0;
}

jobject MediaPlayer::GetJavaObj() {
    return m_JavaObj;
}

JavaVM *MediaPlayer::GetJavaVM() {
    return m_JavaVM;
}

void MediaPlayer::PostMessage(void *context, int msgType, float msgCode) {
    if (context) {
        MediaPlayer *player = static_cast<MediaPlayer *>(context);
        bool isAttach = false;
        JNIEnv *env = player->GetJNIEnv(&isAttach);
        LOGCATE("FFMediaPlayer::PostMessage env=%p", env);
        if (env == nullptr)
            return;
        jobject javaObj = player->GetJavaObj();
        jmethodID mid = env->GetMethodID(env->GetObjectClass(javaObj),
                                         JAVA_PLAYER_EVENT_CALLBACK_API_NAME, "(IF)V");
        env->CallVoidMethod(javaObj, mid, msgType, msgCode);
        if (isAttach) {
            player->GetJavaVM()->DetachCurrentThread();
        }
    }
}

JNIEnv *MediaPlayer::GetJNIEnv(bool *isAttach) {
    JNIEnv *env;
    int status;
    if (nullptr == m_JavaVM) {
        LOGCATE("FFMediaPlayer::GetJNIEnv m_JavaVM == nullptr");
        return nullptr;
    }
    *isAttach = false;
    status = m_JavaVM->GetEnv((void **) &env, JNI_VERSION_1_4);
    if (status != JNI_OK) {
        status = m_JavaVM->AttachCurrentThread(&env, nullptr);
        if (status != JNI_OK) {
            LOGCATE("FFMediaPlayer::GetJNIEnv failed to attach current thread");
            return nullptr;
        }
        *isAttach = true;
    }
    return env;
}
