package me.jiahuan.android.audiovideosample.egl

import android.view.Surface
import javax.microedition.khronos.egl.EGLContext


class EGLThread : Thread() {
    companion object {
        const val RENDERMODE_WHEN_DIRTY = 0
        const val RENDERMODE_CONTINUOUSLY = 0
    }

    var surface: Surface? = null
    var eglContext: EGLContext? = null
    var isCreated = false
    var isChanged = false
    var eglRenderer: EGLRenderer? = null
    var width: Int = 0
    var height: Int = 0
    var renderMode = RENDERMODE_CONTINUOUSLY
    var isExit = false

    private val eglHelper by lazy {
        EGLHelper()
    }

    override fun run() {
        if (surface == null) {
            throw RuntimeException("Surface must not be null")
        }
        eglHelper.initialize(surface!!, eglContext)

        while (true) {
            if (isExit) {
                release()
                break
            }

            onCreate()
            onChanged()
            onDraw()
        }
    }


    private fun onCreate() {
        if (isCreated) {
            isCreated = false
            eglRenderer?.onSurfaceCreated()
        }
    }

    private fun onChanged() {
        if (isChanged) {
            isChanged = false
            eglRenderer?.onSurfaceChanged(width, height)
        }
    }

    private fun onDraw() {
        eglRenderer?.onDrawFrame()
        eglHelper.swapBuffer()
    }

    private fun release() {
        eglHelper.release()
    }

    fun getEGLContext():EGLContext? {
        return eglHelper.getEGLContext()
    }
}