package me.jiahuan.android.audiovideosample.egl

import android.util.Log
import android.view.Surface
import javax.microedition.khronos.egl.EGLContext


open class EGLThread : Thread() {


    companion object {
        private const val TAG = "EGLThread"
    }

    // 绘制的Surface
    private lateinit var surface: Surface
    // 共享的eglContext
    private var eglContext: EGLContext? = null
    // 创建
    private var firstCreate = false
    // Surface大小变化
    private var sizeChanged = false
    // 渲染器
    private var eglRenderer: EGLRenderer? = null
    // 渲染宽度
    private var surfaceWidth: Int = -1
    // 渲染高度
    private var surfaceHeight: Int = -1
    // 
    private var toExist = false

    private val eglHelper by lazy {
        EGLHelper()
    }

    override fun run() {
        eglHelper.initialize(surface, eglContext)

        while (true) {
            if (toExist) {
                release()
                break
            }

            if (firstCreate) {
                Log.d(TAG, "onSurfaceCreate")
                onSurfaceCreated()
                firstCreate = false
            }

            if (sizeChanged) {
                Log.d(
                    TAG,
                    "onSurfaceChanged surfaceWidth = $surfaceWidth, surfaceHeight = $surfaceHeight"
                )
                onSurfaceChanged()
                sizeChanged = false
            }

            onDrawFrame()
            Log.d(TAG, "onDrawFrame")
        }
    }


    private fun onSurfaceCreated() {
        eglRenderer?.onSurfaceCreated()
    }

    private fun onSurfaceChanged() {
        eglRenderer?.onSurfaceChanged(this.surfaceWidth, this.surfaceHeight)
    }

    private fun onDrawFrame() {
        eglRenderer?.onDrawFrame()
        eglHelper.swapBuffer()
    }

    private fun release() {
        eglHelper.release()
    }

    fun getEGLContext(): EGLContext? {
        return eglHelper.getEGLContext()
    }


    /**
     * 初始化
     */
    fun initialize(surface: Surface, eglContext: EGLContext?) {
        firstCreate = true
        this.surface = surface
        this.eglContext = eglContext
    }


    /**
     * 设置Surface宽度和高度
     */
    fun setSurfaceWidthAndHeight(surfaceWidth: Int, surfaceHeight: Int) {
        if (this.surfaceWidth != surfaceWidth || this.surfaceHeight != surfaceHeight) {
            this.surfaceWidth = surfaceWidth
            this.surfaceHeight = surfaceHeight
            sizeChanged = true
        }
    }

    fun setRenderer(renderer: EGLRenderer) {
        this.eglRenderer = renderer
    }


    fun getRenderer(): EGLRenderer? {
        return this.eglRenderer
    }

    /**
     * 退出
     */
    fun exist() {
        toExist = true
    }


    /**
     * 是否退出
     */
    fun isExist(): Boolean {
        return toExist
    }
}