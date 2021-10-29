package me.jiahuan.android.audiovideosample.ffmpeg

import android.content.Context
import android.opengl.GLSurfaceView
import android.util.AttributeSet
import android.util.Log
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

private const val TAG = "MediaPlayer"

class MediaPlayer @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null
) : GLSurfaceView(context, attrs), GLSurfaceView.Renderer {

    // Native MediaPlayer 指针
    private var mNativePlayerHandle: Long = 0


    init {
        System.loadLibrary("native-lib")
        setEGLContextClientVersion(2)
        setRenderer(this)
        renderMode = RENDERMODE_WHEN_DIRTY
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

    // 由 Native 调用
    private fun playerEventCallback(msgType: Int, msgValue: Float) {
        Log.i(TAG, "MediaPlayer playerEventCallback msgType = $msgType, msgValue = $msgValue")
        if (msgType == 1) {
            requestRender()
        }
    }

    override fun onSurfaceCreated(gl10: GL10, eglConfig: EGLConfig) {
        nativeOnSurfaceCreated(mNativePlayerHandle)
    }

    override fun onSurfaceChanged(p0: GL10, width: Int, height: Int) {
        Log.i(TAG, "onSurfaceChanged ${Thread.currentThread().name}")
        nativeOnSurfaceChanged(mNativePlayerHandle, width, height)
    }

    override fun onDrawFrame(p0: GL10) {
        Log.i(TAG, "onDrawFrame ${Thread.currentThread().name}")
        nativeOnDrawFrame(mNativePlayerHandle)
    }

    private external fun nativeInit(uri: String): Long

    private external fun nativeUnInit(playerHandle: Long)

    private external fun nativeOnSurfaceCreated(playerHandle: Long)

    private external fun nativeOnSurfaceChanged(playerHandle: Long, width: Int, height: Int)

    private external fun nativeOnDrawFrame(playerHandle: Long)

    private external fun nativeStart(playerHandle: Long)

    private external fun nativePause(playerHandle: Long)

    private external fun nativeStop(playerHandle: Long)
}