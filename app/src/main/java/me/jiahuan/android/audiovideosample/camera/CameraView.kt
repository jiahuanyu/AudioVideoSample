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

    private var hasSurfaceCreated = false

    var textureId: Int = 0

    private lateinit var surfaceTexture: SurfaceTexture

    init {
        initialize()
    }

    /**
     * 初始化
     */
    private fun initialize() {
        // 设置Render
        val cameraDBORenderer = CameraFBORenderer(context)
        setRenderer(cameraDBORenderer)

        cameraDBORenderer.onSurfaceCreateListener =
            object : CameraFBORenderer.OnSurfaceCreateListener {
                override fun onSurfaceCreated(surfaceTexture: SurfaceTexture, textureId: Int) {
                    hasSurfaceCreated = true
                    this@CameraView.surfaceTexture = surfaceTexture
                    this@CameraView.textureId = textureId
                    onResume()
                }
            }
    }

    fun onResume() {
        if (hasSurfaceCreated) {
            camera.starPreview(
                surfaceTexture,
                android.hardware.Camera.CameraInfo.CAMERA_FACING_BACK
            )
        }
    }

    fun onPause() {
        camera.stopPreview()
    }
}