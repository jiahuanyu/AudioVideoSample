package me.jiahuan.android.audiovideosample.egl

import android.content.Context
import android.util.AttributeSet
import android.view.SurfaceHolder
import android.view.SurfaceView
import javax.microedition.khronos.egl.EGLContext


open class EGLSurfaceView @JvmOverloads constructor(
    context: Context,
    attrs: AttributeSet? = null,
    defStyleAttr: Int = 0
) :
    SurfaceView(context, attrs, defStyleAttr), SurfaceHolder.Callback {

    private val eglThread by lazy {
        EGLThread()
    }

    var surfaceWidth: Int = 0
    var surfaceHeight: Int = 0

    init {
        initialize()
    }

    private fun initialize() {
        holder.addCallback(this)
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
        this.surfaceWidth = width
        this.surfaceHeight = height
        eglThread.isChanged = true
        eglThread.width = width
        eglThread.height = height
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        eglThread.isExit = true
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
        if (holder != null) {
            eglThread.surface = holder.surface
        }
        eglThread.isCreated = true
        eglThread.start()
    }


    fun setRenderer(elgRenderer: EGLRenderer) {
        eglThread.eglRenderer = elgRenderer
    }

    fun getEGLContext(): EGLContext? {
        return eglThread.getEGLContext()
    }
}