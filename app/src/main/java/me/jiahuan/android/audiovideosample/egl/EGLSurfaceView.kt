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

    private var eglThread = EGLThread()

    private var surfaceWidth = 0
    private var surfaceHeight = 0

    init {
        initialize()
    }

    private fun initialize() {
        holder.addCallback(this)
    }

    override fun surfaceChanged(holder: SurfaceHolder?, format: Int, width: Int, height: Int) {
        this.surfaceWidth = width
        this.surfaceHeight = height
        eglThread.setSurfaceWidthAndHeight(width, height)
    }

    override fun surfaceDestroyed(holder: SurfaceHolder?) {
        eglThread.exist()
    }

    override fun surfaceCreated(holder: SurfaceHolder?) {
        if (eglThread.isExist()) {
            val oldRenderer = eglThread.getRenderer()
            eglThread = EGLThread()
            if (oldRenderer != null) {
                setRenderer(oldRenderer)
            }
        }
        eglThread.initialize(holder?.surface!!, null)
        eglThread.start()
    }

    fun getSurfaceWidth(): Int {
        return surfaceWidth
    }

    fun getSurfaceHeight(): Int {
        return surfaceHeight
    }

    fun setRenderer(elgRenderer: EGLRenderer) {
        if (eglThread.isExist()) {
            eglThread = EGLThread()
        }
        eglThread.setRenderer(elgRenderer)
    }

    fun getEGLContext(): EGLContext? {
        return eglThread.getEGLContext()
    }
}