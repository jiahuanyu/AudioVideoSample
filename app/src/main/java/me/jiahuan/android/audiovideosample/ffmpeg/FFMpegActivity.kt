package me.jiahuan.android.audiovideosample.ffmpeg

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import me.jiahuan.android.audiovideosample.databinding.LayoutActivityFfmpegBinding

/**
 * FFMpeg 相关示例
 */
class FFMpegActivity : AppCompatActivity() {

    companion object {
        init {
            System.loadLibrary("native-lib")
        }
    }

    private lateinit var binding: LayoutActivityFfmpegBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        binding = LayoutActivityFfmpegBinding.inflate(layoutInflater)
        setContentView(binding.root)
    }
}