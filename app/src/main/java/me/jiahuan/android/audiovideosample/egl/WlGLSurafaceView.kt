package me.jiahuan.android.audiovideosample.egl

import android.content.Context
import android.opengl.GLSurfaceView
import android.util.AttributeSet

class WlGLSurafaceView(context: Context, attrs: AttributeSet? = null) :
    GLSurfaceView(context, attrs) {

    private var render: WlGLRender =
        WlGLRender()


    init {
        setRenderer(render)
    }

}