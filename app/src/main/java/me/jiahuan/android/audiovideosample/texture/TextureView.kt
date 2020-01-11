package me.jiahuan.android.audiovideosample.texture

import android.content.Context
import android.util.AttributeSet
import me.jiahuan.android.audiovideosample.egl.EGLSurfaceView

/**
 * 摄像头预览View
 */
class TextureView(context: Context, attrs: AttributeSet? = null) :
    EGLSurfaceView(context, attrs) {

    init {
        initialize()
    }

    /**
     * 初始化
     */
    private fun initialize() {
        // 设置Render
        setRenderer(TextureFBORenderer(context))
    }
}