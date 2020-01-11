package me.jiahuan.android.audiovideosample.camera

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_camera.*
import me.jiahuan.android.audiovideosample.R
import me.jiahuan.android.audiovideosample.encoder.MediaEncoder
import java.io.File

class CameraActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_camera)
        var mediaEncoder = MediaEncoder()

        id_start_record_button.setOnClickListener {
            mediaEncoder = MediaEncoder()
            mediaEncoder.start(
                this,
                cameraView.surfaceWidth,
                cameraView.surfaceHeight,
                cameraView.getEGLContext(),
                cameraView.textureId,
                File("/sdcard/test.mp4")
            )
        }


        id_stop_record_button.setOnClickListener {
            mediaEncoder.stop()
        }
    }
}