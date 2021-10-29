#include "VideoRender.h"
#include <android/native_window_jni.h>
#include <GLUtil.h>
#include "../../util/log_util.h"

#define GET_STR(x) #x

static const char *vertexShader = GET_STR(
        attribute vec4 aPosition;
        attribute vec2 aTexCoord;

        varying vec2 vTexCoord;

        void main() {
            vTexCoord = aTexCoord;
            gl_Position = aPosition;
        }
);

// 片源着色器
static const char *fragmentYUV420PShader = GET_STR(
        precision mediump float;
        uniform sampler2D yTexture;
        uniform sampler2D uTexture;
        uniform sampler2D vTexture;

        varying vec2 vTexCoord;

        void main() {
            vec3 yuv;
            vec3 rgb;
            yuv.x = texture2D(yTexture, vTexCoord).r;
            yuv.y = texture2D(uTexture, vTexCoord).r - 0.5;
            yuv.z = texture2D(vTexture, vTexCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.3945, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;
            gl_FragColor = vec4(rgb, 1.0);
        }
);

//void VideoRender::InitEnvironment() {
//    if (!m_NativeWindow) {
//        return;
//    }
//
//    m_EglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
//    if (m_EglDisplay == EGL_NO_DISPLAY) {
//        LOGCATE("eglGetDisplay fail");
//        return;
//    }
//    LOGCATI("eglGetDisplay success");
//
//    if (eglInitialize(m_EglDisplay, nullptr, nullptr) != EGL_TRUE) {
//        LOGCATE("eglInitialize fail");
//        return;
//    }
//    LOGCATI("eglInitialize success");
//
//    EGLint configSpec[] = {
//            EGL_RED_SIZE, 8,
//            EGL_GREEN_SIZE, 8,
//            EGL_BLUE_SIZE, 8,
//            EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_NONE
//    };
//    EGLConfig eglConfig; // 输出的配置项
//    EGLint configNum; // 输出的配置项
//    if (eglChooseConfig(m_EglDisplay, configSpec, &eglConfig, 1, &configNum) != EGL_TRUE) {
//        LOGCATE("eglChooseConfig fail");
//        return;
//    }
//    LOGCATI("eglChooseConfig success");
//    m_EglSurface = eglCreateWindowSurface(m_EglDisplay, eglConfig, m_NativeWindow, nullptr);
//
//    // 配置
//    const EGLint ctxAttr[] = {
//            EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE
//    };
//    m_EglContext = eglCreateContext(m_EglDisplay, eglConfig, EGL_NO_CONTEXT, ctxAttr);
//    if (m_EglContext == EGL_NO_CONTEXT) {
//        LOGCATE("eglCreateContext fail");
//        return;
//    }
//    if (eglMakeCurrent(m_EglDisplay, m_EglSurface, m_EglSurface, m_EglContext) != EGL_TRUE) {
//        LOGCATE("eglMakeCurrent fail");
//        return;
//    }
//
//    LOGCATI("EGL INIT SUCCESS");
//
//    m_Vsh = GLUtil::LoadShader(vertexShader, GL_VERTEX_SHADER);
//    if (m_Vsh == 0) {
//        LOGCATE("LoadShader GL_VERTEX_SHADER fail");
//    }
//    LOGCATI("LoadShader GL_VERTEX_SHADER success");
//
//    m_Fsh = GLUtil::LoadShader(fragmentYUV420PShader, GL_FRAGMENT_SHADER);
//    if (m_Fsh == 0) {
//        LOGCATE("LoadShader GL_FRAGMENT_SHADER fail");
//    }
//    LOGCATI("LoadShader GL_FRAGMENT_SHADER success");
//
//    m_Program = glCreateProgram();
//    if (m_Program == 0) {
//        LOGCATE("glCreateProgram fail");
//        return;
//    }
//    // 加入着色器
//    glAttachShader(m_Program, m_Vsh);
//    glAttachShader(m_Program, m_Fsh);
//
//    // 链接
//    glLinkProgram(m_Program);
//
//
//    GLint status;
//    glGetProgramiv(m_Program, GL_LINK_STATUS, &status);
//    if (status != GL_TRUE) {
//        LOGCATE("glLinkProgram fail");
//        return;
//    }
//
//    // 激活
//    glUseProgram(m_Program);
//
//    // 设置顶点坐标
//    static float vertexCoords[] = {
//            1.0f, -1.0f, 0.0f,
//            -1.0f, -1.0f, 0.0f,
//            1.0f, 1.0f, 0.0f,
//            -1.0f, 1.0f, 0.0f
//    };
//    GLint aPositionLocation = glGetAttribLocation(m_Program, "aPosition");
//    glEnableVertexAttribArray(aPositionLocation);
//    glVertexAttribPointer(aPositionLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
//                          vertexCoords);
//
//    // 设置纹理坐标
//    static float textureCoords[] = {
//            1.0f, 1.0f,
//            0.0f, 1.0f,
//            1.0f, 0.0f,
//            0.0f, 0.0f,
//    };
//    GLint aTextCoordLocation = glGetAttribLocation(m_Program, "aTexCoord");
//    glEnableVertexAttribArray(aTextCoordLocation);
//    glVertexAttribPointer(aTextCoordLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
//                          textureCoords);
//
//    // 设置层
//    glUniform1i(glGetUniformLocation(m_Program, "yTexture"), 0);
//    glUniform1i(glGetUniformLocation(m_Program, "uTexture"), 1);
//    glUniform1i(glGetUniformLocation(m_Program, "vTexture"), 2);
//
//    LOGCATI("init shader success");
//
//    glGenTextures(TEXTURE_COUNT, m_TextureIds);
//    for (int i = 0; i < TEXTURE_COUNT; i++) {
//        glActiveTexture(GL_TEXTURE0 + i);
//        glBindTexture(GL_TEXTURE_2D, m_TextureIds[i]);
//        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glBindTexture(GL_TEXTURE_2D, GL_NONE);
//    }
//    LOGCATI("create texture success");
//}

void VideoRender::RenderFrame(NativeImage *pImage) {
    LOGCATI("VideoRender::RenderFrame pImage=%p", pImage);
    if (pImage == nullptr || pImage->ppPlane[0] == nullptr)
        return;
    m_Lock.lock();
    if (pImage->width != m_RenderImage.width || pImage->height != m_RenderImage.height) {
        if (m_RenderImage.ppPlane[0] != nullptr) {
            NativeImageUtil::FreeNativeImage(&m_RenderImage);
        }
        memset(&m_RenderImage, 0, sizeof(NativeImage));
        m_RenderImage.format = pImage->format;
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        NativeImageUtil::AllocNativeImage(&m_RenderImage);
    }

    NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    m_Lock.unlock();
}

void VideoRender::OnSurfaceCreated() {
    m_Vsh = GLUtil::LoadShader(vertexShader, GL_VERTEX_SHADER);
    if (m_Vsh == 0) {
        LOGCATE("LoadShader GL_VERTEX_SHADER fail");
    }
    LOGCATI("LoadShader GL_VERTEX_SHADER success");

    m_Fsh = GLUtil::LoadShader(fragmentYUV420PShader, GL_FRAGMENT_SHADER);
    if (m_Fsh == 0) {
        LOGCATE("LoadShader GL_FRAGMENT_SHADER fail");
    }
    LOGCATI("LoadShader GL_FRAGMENT_SHADER success");

    m_Program = glCreateProgram();
    if (m_Program == 0) {
        LOGCATE("glCreateProgram fail");
        return;
    }
    // 加入着色器
    glAttachShader(m_Program, m_Vsh);
    glAttachShader(m_Program, m_Fsh);

    // 链接
    glLinkProgram(m_Program);


    GLint status;
    glGetProgramiv(m_Program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        LOGCATE("glLinkProgram fail");
        return;
    }

    // 激活
    glUseProgram(m_Program);

    // 设置顶点坐标
    static float vertexCoords[] = {
            1.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f
    };
    GLint aPositionLocation = glGetAttribLocation(m_Program, "aPosition");
    glEnableVertexAttribArray(aPositionLocation);
    glVertexAttribPointer(aPositionLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          vertexCoords);

    // 设置纹理坐标
    static float textureCoords[] = {
            1.0f, 1.0f,
            0.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
    };
    GLint aTextCoordLocation = glGetAttribLocation(m_Program, "aTexCoord");
    glEnableVertexAttribArray(aTextCoordLocation);
    glVertexAttribPointer(aTextCoordLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                          textureCoords);

    // 设置层
    glUniform1i(glGetUniformLocation(m_Program, "yTexture"), 0);
    glUniform1i(glGetUniformLocation(m_Program, "uTexture"), 1);
    glUniform1i(glGetUniformLocation(m_Program, "vTexture"), 2);

    LOGCATI("init shader success");

    glGenTextures(TEXTURE_COUNT, m_TextureIds);
    for (int i = 0; i < TEXTURE_COUNT; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_TextureIds[i]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }
    LOGCATI("create texture success");
}

void VideoRender::OnSurfaceChanged(int width, int height) {
    glViewport(0, 0, width, height);
}

void VideoRender::OnDrawFrame() {
    m_Lock.lock();

    int width = m_RenderImage.width;
    int height = m_RenderImage.height;

    LOGCATI("width = %d, height = %d", width, height);

    //upload Y plane data
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureIds[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width,
                 height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    //update U plane data
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_TextureIds[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width >> 1,
                 height >> 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 m_RenderImage.ppPlane[1]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    //update V plane data
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_TextureIds[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, width >> 1,
                 height >> 1, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                 m_RenderImage.ppPlane[2]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    m_Lock.unlock();

    for (int i = 0; i < TEXTURE_COUNT; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_TextureIds[i]);
    }

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

VideoRender::~VideoRender() {
//    if (m_EglDisplay && m_EglSurface) {
//        eglDestroySurface(m_EglDisplay, m_EglSurface);
//    }
//
//    if (m_EglDisplay && m_EglContext) {
//        eglDestroyContext(m_EglDisplay, m_EglContext);
//    }
//
//    if (m_EglDisplay) {
//        eglTerminate(m_EglDisplay);
//    }

//    if (m_NativeWindow) {
//        ANativeWindow_release(m_NativeWindow);
//        m_NativeWindow = nullptr;
//    }

    glDetachShader(m_Program, m_Vsh);
    glDeleteShader(m_Vsh);

    glDetachShader(m_Program, m_Fsh);
    glDeleteShader(m_Fsh);

    glDeleteProgram(m_Program);

    glDeleteTextures(TEXTURE_COUNT, m_TextureIds);
}

VideoRender::VideoRender() = default;
