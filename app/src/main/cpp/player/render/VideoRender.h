
#ifndef FFVIDEORENDER_H
#define FFVIDEORENDER_H

#include <GLES2/gl2.h>
#include <EGL/egl.h>
#include <jni.h>
#include <mutex>
#include <ImageDef.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libavutil/time.h>
#include <libavcodec/jni.h>
}

#define TEXTURE_COUNT 3

class VideoRender {
private:
//    EGLDisplay m_EglDisplay = EGL_NO_DISPLAY;
//    EGLSurface m_EglSurface = EGL_NO_SURFACE;
//    EGLContext m_EglContext = EGL_NO_CONTEXT;

//    ANativeWindow *m_NativeWindow = nullptr;

    GLuint m_Vsh = 0;
    GLuint m_Fsh = 0;
    GLuint m_Program = 0;
    GLuint m_TextureIds[TEXTURE_COUNT] = {0};

//    bool m_HasInitEnvironment = false;

    // 初始化 EGL 环境，OPENGL 配置
//    void InitEnvironment();
    std::mutex m_Lock;

    NativeImage m_RenderImage;

public:
    VideoRender();

    virtual ~VideoRender();

    void OnSurfaceCreated();

    void OnSurfaceChanged(int width, int height);

    void OnDrawFrame();

    void RenderFrame(NativeImage *pImage);
};


#endif
