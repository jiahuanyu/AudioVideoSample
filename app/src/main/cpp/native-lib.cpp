#include <jni.h>
#include "util/log_util.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

static double r2d(AVRational r) {
    return r.num == 0 || r.den == 0 ? 0 : (double) r.num / r.den;
}

extern "C"
JNIEXPORT void JNICALL
Java_me_jiahuan_android_audiovideosample_ffmpeg_XPlay_nativeOpen(JNIEnv *env, jobject thiz,
                                                                 jstring jUrl, jobject jSurface) {
//    avformat_network_init();

    // * 创建封装格式上下文
    AVFormatContext *avFormatContext = avformat_alloc_context();

    // * 打开文件
    const char *path = env->GetStringUTFChars(jUrl, nullptr);
    int result = avformat_open_input(&avFormatContext, path, nullptr, nullptr);
    env->ReleaseStringUTFChars(jUrl, path);
    if (result != 0) {
        LOGCATE("avformat_open_input failed! %s", av_err2str(result));
        return;
    }
    LOGCATI("avformat_open_input %s success", path);

    // * 获取音视频流信息
    // 对于没有头部信息，可以探测媒体信息
    result = avformat_find_stream_info(avFormatContext, nullptr);
    if (result < 0) {
        LOGCATE("avformat_find_stream_info failed!");
        return;
    }
    // duration: 总时长（微秒）, 流（视频流/音频流==）数量
    LOGCATI("duration = %ld, nb_streams = %d", avFormatContext->duration,
            avFormatContext->nb_streams);


    // * 获取音视频索引
    int videoStreamIndex = -1;
    int audioStreamIndex = -1;
    for (int i = 0; i < avFormatContext->nb_streams; i++) {
        AVStream *avStream = avFormatContext->streams[i];
        // 判断流是视频
        if (avStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            LOGCATI("avStream video fps = %d, width = %d, height = %d, codeId = %d, format = %d",
                    (int) r2d(avStream->avg_frame_rate),
                    avStream->codecpar->width,
                    avStream->codecpar->height,
                    avStream->codecpar->codec_id,
                    avStream->codecpar->format);
            videoStreamIndex = i;
        } else if (avStream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            // 判断流是音频
            LOGCATI("avStream audio sample_rate = %d, channels = %d, format = %d",
                    avStream->codecpar->sample_rate,
                    avStream->codecpar->channels,
                    avStream->codecpar->format);
            audioStreamIndex = i;
        }
    }
    // 判断视频流是否存在
    if (videoStreamIndex == -1 && audioStreamIndex == -1) {
        return;
    }

    // * 获取解码器
    //////////////////  视频 ///////////////////
    // 软解码
    const AVCodec *videoAvCodec = avcodec_find_decoder(
            avFormatContext->streams[videoStreamIndex]->codecpar->codec_id);
    // 硬解码
//    videoAvCodec = avcodec_find_decoder_by_name("h264_mediacodec");
    if (videoAvCodec == nullptr) {
        return;
    }

    // * 创建解码器上下文
    AVCodecContext *videoAvCodecContext = avcodec_alloc_context3(videoAvCodec);
    result = avcodec_parameters_to_context(videoAvCodecContext,
                                           avFormatContext->streams[videoStreamIndex]->codecpar);
    if (result != 0) {
        LOGCATE("video avcodec_parameters_to_context fail");
        return;
    }

    // * 打开解码器
    result = avcodec_open2(videoAvCodecContext, videoAvCodec, nullptr);
    if (result < 0) {
        LOGCATE("video avcodec_open2 fail. result=%d", result);
        return;
    }

    // * 解码循环
    // 存放编码数据（帧数据）
    AVPacket *avPacket = av_packet_alloc();
    // 存放解码数据
    AVFrame *avFrame = av_frame_alloc();

    SwsContext *swsContext = nullptr;

    // 视频输出宽高
    int outWidth = 1280;
    int outHeight = 720;

    uint8_t *rgb = new uint8_t[outWidth * outHeight * 4];

    // * 初始化显示窗口
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, jSurface);
    // 设置渲染区域输入格式
    ANativeWindow_setBuffersGeometry(nativeWindow, outWidth,
                                     outHeight, WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer nativeWindowBuffer;

    for (;;) {
        result = av_read_frame(avFormatContext, avPacket);
        if (result != 0) {
            break;
//            av_seek_frame(avFormatContext, videoStreamIndex,
//                          5 * r2d(avFormatContext->streams[videoStreamIndex]->time_base),
//                          AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
//            continue;
        }
        if (avPacket->stream_index != videoStreamIndex) {
            continue;
        }
        AVCodecContext *currentCodecContext = nullptr;
        if (avPacket->stream_index == videoStreamIndex) {
            currentCodecContext = videoAvCodecContext;
        }
        if (currentCodecContext == nullptr) {
            continue;
        }
        // 当前只处理视频
        result = avcodec_send_packet(currentCodecContext, avPacket);
        // 清理
        av_packet_unref(avPacket);
        if (result != 0) {
            LOGCATE("avcodec_send_packet fail");
            continue;
        }
        for (;;) {
            result = avcodec_receive_frame(currentCodecContext, avFrame);
            if (result != 0) {
                break;
            }
            LOGCATI("avcodec_receive_frame pts = %ld", avFrame->pts);
            // 宽高比不变的情况下，只会初始化一次
            swsContext = sws_getCachedContext(
                    swsContext,
                    avFrame->width,
                    avFrame->height,
                    (AVPixelFormat) avFrame->format,
                    outWidth,
                    outHeight,
                    AV_PIX_FMT_RGBA,
                    SWS_FAST_BILINEAR,
                    nullptr,
                    nullptr,
                    nullptr
            );
            if (swsContext == nullptr) {
                LOGCATE("sws_getCachedContext fail");
            } else {
                uint8_t *data[AV_NUM_DATA_POINTERS] = {nullptr};
                data[0] = rgb;
                int lines[AV_NUM_DATA_POINTERS] = {0};
                lines[0] = outWidth * 4;
                int h = sws_scale(
                        swsContext,
                        avFrame->data,
                        avFrame->linesize,
                        0,
                        avFrame->height,
                        data,
                        lines
                );
                LOGCATI("sws_scale height = %d", h);
                if (h > 0) {
                    ANativeWindow_lock(nativeWindow, &nativeWindowBuffer, nullptr);
                    uint8_t *dstBuffer = static_cast<uint8_t *>(nativeWindowBuffer.bits);
                    memcpy(dstBuffer, rgb, outWidth * outHeight * 4);
                    ANativeWindow_unlockAndPost(nativeWindow);
                }
            }
        }
    }

    // * 释放资源
    if (nativeWindow != nullptr) {
        ANativeWindow_release(nativeWindow);
    }

    if (swsContext != nullptr) {
        sws_freeContext(swsContext);
        swsContext = nullptr;
    }

    if (avFrame != nullptr) {
        av_frame_free(&avFrame);
        avFrame = nullptr;
    }

    if (avPacket != nullptr) {
        av_packet_free(&avPacket);
        avPacket = nullptr;
    }

    if (videoAvCodecContext != nullptr) {
        avcodec_close(videoAvCodecContext);
        avcodec_free_context(&videoAvCodecContext);
        videoAvCodecContext = nullptr;
    }

    if (avFormatContext != nullptr) {
        avformat_close_input(&avFormatContext);
        avformat_free_context(avFormatContext);
        avFormatContext = nullptr;
    }

    delete[] rgb;
}


static SLObjectItf slEngine = nullptr;

SLEngineItf CreateSL() {
    SLresult result;
    result = slCreateEngine(&slEngine, 0, nullptr, 0, nullptr, nullptr);
    if (result != SL_RESULT_SUCCESS) {
        return nullptr;
    }
    // 实例化
    result = (*slEngine)->Realize(slEngine, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        return nullptr;
    }
    SLEngineItf slEngineItf;
    result = (*slEngine)->GetInterface(slEngine, SL_IID_ENGINE, &slEngineItf);
    if (result != SL_RESULT_SUCCESS) {
        return nullptr;
    }
    return slEngineItf;
}

/**
 * 回调
 */

void PCMCallback(SLAndroidSimpleBufferQueueItf queue, void *context) {
    LOGCATI("PCMCallback");
    static FILE *fp = nullptr;
    static char *buffer = nullptr;
    if (buffer == nullptr) {
        buffer = new char[1024 * 1024];
    }
    if (fp == nullptr) {
        fp = fopen("/sdcard/test.pcm", "rb");
    }
    if (fp == nullptr) {
        LOGCATI("open fail");
        return;
    }
    if (feof(fp) == 0) {
        // 没有到文件结尾
        int len = fread(buffer, 1, 1024, fp);
        if (len > 0) {
            (*queue)->Enqueue(queue, buffer, len);
        }
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_me_jiahuan_android_audiovideosample_ffmpeg_XPlay_nativeOpenSL(JNIEnv *env, jobject thiz) {
    // * 创建引擎
    SLEngineItf slEngineItf = CreateSL();
    if (slEngineItf != nullptr) {
        LOGCATI("CreateSL success");
    } else {
        LOGCATI("CreateSL fail");
        return;
    }

    // * 创建混音器
    SLObjectItf slMix = nullptr;
    (*slEngineItf)->CreateOutputMix(slEngineItf, &slMix, 0, nullptr, nullptr);
    SLresult result = (*slMix)->Realize(slMix, SL_BOOLEAN_FALSE);
    if (result != SL_RESULT_SUCCESS) {
        LOGCATI("CreateOutputMix fail");
        return;
    } else {
        LOGCATI("CreateOutputMix success");
    }

    SLDataLocator_OutputMix slDataLocatorOutputMix = {
            SL_DATALOCATOR_OUTPUTMIX,
            slMix
    };
    SLDataSink slDataSink = {
            &slDataLocatorOutputMix,
            nullptr
    };

    // * 配置音频信息
    // 缓冲队列
    SLDataLocator_AndroidSimpleBufferQueue queue = {
            SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
            10
    };
    // 音频格式
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            2,
            SL_SAMPLINGRATE_44_1,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN, // 小端
    };
    SLDataSource dataSource = {&queue, &pcm};

    // * 创建播放器
    SLObjectItf slPlayer = nullptr;
    const SLInterfaceID ids[] = {SL_IID_BUFFERQUEUE};
    const SLboolean req[] = {SL_BOOLEAN_TRUE};
    result = (*slEngineItf)->CreateAudioPlayer(
            slEngineItf,
            &slPlayer,
            &dataSource,
            &slDataSink,
            sizeof(ids) / sizeof(SLInterfaceID),
            ids,
            req
    );
    if (result != SL_RESULT_SUCCESS) {
        LOGCATI("CreateAudioPlayer fail");
        return;
    } else {
        LOGCATI("CreateAudioPlayer success");
    }

    SLPlayItf slPlayItf = nullptr;
    (*slPlayer)->Realize(slPlayer, SL_BOOLEAN_FALSE);
    result = (*slPlayer)->GetInterface(slPlayer, SL_IID_PLAY, &slPlayItf);
    if (result != SL_RESULT_SUCCESS) {
        LOGCATI("GetInterface SL_IID_PLAY fail");
        return;
    } else {
        LOGCATI("GetInterface SL_IID_PLAY success");
    }

    SLAndroidSimpleBufferQueueItf slAndroidSimpleBufferQueueItf = nullptr;
    result = (*slPlayer)->GetInterface(slPlayer, SL_IID_BUFFERQUEUE,
                                       &slAndroidSimpleBufferQueueItf);
    if (result != SL_RESULT_SUCCESS) {
        LOGCATI("GetInterface SL_IID_BUFFERQUEUE fail");
        return;
    } else {
        LOGCATI("GetInterface SL_IID_BUFFERQUEUE success");
    }

    // 设置回调函数，播放队列空调用
    (*slAndroidSimpleBufferQueueItf)->RegisterCallback(
            slAndroidSimpleBufferQueueItf,
            PCMCallback,
            nullptr
    );

    (*slPlayItf)->SetPlayState(slPlayItf, SL_PLAYSTATE_PLAYING);

    // 启动队列回调
    (*slAndroidSimpleBufferQueueItf)->Enqueue(slAndroidSimpleBufferQueueItf, "", 1);
}
