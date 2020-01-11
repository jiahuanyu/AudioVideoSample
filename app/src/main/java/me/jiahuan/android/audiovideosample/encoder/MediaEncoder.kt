package me.jiahuan.android.audiovideosample.encoder

import android.content.Context
import android.media.*
import android.util.Log
import java.io.File
import java.util.concurrent.ConcurrentLinkedQueue
import javax.microedition.khronos.egl.EGLContext

/**
 * 音视频媒体编码器
 */
class MediaEncoder {

    companion object {
        private const val TAG = "MediaEncoder"
    }

    private lateinit var surfaceDrawThread: SurfaceDrawThread

    private lateinit var videoEncoderThread: VideoEncoderThread

    private lateinit var audioEncoderThread: AudioEncoderThread

    private lateinit var audioRecorderThread: AudioRecordThread

    private lateinit var mediaMuxer: MediaMuxer

    private lateinit var videoCodec: MediaCodec

    private lateinit var audioCodec: MediaCodec

    private lateinit var audioRecord: AudioRecord

    private var bufferSizeInBytes: Int = 0

    private var videoTrackIndex = -1
    private var audioTrackIndex = -1

    // MediaMuxer是否开启，是否调用了start方法
    private var isMediaMuxerStated = false

    // 退出编码
    private var isMediaEncodeToStop = false

    private var isVideoEncodeStoped = false

    private var isAudioEncodeStoped = false

    //  存放

    fun start(
        context: Context,
        width: Int,
        height: Int,
        eglContext: EGLContext?,
        textureId: Int,
        file: File
    ) {

        initMediaMuxer(file)

        initVideoEncoderEnv(context, width, height, eglContext, textureId)

        initAudioEncoderEnv()

        surfaceDrawThread.start()
        videoEncoderThread.start()
        audioEncoderThread.start()
        audioRecorderThread.start()
    }


    fun stop() {
        isMediaEncodeToStop = true
        surfaceDrawThread.isExit = true
    }

    private fun initMediaMuxer(file: File) {
        // 初始化MediaMuxer
        mediaMuxer = MediaMuxer(file.absolutePath, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4)
    }

    private fun initVideoEncoderEnv(
        context: Context,
        width: Int,
        height: Int,
        eglContext: EGLContext?,
        textureId: Int
    ) {
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
        videoCodec = MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_VIDEO_AVC)
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
        videoEncoderThread = VideoEncoderThread()
    }


    private fun initAudioEncoderEnv() {
        bufferSizeInBytes = AudioRecord.getMinBufferSize(
            44100, AudioFormat.CHANNEL_IN_STEREO,
            AudioFormat.ENCODING_PCM_16BIT
        )

        audioRecord = AudioRecord(
            MediaRecorder.AudioSource.MIC, 44100, AudioFormat.CHANNEL_IN_STEREO,
            AudioFormat.ENCODING_PCM_16BIT,
            bufferSizeInBytes
        )
        audioRecorderThread = AudioRecordThread()


        // 创建音频编码所需
        val audioFormat = MediaFormat.createAudioFormat(MediaFormat.MIMETYPE_AUDIO_AAC, 44100, 2)
        audioFormat.setInteger(MediaFormat.KEY_BIT_RATE, 44100 * 16 * 2)
        audioFormat.setInteger(
            MediaFormat.KEY_AAC_PROFILE,
            MediaCodecInfo.CodecProfileLevel.AACObjectLC
        )
        audioFormat.setInteger(
            MediaFormat.KEY_MAX_INPUT_SIZE,
            bufferSizeInBytes
        )
        audioCodec = MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_AUDIO_AAC)
        audioCodec.configure(audioFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE)
        // 音频编码
        audioEncoderThread = AudioEncoderThread()
    }

    private fun startMediaMuxer() {
        if (videoTrackIndex != -1 && audioTrackIndex != -1) {
            mediaMuxer.start()
            isMediaMuxerStated = true
        }
    }

    private fun releaseMediaMuxer() {
        if (isVideoEncodeStoped && isAudioEncodeStoped) {
            if (isMediaMuxerStated) {
                mediaMuxer.stop()
            }
            mediaMuxer.release()
        }
    }


    /**
     * 视频（不包括音频）编码线程
     */
    inner class VideoEncoderThread : Thread() {
        private var pts = 0L

        override fun run() {
            val bufferInfo = MediaCodec.BufferInfo()
            videoCodec.start()

            while (true) {
                if (isMediaEncodeToStop) {
                    videoCodec.stop()
                    videoCodec.release()
                    isVideoEncodeStoped = true
                    releaseMediaMuxer()
                    break
                }

                var outputBufferIndex = videoCodec.dequeueOutputBuffer(bufferInfo, 0)

                if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                    videoTrackIndex = mediaMuxer.addTrack(videoCodec.outputFormat)
                    startMediaMuxer()
                } else {
                    if (isMediaMuxerStated) {
                        while (outputBufferIndex >= 0) {

                            val outputBuffer = videoCodec.getOutputBuffer(outputBufferIndex)
                            if (outputBuffer != null) {
                                outputBuffer.position(bufferInfo.offset)
                                outputBuffer.limit(bufferInfo.offset + bufferInfo.size)
                                if (pts == 0L) {
                                    pts = bufferInfo.presentationTimeUs
                                }
                                bufferInfo.presentationTimeUs = bufferInfo.presentationTimeUs - pts
                                Log.d(TAG, "time = ${bufferInfo.presentationTimeUs / 1000000}")
                                mediaMuxer.writeSampleData(
                                    videoTrackIndex,
                                    outputBuffer,
                                    bufferInfo
                                )
                                videoCodec.releaseOutputBuffer(outputBufferIndex, false)
                            }

                            outputBufferIndex = videoCodec.dequeueOutputBuffer(bufferInfo, 0)
                        }
                    }
                }
            }
        }
    }


    /**
     * 音频编码线程
     */
    inner class AudioEncoderThread : Thread() {

        private var pts = 0L

        override fun run() {
            val bufferInfo = MediaCodec.BufferInfo()
            audioCodec.start()

            while (true) {
                if (isMediaEncodeToStop) {
                    audioCodec.stop()
                    audioCodec.release()
                    isAudioEncodeStoped = true
                    releaseMediaMuxer()
                    break
                }

                val buffer = pcmData.poll()

                if (isMediaMuxerStated && buffer != null && buffer.isNotEmpty()) {
                    val inputBufferIndex = audioCodec.dequeueInputBuffer(0)
                    if (inputBufferIndex > 0) {
                        val byteBuffer = audioCodec.getInputBuffer(inputBufferIndex)
                        if (byteBuffer != null) {
                            byteBuffer.clear()
                            byteBuffer.put(buffer)
                            calcAudioPts(buffer.size)
                            audioCodec.queueInputBuffer(
                                inputBufferIndex,
                                0,
                                buffer.size,
                                audioPts,
                                0
                            )
                        }
                    }
                }

                var outputBufferIndex = audioCodec.dequeueOutputBuffer(bufferInfo, 0)
                if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                    audioTrackIndex = mediaMuxer.addTrack(audioCodec.outputFormat)
                    startMediaMuxer()
                } else {
                    if (isMediaMuxerStated) {
                        while (outputBufferIndex >= 0) {
                            val outputBuffer = audioCodec.getOutputBuffer(outputBufferIndex)
                            if (outputBuffer != null) {
                                outputBuffer.position(bufferInfo.offset)
                                outputBuffer.limit(bufferInfo.offset + bufferInfo.size)
                                if (pts == 0L) {
                                    pts = bufferInfo.presentationTimeUs
                                }
                                bufferInfo.presentationTimeUs =
                                    bufferInfo.presentationTimeUs - pts
                                mediaMuxer.writeSampleData(
                                    audioTrackIndex,
                                    outputBuffer,
                                    bufferInfo
                                )
                                audioCodec.releaseOutputBuffer(outputBufferIndex, false)
                            }

                            outputBufferIndex = audioCodec.dequeueOutputBuffer(bufferInfo, 0)
                        }
                    }
                }
            }
        }
    }

    private val pcmData = ConcurrentLinkedQueue<ByteArray>()


    /**
     * 录音线程
     */
    inner class AudioRecordThread : Thread() {

        override fun run() {
            audioRecord.startRecording()

            val audioData = ByteArray(bufferSizeInBytes)

            while (true) {
                if (isMediaEncodeToStop) {
                    audioRecord.stop()
                    audioRecord.release()
                    break
                }
                val len = audioRecord.read(audioData, 0, bufferSizeInBytes)
                if (len > 0) {
                    pcmData.add(audioData.copyOfRange(0, len))
                }
            }
        }
    }

    private var audioPts = 0L

    private fun calcAudioPts(size: Int) {
        audioPts += (size / (44100f * 2 * 2) * 1000000).toLong()
    }
}