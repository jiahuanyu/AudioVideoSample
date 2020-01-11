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
                id_camera_view.surfaceWidth,
                id_camera_view.surfaceHeight,
                id_camera_view.getEGLContext(),
                id_camera_view.textureId,
                File("/sdcard/test.mp4")
            )
        }


        id_stop_record_button.setOnClickListener {
            mediaEncoder.stop()
        }
    }

    override fun onResume() {
        super.onResume()
        id_camera_view.onResume()
    }

    override fun onPause() {
        super.onPause()
        id_camera_view.onPause()
    }
}