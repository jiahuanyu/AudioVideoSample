package me.jiahuan.android.audiovideosample.screen

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import kotlinx.android.synthetic.main.activity_camera.*
import me.jiahuan.android.audiovideosample.R

class ScreenActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_screen)
        val screenEncoder = ScreenEncoder()
        id_start_record_button.setOnClickListener {
            screenEncoder.start()
        }

        id_stop_record_button.setOnClickListener {
            screenEncoder.stop()
        }
    }
}