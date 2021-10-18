package me.jiahuan.android.audiovideosample.ffmpeg

import android.content.Context
import android.util.AttributeSet
import android.view.SurfaceHolder
import android.view.SurfaceView

class XPlay @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null
) : SurfaceView(context, attrs), SurfaceHolder.Callback, Runnable {
    init {
        holder.addCallback(this)
    }

    override fun surfaceCreated(surfaceHolder: SurfaceHolder) {
//        Thread(this).start()
//        nativeOpenSL()
        nativeOpen3()
    }

    override fun surfaceChanged(surfaceHolder: SurfaceHolder, p1: Int, p2: Int, p3: Int) {
    }

    override fun surfaceDestroyed(surfaceHolder: SurfaceHolder) {
    }

    override fun run() {
//        nativeOpen2("/sdcard/one_piece.yuv", holder.surface)
    }

    private external fun nativeOpen(url:String, surface: Any)

    private external fun nativeOpenSL()

    private external fun nativeOpen2(url: String, surface: Any)

    private external fun nativeOpen3()
}