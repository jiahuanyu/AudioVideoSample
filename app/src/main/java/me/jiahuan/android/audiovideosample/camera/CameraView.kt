package me.jiahuan.android.audiovideosample.camera

import android.content.Context
import android.util.AttributeSet
import me.jiahuan.android.audiovideosample.egl.EGLSurfaceView

/**
 * 摄像头预览View
 */
class CameraView @JvmOverloads constructor(context: Context, attrs: AttributeSet? = null) :
    EGLSurfaceView(context, attrs) {

    private val camera = Camera()
    private val cameraFBORenderer = CameraFBORenderer(context)


    init {
        initialize()
    }

    /**
     * 初始化
     */
    private fun initialize() {
        // 设置Render
        setRenderer(cameraFBORenderer)
    }

    fun getTextureId(): Int {
        return cameraFBORenderer.getTextureId()
    }

    fun onResume() {
        post {
            camera.starPreview(
                cameraFBORenderer.getSurfaceTexture(),
                android.hardware.Camera.CameraInfo.CAMERA_FACING_BACK
            )
        }
    }

    fun onPause() {
        post {
            camera.stopPreview()
        }
    }
}