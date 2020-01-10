package me.jiahuan.android.audiovideosample.encoder

import android.content.Context
import android.media.MediaCodec
import android.media.MediaCodecInfo
import android.media.MediaFormat
import android.media.MediaMuxer
import javax.microedition.khronos.egl.EGLContext

/**
 * 音视频媒体编码器
 */
class MediaEncoder {

    private lateinit var surfaceDrawThread: SurfaceDrawThread

    private lateinit var videoEncoderThread: VideoEncoderThread

    private lateinit var mediaMuxer: MediaMuxer

    fun start(context: Context, width: Int, height: Int, eglContext: EGLContext?, textureId: Int) {

        // 初始化MediaMuxer
        mediaMuxer = MediaMuxer("/sdcard/test.mp4", MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4)

        // 创建视频编码所需
        val videoFormat =
            MediaFormat.createVideoFormat(MediaFormat.MIMETYPE_VIDEO_AVC, width, height)
        videoFormat.setInteger(
            MediaFormat.KEY_COLOR_FORMAT,
            MediaCodecInfo.CodecCapabilities.COLOR_FormatSurface
        )
        // 比特率
        videoFormat.setInteger(MediaFormat.KEY_BIT_RATE, width * height * 4)
        // 帧率
        videoFormat.setInteger(MediaFormat.KEY_FRAME_RATE, 30)
        // 关键帧 1秒
        videoFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, 1)
        val videoCodec = MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_VIDEO_AVC)
        videoCodec.configure(videoFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE)

        val surface = videoCodec.createInputSurface()


        // 创建EGLThread需要的Surface EGLContext
        surfaceDrawThread = SurfaceDrawThread()
        surfaceDrawThread.surface = surface
        surfaceDrawThread.width = width
        surfaceDrawThread.height = height
        surfaceDrawThread.eglContext = eglContext
        surfaceDrawThread.isCreated = true
        surfaceDrawThread.isChanged = true
        surfaceDrawThread.eglRenderer = SurfaceRenderer(context, textureId)


        // 视频编码
        videoEncoderThread = VideoEncoderThread(videoCodec, videoFormat, mediaMuxer)


        surfaceDrawThread.start()
        videoEncoderThread.start()
    }


    fun stop() {
        videoEncoderThread.isExist = true
        surfaceDrawThread.isExit = true
        mediaMuxer.stop()
        mediaMuxer.release()
    }
}