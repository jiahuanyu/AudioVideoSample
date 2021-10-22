package me.jiahuan.android.audiovideosample.ffmpeg

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import me.jiahuan.android.audiovideosample.databinding.LayoutActivityFfmpegBinding

/**
 * FFMpeg 相关示例
 */
class FFMpegActivity : AppCompatActivity() {

    private lateinit var binding: LayoutActivityFfmpegBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = LayoutActivityFfmpegBinding.inflate(layoutInflater)
        setContentView(binding.root)
        binding.idPlayer.init("/sdcard/1080.mp4")
    }

    override fun onResume() {
        super.onResume()
        binding.idPlayer.start()
    }

    override fun onPause() {
        super.onPause()
        binding.idPlayer.pause()
    }

    override fun onDestroy() {
        super.onDestroy()
        binding.idPlayer.unInit()
    }
}