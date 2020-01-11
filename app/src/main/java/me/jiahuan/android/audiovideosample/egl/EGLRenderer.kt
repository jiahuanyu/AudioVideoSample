package me.jiahuan.android.audiovideosample.egl


interface EGLRenderer {
    fun onSurfaceCreated(width: Int, height: Int)
    fun onSurfaceChanged(width: Int, height: Int)
    fun onDrawFrame()
}