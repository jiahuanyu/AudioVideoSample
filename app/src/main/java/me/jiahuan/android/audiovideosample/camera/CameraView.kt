package me.jiahuan.android.audiovideosample.camera

import android.content.Context
import android.graphics.SurfaceTexture
import android.util.AttributeSet
import me.jiahuan.android.audiovideosample.egl.EGLSurfaceView

/**
 * 摄像头预览View
 */
class CameraView @JvmOverloads constructor(context: Context, attrs: AttributeSet? = null) :
    EGLSurfaceView(context, attrs) {

    private val camera = Camera()

    init {
        initialize()
    }

    /**
     * 初始化
     */
    private fun initialize() {
        // 设置Render
        val cameraRender = CameraRender(context)
        setRenderer(cameraRender)

        cameraRender.onSurfaceCreateListener = object : CameraRender.OnSurfaceCreateListener {
            override fun onSurfaceCreated(surfaceTexture: SurfaceTexture) {
                camera.initCamera(
                    surfaceTexture,
                    android.hardware.Camera.CameraInfo.CAMERA_FACING_BACK
                )
            }
        }
    }
}