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

        // 音频采样率
        private const val AUDIO_SAMPLE_RATE = 44100
        // 音频通道数
        private const val AUDIO_CHANNEL_COUNT = 2
        // 音频精度
        private const val AUDIO_FORMAT = 16
        // 视频编码帧率
        private const val VIDEO_FRAME_RATE = 30
        // 视频关键帧间隔 秒
        private const val VIDEO_I_KET_FRAME_INTERVAL = 1

        private const val WAIT_TIME_US = 50000L
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

    private var isAudioRecordeStoped = false

    //  存放PCM数据的队列
    private val pcmData = ConcurrentLinkedQueue<ByteArray>()

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
        if (::surfaceDrawThread.isInitialized) {
            surfaceDrawThread.exist()
        }
    }

    /**
     * 初始化MediaMuxer
     */
    private fun initMediaMuxer(file: File) {
        // 初始化MediaMuxer
        mediaMuxer = MediaMuxer(file.absolutePath, MediaMuxer.OutputFormat.MUXER_OUTPUT_MPEG_4)
    }

    /**
     * 初始化视频编码环境
     */
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
        // 比特率，RGBA 4字节
        videoFormat.setInteger(MediaFormat.KEY_BIT_RATE, width * height * 4)
        // 帧率
        videoFormat.setInteger(MediaFormat.KEY_FRAME_RATE, VIDEO_FRAME_RATE)
        // 关键帧 1秒
        videoFormat.setInteger(MediaFormat.KEY_I_FRAME_INTERVAL, VIDEO_I_KET_FRAME_INTERVAL)
        videoCodec = MediaCodec.createEncoderByType(MediaFormat.MIMETYPE_VIDEO_AVC)
        videoCodec.configure(videoFormat, null, null, MediaCodec.CONFIGURE_FLAG_ENCODE)

        val surface = videoCodec.createInputSurface()

        // 创建EGLThread需要的Surface EGLContext
        surfaceDrawThread = SurfaceDrawThread()
        surfaceDrawThread.initialize(surface, eglContext)
        surfaceDrawThread.setSurfaceWidthAndHeight(width, height)
        surfaceDrawThread.setRenderer(SurfaceRenderer(context, textureId))

        // 视频编码
        videoEncoderThread = VideoEncoderThread()
    }


    private fun initAudioEncoderEnv() {
        bufferSizeInBytes = AudioRecord.getMinBufferSize(
            AUDIO_SAMPLE_RATE, AudioFormat.CHANNEL_IN_STEREO,
            AudioFormat.ENCODING_PCM_16BIT
        )

        audioRecord = AudioRecord(
            MediaRecorder.AudioSource.MIC, AUDIO_SAMPLE_RATE, AudioFormat.CHANNEL_IN_STEREO,
            AudioFormat.ENCODING_PCM_16BIT,
            bufferSizeInBytes
        )
        audioRecorderThread = AudioRecordThread()


        // 创建音频编码所需
        val audioFormat = MediaFormat.createAudioFormat(
            MediaFormat.MIMETYPE_AUDIO_AAC,
            AUDIO_SAMPLE_RATE,
            AUDIO_CHANNEL_COUNT
        )
        audioFormat.setInteger(
            MediaFormat.KEY_BIT_RATE,
            AUDIO_SAMPLE_RATE * AUDIO_FORMAT * AUDIO_CHANNEL_COUNT
        )
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
            callback?.onEncoderFinished()
        }
    }


    /**
     * 视频（不包括音频）编码线程
     */
    inner class VideoEncoderThread : Thread() {
        private var presentationTimeUs = 0L
        private var realExit = false


        override fun run() {
            val bufferInfo = MediaCodec.BufferInfo()
            videoCodec.start()

            while (true) {
                if (realExit) {
                    videoCodec.stop()
                    videoCodec.release()
                    isVideoEncodeStoped = true
                    releaseMediaMuxer()
                    break
                }

                if (isMediaEncodeToStop) {
                    videoCodec.signalEndOfInputStream()
                }

                var outputBufferIndex = videoCodec.dequeueOutputBuffer(bufferInfo, WAIT_TIME_US)
                if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                    videoTrackIndex = mediaMuxer.addTrack(videoCodec.outputFormat)
                    startMediaMuxer()
                } else {
                    if (isMediaMuxerStated) {
                        if (bufferInfo.flags and MediaCodec.BUFFER_FLAG_END_OF_STREAM != 0 && isMediaEncodeToStop) {
                            Log.d(TAG, "视频结束")
                            realExit = true
                            continue
                        }

                        while (outputBufferIndex >= 0) {
                            val outputBuffer = videoCodec.getOutputBuffer(outputBufferIndex)
                            if (outputBuffer != null) {
                                outputBuffer.position(bufferInfo.offset)
                                outputBuffer.limit(bufferInfo.offset + bufferInfo.size)
                                if (presentationTimeUs == 0L) {
                                    presentationTimeUs = bufferInfo.presentationTimeUs
                                }
                                bufferInfo.presentationTimeUs =
                                    bufferInfo.presentationTimeUs - presentationTimeUs
                                Log.d(TAG, "time = ${bufferInfo.presentationTimeUs / 1000000}")
                                mediaMuxer.writeSampleData(
                                    videoTrackIndex,
                                    outputBuffer,
                                    bufferInfo
                                )
                                videoCodec.releaseOutputBuffer(outputBufferIndex, false)
                            }
                            outputBufferIndex =
                                videoCodec.dequeueOutputBuffer(bufferInfo, WAIT_TIME_US)
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
        private var realExit = false
        // 编码pts
        private var presentationTimeUs = 0L
        // 输入的pts
        private var inputPresentationTimeUs = 0L

        // endOfStream
        private var isEndOfStream = false

        override fun run() {
            val bufferInfo = MediaCodec.BufferInfo()
            audioCodec.start()

            while (true) {
                if (realExit) {
                    audioCodec.stop()
                    audioCodec.release()
                    isAudioEncodeStoped = true
                    releaseMediaMuxer()
                    break
                }

                if (isMediaMuxerStated && !isEndOfStream) {
                    val inputBufferIndex = audioCodec.dequeueInputBuffer(WAIT_TIME_US)
                    if (inputBufferIndex >= 0) {
                        val buffer = pcmData.poll()
                        if (buffer != null && buffer.isNotEmpty()) {
                            // 有数据的话
                            val byteBuffer = audioCodec.getInputBuffer(inputBufferIndex)
                            if (byteBuffer != null) {
                                byteBuffer.clear()
                                byteBuffer.put(buffer)
                                inputPresentationTimeUs += (buffer.size / (AUDIO_SAMPLE_RATE.toFloat() * AUDIO_FORMAT / 8 * AUDIO_CHANNEL_COUNT) * 1000000).toLong()
                                audioCodec.queueInputBuffer(
                                    inputBufferIndex,
                                    0,
                                    buffer.size,
                                    inputPresentationTimeUs,
                                    0
                                )
                            }
                        } else if (isMediaEncodeToStop && isAudioRecordeStoped) {
                            // 无数据且结束
                            audioCodec.queueInputBuffer(
                                inputBufferIndex,
                                0,
                                0,
                                0,
                                MediaCodec.BUFFER_FLAG_END_OF_STREAM
                            )
                            isEndOfStream = true
                        }
                    }
                }

                var outputBufferIndex = audioCodec.dequeueOutputBuffer(bufferInfo, WAIT_TIME_US)
                if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                    audioTrackIndex = mediaMuxer.addTrack(audioCodec.outputFormat)
                    startMediaMuxer()
                } else {
                    if (isMediaMuxerStated) {
                        if (bufferInfo.flags and MediaCodec.BUFFER_FLAG_END_OF_STREAM != 0 && isMediaEncodeToStop) {
                            Log.d(TAG, "音频结束")
                            realExit = true
                            continue
                        }
                        while (outputBufferIndex >= 0) {
                            val outputBuffer = audioCodec.getOutputBuffer(outputBufferIndex)
                            if (outputBuffer != null) {
                                outputBuffer.position(bufferInfo.offset)
                                outputBuffer.limit(bufferInfo.offset + bufferInfo.size)
                                if (presentationTimeUs == 0L) {
                                    presentationTimeUs = bufferInfo.presentationTimeUs
                                }
                                bufferInfo.presentationTimeUs =
                                    bufferInfo.presentationTimeUs - presentationTimeUs
                                mediaMuxer.writeSampleData(
                                    audioTrackIndex,
                                    outputBuffer,
                                    bufferInfo
                                )
                                audioCodec.releaseOutputBuffer(outputBufferIndex, false)

                            }
                            outputBufferIndex =
                                audioCodec.dequeueOutputBuffer(bufferInfo, WAIT_TIME_US)
                        }
                    }
                }
            }
        }
    }


    /**
     * 录音线程
     */
    inner class AudioRecordThread : Thread() {

        override fun run() {
            audioRecord.startRecording()


            while (true) {
                if (isMediaEncodeToStop) {
                    audioRecord.stop()
                    audioRecord.release()
                    isAudioRecordeStoped = true
                    break
                }

                val audioData = ByteArray(bufferSizeInBytes)
                val len = audioRecord.read(audioData, 0, bufferSizeInBytes)
                Log.d(TAG, "len = $len")
                if (len > 0) {
                    pcmData.offer(audioData.copyOfRange(0, len))
                }
            }
        }
    }

    private var callback: Callback? = null

    fun setCallback(callback: Callback) {
        this.callback = callback
    }

    interface Callback {
        fun onEncoderFinished()
    }
}