package me.jiahuan.android.audiovideosample

import android.Manifest
import android.content.Intent
import android.os.Bundle
import androidx.activity.result.contract.ActivityResultContracts
import androidx.appcompat.app.AppCompatActivity
import me.jiahuan.android.audiovideosample.camera.CameraActivity
import me.jiahuan.android.audiovideosample.databinding.LayoutActivityMainBinding
import me.jiahuan.android.audiovideosample.egl.OpenglESActivity
import me.jiahuan.android.audiovideosample.ffmpeg.FFMpegActivity

class MainActivity : AppCompatActivity() {

    private val requestStorageContracts =
        registerForActivityResult(ActivityResultContracts.RequestPermission()) {
            if (!it) {
                finish()
            }
        }

    private lateinit var binding: LayoutActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        requestStorageContracts.launch(Manifest.permission.WRITE_EXTERNAL_STORAGE)
        binding = LayoutActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)
        // opengles
        binding.idOpenglesButton.setOnClickListener {
            startActivity(Intent(this, OpenglESActivity::class.java))
        }
        // 摄像
        binding.idCameraButton.setOnClickListener {
            startActivity(Intent(this, CameraActivity::class.java))
        }
        // ffmpeg
        binding.idFfmpegButton.setOnClickListener {
            startActivity(Intent(this, FFMpegActivity::class.java))
        }
    }
}
