package me.jiahuan.android.audiovideosample.egl

import android.view.Surface
import javax.microedition.khronos.egl.EGLContext


open class EGLThread : Thread() {
    // 绘制的Surface
    lateinit var surface: Surface
    // 共享的eglContext
    var eglContext: EGLContext? = null
    //
    var isCreated = false
    //
    var isChanged = false
    // 渲染器
    var eglRenderer: EGLRenderer? = null
    // 渲染宽度
    var width: Int = 0
    // 渲染高度
    var height: Int = 0
    // 
    var isExit = false

    private val eglHelper by lazy {
        EGLHelper()
    }

    override fun run() {
        eglHelper.initialize(surface, eglContext)

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

    fun getEGLContext(): EGLContext? {
        return eglHelper.getEGLContext()
    }
}