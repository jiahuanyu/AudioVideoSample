package me.jiahuan.android.audiovideosample.encoder

import android.media.MediaCodec
import android.media.MediaFormat
import android.media.MediaMuxer

/**
 * 视频（不包括音频）编码线程
 */
class VideoEncoderThread(
    private val mediaCodec: MediaCodec,
    private val mediaFormat: MediaFormat,
    private val mediaMuxer: MediaMuxer
) :
    Thread() {

    var isExist = false

    private var videoTrackIndex = 0
    private var pts = 0L

    override fun run() {
        mediaCodec.start()
        val bufferInfo = MediaCodec.BufferInfo()

        while (true) {
            if (isExist) {
                release()
                break
            }

            var outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 0)

            if (outputBufferIndex == MediaCodec.INFO_OUTPUT_FORMAT_CHANGED) {
                videoTrackIndex = mediaMuxer.addTrack(mediaCodec.outputFormat)
                mediaMuxer.start()
            } else {

                while (outputBufferIndex >= 0) {

                    val outputBuffer = mediaCodec.getOutputBuffer(outputBufferIndex)
                    if (outputBuffer != null) {
                        outputBuffer.position(bufferInfo.offset)
                        outputBuffer.limit(bufferInfo.offset + bufferInfo.size)
                        if (pts == 0L) {
                            pts = bufferInfo.presentationTimeUs
                        }
                        bufferInfo.presentationTimeUs = bufferInfo.presentationTimeUs - pts

                        mediaMuxer.writeSampleData(videoTrackIndex, outputBuffer, bufferInfo)
                        mediaCodec.releaseOutputBuffer(outputBufferIndex, false)
                    }

                    outputBufferIndex = mediaCodec.dequeueOutputBuffer(bufferInfo, 0)
                }
            }
        }
    }

    private fun release() {
        mediaCodec.stop()
        mediaCodec.release()
    }
}