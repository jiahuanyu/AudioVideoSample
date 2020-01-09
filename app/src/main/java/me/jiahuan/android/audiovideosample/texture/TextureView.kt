package me.jiahuan.android.audiovideosample.texture

import android.content.Context
import android.opengl.GLSurfaceView
import android.util.AttributeSet

/**
 * 摄像头预览View
 */
class TextureView(context: Context, attrs: AttributeSet? = null) :
    GLSurfaceView(context, attrs) {

    init {
        initialize()
    }

    /**
     * 初始化
     */
    private fun initialize() {
        // GLES20
        setEGLContextClientVersion(2)
        // 设置Render
        setRenderer(TextureRender(context))
    }
}