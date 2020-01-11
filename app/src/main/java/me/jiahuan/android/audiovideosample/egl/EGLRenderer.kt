package me.jiahuan.android.audiovideosample.egl


interface EGLRenderer {
    fun onSurfaceCreated()
    fun onSurfaceChanged(width: Int, height: Int)
    fun onDrawFrame()
}