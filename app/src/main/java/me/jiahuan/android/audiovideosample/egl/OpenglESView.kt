package me.jiahuan.android.audiovideosample.egl

import android.content.Context
import android.util.AttributeSet

class OpenglESView @JvmOverloads constructor(context: Context, attrs: AttributeSet? = null, defStyleAttr: Int = 0) :
    EGLSurfaceView(context, attrs, defStyleAttr) {


    init {
        initialize()
    }

    private fun initialize() {
        setRenderer(OpenglESRenderer())
    }

}