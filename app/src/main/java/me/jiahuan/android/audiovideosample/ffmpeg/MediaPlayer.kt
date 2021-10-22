package me.jiahuan.android.audiovideosample.ffmpeg

import android.content.Context
import android.util.AttributeSet
import android.util.Log
import android.view.SurfaceHolder
import android.view.SurfaceView

private const val TAG = "MediaPlayer"

class MediaPlayer @JvmOverloads constructor(
    context: Context, attrs: AttributeSet? = null
) : SurfaceView(context, attrs), SurfaceHolder.Callback {

    // Native MediaPlayer 指针
    private var mNativePlayerHandle: Long = 0

    private var mRatioWidth = 0
    private var mRatioHeight = 0

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

    override fun onMeasure(widthMeasureSpec: Int, heightMeasureSpec: Int) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec)
        val width = MeasureSpec.getSize(widthMeasureSpec)
        val height = MeasureSpec.getSize(heightMeasureSpec)

        if (0 == mRatioWidth || 0 == mRatioHeight) {
            setMeasuredDimension(width, height)
        } else {
            if (width < height * mRatioWidth / mRatioHeight) {
                setMeasuredDimension(width, width * mRatioHeight / mRatioWidth)
            } else {
                setMeasuredDimension(height * mRatioWidth / mRatioHeight, height)
            }
        }
    }

    // 由 Native 调用
    private fun playerEventCallback(msgType: Int, msgValue: Float) {
        Log.i(TAG, "MediaPlayer playerEventCallback msgType = $msgType, msgValue = $msgValue")
        setAspectRatio(
            nativeGetVideoWidth(mNativePlayerHandle),
            nativeGetVideoHeight(mNativePlayerHandle)
        )
    }

    private fun setAspectRatio(width: Int, height: Int) {
        mRatioWidth = width
        mRatioHeight = height
        requestLayout()
    }

    private external fun nativeInit(uri: String): Long

    private external fun nativeUnInit(playerHandle: Long)

    private external fun nativeSurfaceCreated(playerHandle: Long, surface: Any)

    private external fun nativeStart(playerHandle: Long)

    private external fun nativePause(playerHandle: Long)

    private external fun nativeStop(playerHandle: Long)

    private external fun nativeGetVideoWidth(playerHandle: Long): Int

    private external fun nativeGetVideoHeight(playerHandle: Long): Int
}