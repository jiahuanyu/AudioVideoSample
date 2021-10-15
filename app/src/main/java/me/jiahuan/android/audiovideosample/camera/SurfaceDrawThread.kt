package me.jiahuan.android.audiovideosample.camera

import me.jiahuan.android.audiovideosample.egl.EGLThread

/**
 * Surface绘制线程，往surface上绘制摄像头预览画面，共享eglContext
 */
class SurfaceDrawThread : EGLThread()