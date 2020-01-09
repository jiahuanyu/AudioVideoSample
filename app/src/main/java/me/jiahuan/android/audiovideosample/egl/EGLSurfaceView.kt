package me.jiahuan.android.audiovideosample.egl

import android.content.Context
import android.util.AttributeSet
import android.view.SurfaceHolder
import android.view.SurfaceView
import javax.microedition.khronos.egl.EGLContext


open class EGLSurfaceView @JvmOverloads constructor(context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0) :
    SurfaceView(context, attrs, defStyleAttr), SurfaceHolder.Callback {

    private val eglThread by lazy {
        EGLThread()
    }

    init {
        initialize()
    }

    private fun initialize() {
        holder.addCallback(this)
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
        eglThread.isChanged = true
        eglThread.width = width
        eglThread.height = height
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        eglThread.isExit = true
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
        eglThread.surface = holder?.surface
        eglThread.isCreated = true
        eglThread.start()
    }


    fun setRenderer(elgRenderer: EGLRenderer) {
        eglThread.eglRenderer = elgRenderer
    }

    fun setRenderMode(renderMode: Int) {
        eglThread.renderMode = renderMode
    }

    fun getEGLContext(): EGLContext? {
        return eglThread.getEGLContext()
    }
}