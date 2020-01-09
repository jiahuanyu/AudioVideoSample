package me.jiahuan.android.audiovideosample

import android.content.Intent
import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_main.*
import me.jiahuan.android.audiovideosample.camera.CameraActivity
import me.jiahuan.android.audiovideosample.egl.OpenglESActivity
import me.jiahuan.android.audiovideosample.texture.TexutureActivity

class MainActivity : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // opengles
        id_opengles_button.setOnClickListener {
            startActivity(Intent(this, OpenglESActivity::class.java))
        }

        id_texture_button.setOnClickListener {
            startActivity(Intent(this, TexutureActivity::class.java))
        }

        id_camera_button.setOnClickListener {
            startActivity(Intent(this, CameraActivity::class.java))
        }
    }
}
