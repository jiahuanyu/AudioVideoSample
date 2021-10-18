package me.jiahuan.android.audiovideosample.opengl

import android.opengl.GLES20
import me.jiahuan.android.audiovideosample.egl.EGLRenderer

class OpenglESRenderer : EGLRenderer {
    override fun onSurfaceCreated() {

    }

    override fun onSurfaceChanged(width: Int, height: Int) {
        GLES20.glViewport(0, 0, width, height)
    }

    override fun onDrawFrame() {
        GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT)
        GLES20.glClearColor(1f, 1f, 0f, 1f)
    }
}