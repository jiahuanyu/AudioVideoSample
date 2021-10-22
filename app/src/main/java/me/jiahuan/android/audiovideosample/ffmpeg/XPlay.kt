package me.jiahuan.android.audiovideosample.ffmpeg

import android.content.Context
import android.util.AttributeSet
import android.view.SurfaceHolder
import android.view.SurfaceView

class XPlay @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null
) : SurfaceView(context, attrs), SurfaceHolder.Callback {
    init {
        System.loadLibrary("native-lib")
    }

    init {
        holder.addCallback(this)
    }

    override fun surfaceCreated(surfaceHolder: SurfaceHolder) {
    }

    override fun surfaceChanged(
        surfaceHolder: SurfaceHolder,
        format: Int,
        width: Int,
        height: Int
    ) {
    }

    override fun surfaceDestroyed(surfaceHolder: SurfaceHolder) {
    }

    private external fun nativeOpen(url: String, surface: Any)

    private external fun nativeOpenSL()

    private external fun nativeOpen2(url: String, surface: Any)
}