package me.jiahuan.android.audiovideosample.ffmpeg

import android.content.Context
import android.util.AttributeSet
import android.view.SurfaceHolder
import android.view.SurfaceView

class MediaPlayer @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null
) : SurfaceView(context, attrs), SurfaceHolder.Callback {

    // Native MediaPlayer 指针
    private var mNativePlayerHandle: Long = 0

    init {
        System.loadLibrary("native-lib")
        holder.addCallback(this)
    }

    fun init(uri: String) {
        mNativePlayerHandle = nativeInit(uri)
    }

    fun unInit() {
        nativeUnInit(mNativePlayerHandle)
    }

    fun start() {
        nativeStart(mNativePlayerHandle)
    }

    fun pause() {
        nativePause(mNativePlayerHandle)
    }

    fun stop() {
        nativeStop(mNativePlayerHandle)
    }

    override fun surfaceCreated(surfaceHolder: SurfaceHolder) {
        nativeSurfaceCreated(mNativePlayerHandle, surfaceHolder.surface)
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

    private external fun nativeInit(uri: String): Long

    private external fun nativeUnInit(playerHandle: Long)

    private external fun nativeSurfaceCreated(playerHandle:Long, surface: Any)

    private external fun nativeStart(playerHandle: Long)

    private external fun nativePause(playerHandle: Long)

    private external fun nativeStop(playerHandle: Long)
}