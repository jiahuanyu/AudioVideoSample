#include "XEGL.h"
#include <android/native_window_jni.h>
#include <EGL/egl.h>
#include "XLog.h"

class CXEGL : public XEGL {
public:
    EGLDisplay eglDisplay = EGL_NO_DISPLAY;
    EGLSurface eglSurface = EGL_NO_SURFACE;
    EGLContext eglContext = EGL_NO_CONTEXT;

    bool Init(void *window) override {
        auto *aNativeWindow = (ANativeWindow *) window;
        eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        if (eglDisplay == EGL_NO_DISPLAY) {
            XLOGE("eglGetDisplay fail");
            return false;
        }
        XLOGI("eglGetDisplay success");

        if (eglInitialize(eglDisplay, nullptr, nullptr) != EGL_TRUE) {
            XLOGE("eglInitialize fail");
            return false;
        }
        XLOGI("eglInitialize success");

        EGLint configSpec[] = {
                EGL_RED_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_BLUE_SIZE, 8,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_NONE
        };
        EGLConfig eglConfig; // 输出的配置项
        EGLint configNum; // 输出的配置项
        if (eglChooseConfig(eglDisplay, configSpec, &eglConfig, 1, &configNum) != EGL_TRUE) {
            XLOGE("eglChooseConfig fail");
            return false;
        }
        XLOGI("eglChooseConfig success");
        eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, aNativeWindow, nullptr);

        // 配置
        const EGLint ctxAttr[] = {
                EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE
        };
        eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, ctxAttr);
        if (eglContext == EGL_NO_CONTEXT) {
            XLOGE("eglCreateContext fail");
            return false;
        }
        if (eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext) != EGL_TRUE) {
            XLOGE("eglMakeCurrent fail");
            return false;
        }
        XLOGI("EGL INIT SUCCESS");
        return true;
    }

    void Draw() override {
        if (eglDisplay == EGL_NO_DISPLAY || eglSurface == EGL_NO_SURFACE) {
            return;
        }
        eglSwapBuffers(eglDisplay, eglSurface);
    }
};

XEGL *XEGL::Get() {
    static CXEGL egl;
    return &egl;
}
