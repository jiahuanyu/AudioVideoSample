package me.jiahuan.android.audiovideosample.camera

import android.graphics.ImageFormat
import android.graphics.SurfaceTexture
import android.hardware.Camera


class Camera {

    private var currentCamera: Camera? = null

    fun starPreview(surfaceTexture: SurfaceTexture) {
        currentCamera = Camera.open(Camera.CameraInfo.CAMERA_FACING_BACK)

        currentCamera?.let { camera ->
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
            parameters.flashMode = Camera.Parameters.FLASH_MODE_OFF

            val modes: List<String> =
                parameters.supportedFocusModes
            if (modes.contains(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE)) {
                parameters.focusMode = Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE
            } else if (modes.contains(Camera.Parameters.FOCUS_MODE_FIXED)) {
                parameters.focusMode = Camera.Parameters.FOCUS_MODE_FIXED
            } else if (modes.contains(Camera.Parameters.FOCUS_MODE_INFINITY)) {
                parameters.focusMode = Camera.Parameters.FOCUS_MODE_INFINITY
            } else {
                parameters.focusMode = modes[0]
            }
            camera.parameters = parameters

            camera.setPreviewTexture(surfaceTexture)

            camera.startPreview()
        }
    }


    fun stopPreview() {
        currentCamera?.stopPreview()
        currentCamera?.release()
    }
}