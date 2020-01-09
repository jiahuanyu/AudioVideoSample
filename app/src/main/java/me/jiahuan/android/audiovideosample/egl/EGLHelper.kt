package me.jiahuan.android.audiovideosample.egl

import android.opengl.EGL14
import android.view.Surface
import javax.microedition.khronos.egl.*

/**
 * 自定义EGL环境
 */
class EGLHelper {
    private lateinit var egl: EGL10
    private lateinit var eglDisplay: EGLDisplay
    private lateinit var eglContext: EGLContext
    private lateinit var eglSurface: EGLSurface

    fun initialize(surface: Surface, eglContext: EGLContext?) {

        // 创建EGL
        egl = EGLContext.getEGL() as EGL10


        eglDisplay = egl.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY)

        if (eglDisplay === EGL10.EGL_NO_DISPLAY) {
            throw RuntimeException("eglGetDisplay failed")
        }


        val version = IntArray(2)
        if (!egl.eglInitialize(eglDisplay, version)) {
            throw RuntimeException("eglInitialize failed")
        }

        // 配置
        val attributes = intArrayOf(
            EGL10.EGL_RED_SIZE, 8,
            EGL10.EGL_GREEN_SIZE, 8,
            EGL10.EGL_BLUE_SIZE, 8,
            EGL10.EGL_ALPHA_SIZE, 8,
            EGL10.EGL_DEPTH_SIZE, 8,
            EGL10.EGL_STENCIL_SIZE, 8,
            EGL10.EGL_RENDERABLE_TYPE, 4,
            EGL10.EGL_NONE
        )

        val numConfig = IntArray(1)
        if (!egl.eglChooseConfig(eglDisplay, attributes, null, 0, numConfig)) {
            throw  IllegalArgumentException("eglChooseConfig failed")
        }

        val numConfigs = numConfig[0]

        if (numConfigs <= 0) {
            throw  IllegalArgumentException(
                "No configs match configSpec"
            )
        }

        val configs = arrayOfNulls<EGLConfig>(numConfigs)
        if (!egl.eglChooseConfig(
                eglDisplay, attributes, configs, numConfigs, numConfig
            )
        ) {
            throw  IllegalArgumentException("eglChooseConfig#2 failed");
        }


        // 创建EGLContext
        val attribList = intArrayOf(
            EGL14.EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE
        )
        val config = configs[0]
        this.eglContext = if (eglContext != null) {
            egl.eglCreateContext(eglDisplay, config, eglContext, attribList)
        } else {
            egl.eglCreateContext(eglDisplay, config, EGL10.EGL_NO_CONTEXT, attribList)
        }


        // 创建Surface
        this.eglSurface = egl.eglCreateWindowSurface(eglDisplay, config, surface, null)


        // 绑定Context和Surface
        if (!egl.eglMakeCurrent(eglDisplay, eglSurface, eglSurface, this.eglContext)) {
            throw RuntimeException("eglMakeCurrent fail")
        }

    }


    fun swapBuffer(): Boolean {
        return egl.eglSwapBuffers(eglDisplay, eglSurface)
    }

    fun getEGLContext(): EGLContext {
        return this.eglContext
    }

    fun release() {
        egl.eglMakeCurrent(
            eglDisplay,
            EGL10.EGL_NO_SURFACE,
            EGL10.EGL_NO_SURFACE,
            EGL10.EGL_NO_CONTEXT
        )
        egl.eglDestroySurface(eglDisplay, eglSurface)
        egl.eglDestroyContext(eglDisplay, eglContext)
        egl.eglTerminate(eglDisplay)
    }
}