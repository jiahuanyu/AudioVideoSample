package me.jiahuan.android.audiovideosample.camera

import android.content.Context
import android.util.AttributeSet
import me.jiahuan.android.audiovideosample.egl.EGLSurfaceView

/**
 * 摄像头预览View
 */
class CameraView @JvmOverloads constructor(context: Context, attrs: AttributeSet? = null) :
    EGLSurfaceView(context, attrs), CameraFBORenderer.Callback {

    private val camera = Camera()

    private val cameraFBORenderer by lazy {
        CameraFBORenderer(context)
    }

    init {
        initialize()
    }

    private var hasResume = false

    private var hasSurfaceTextureCreated = false

    /**
     * 初始化
     */
    private fun initialize() {
        cameraFBORenderer.setCallback(this)
        setRenderer(cameraFBORenderer)
    }

    fun getTextureId(): Int {
        return cameraFBORenderer.getTextureId()
    }

    fun onResume() {
        hasResume = true
        tryToStartPreview()
    }

    fun onPause() {
        hasResume = false
        hasSurfaceTextureCreated = false
        camera.stopPreview()
    }


    private fun tryToStartPreview() {
        if (!hasResume || !hasSurfaceTextureCreated) {
            return
        }
        val surfaceTexture = cameraFBORenderer.getSurfaceTexture()
        if (surfaceTexture != null) {
            camera.starPreview(surfaceTexture)
        }
    }

    override fun onSurfaceTextureCreated() {
        hasSurfaceTextureCreated = true
        tryToStartPreview()
    }
}