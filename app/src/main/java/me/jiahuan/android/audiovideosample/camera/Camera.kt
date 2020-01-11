package me.jiahuan.android.audiovideosample.camera

import android.graphics.ImageFormat
import android.graphics.SurfaceTexture
import android.hardware.Camera

class Camera {

    fun initCamera(surfaceTexture: SurfaceTexture, cameraId: Int) {
        setCameraParam(surfaceTexture, cameraId)
    }

    private fun setCameraParam(surfaceTexture: SurfaceTexture, cameraId: Int) {
        val camera = Camera.open(cameraId)

        camera.setPreviewTexture(surfaceTexture)

        val parameters = camera.parameters
        parameters.previewFormat = ImageFormat.NV21
        parameters.setPictureSize(
            parameters.supportedPictureSizes[0].width,
            parameters.supportedPictureSizes[0].height
        )

        parameters.setPreviewSize(
            parameters.supportedPreviewSizes[0].width,
            parameters.supportedPreviewSizes[0].height
        )

        camera.parameters = parameters

        camera.startPreview()
    }


    fun release() {

    }
}