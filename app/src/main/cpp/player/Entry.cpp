#include "Entry.h"
#include "MediaPlayer.h"
#include <jni.h>

extern "C"
JNIEXPORT jlong JNICALL
Java_me_jiahuan_android_audiovideosample_ffmpeg_MediaPlayer_nativeInit(JNIEnv *env, jobject thiz,
                                                                       jstring jUri) {
    const char *url = env->GetStringUTFChars(jUri, nullptr);
    MediaPlayer *player = new MediaPlayer();
    player->Init(env, thiz, const_cast<char *>(url));
    env->ReleaseStringUTFChars(jUri, url);
    return reinterpret_cast<jlong>(player);
}

extern "C"
JNIEXPORT void JNICALL
Java_me_jiahuan_android_audiovideosample_ffmpeg_MediaPlayer_nativeSurfaceCreated(JNIEnv *env,
                                                                                 jobject thiz,
                                                                                 jlong jPlayer_handle,
                                                                                 jobject jSurface) {
    if (jPlayer_handle != 0) {
        MediaPlayer *player = reinterpret_cast<MediaPlayer *>(jPlayer_handle);
        player->surfaceCreated(env, jSurface);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_me_jiahuan_android_audiovideosample_ffmpeg_MediaPlayer_nativeStart(JNIEnv *env, jobject thiz,
                                                                        jlong jPlayer_handle) {
    if (jPlayer_handle != 0) {
        MediaPlayer *player = reinterpret_cast<MediaPlayer *>(jPlayer_handle);
        player->Start();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_me_jiahuan_android_audiovideosample_ffmpeg_MediaPlayer_nativePause(JNIEnv *env, jobject thiz,
                                                                        jlong jPlayer_handle) {
    if (jPlayer_handle != 0) {
        MediaPlayer *player = reinterpret_cast<MediaPlayer *>(jPlayer_handle);
        player->Pause();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_me_jiahuan_android_audiovideosample_ffmpeg_MediaPlayer_nativeStop(JNIEnv *env, jobject thiz,
                                                                       jlong jPlayer_handle) {
    if (jPlayer_handle != 0) {
        MediaPlayer *player = reinterpret_cast<MediaPlayer *>(jPlayer_handle);
        player->Stop();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_me_jiahuan_android_audiovideosample_ffmpeg_MediaPlayer_nativeUnInit(JNIEnv *env, jobject thiz,
                                                                         jlong jPlayer_handle) {
    if (jPlayer_handle != 0) {
        MediaPlayer *player = reinterpret_cast<MediaPlayer *>(jPlayer_handle);
        player->UnInit();
    }
}


extern "C"
JNIEXPORT jint JNICALL
Java_me_jiahuan_android_audiovideosample_ffmpeg_MediaPlayer_nativeGetVideoWidth(JNIEnv *env,
                                                                                jobject thiz,
                                                                                jlong jPlayer_handle) {
    if (jPlayer_handle != 0) {
        MediaPlayer *player = reinterpret_cast<MediaPlayer *>(jPlayer_handle);
        return player->GetVideoWidth();
    }
    return 0;
}

extern "C"
JNIEXPORT jint JNICALL
Java_me_jiahuan_android_audiovideosample_ffmpeg_MediaPlayer_nativeGetVideoHeight(JNIEnv *env,
                                                                                 jobject thiz,
                                                                                 jlong jPlayer_handle) {
    if (jPlayer_handle != 0) {
        MediaPlayer *player = reinterpret_cast<MediaPlayer *>(jPlayer_handle);
        return player->GetVideoHeight();
    }
    return 0;
}
