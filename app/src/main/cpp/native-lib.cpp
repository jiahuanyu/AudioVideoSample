#include <jni.h>
#include "util/log_util.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <EGL/egl.h>
#include <GLES3/gl3.h>

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

/////////////
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


/////////////

// 顶点着色器
#define GET_STR(x) #x
static const char *vertexShader = GET_STR(
        attribute vec4 aPosition;
        attribute vec2 aTexCoord;

        varying vec2 vTexCoord;

        void main() {
            vTexCoord = aTexCoord;
            gl_Position = aPosition;
        }
);

// 片源着色器
static const char *fragmentYUV420PShader = GET_STR(
        precision mediump float;
        uniform sampler2D yTexture;
        uniform sampler2D uTexture;
        uniform sampler2D vTexture;

        varying vec2 vTexCoord;

        void main() {
            vec3 yuv;
            vec3 rgb;
            yuv.x = texture2D(yTexture, vTexCoord).r;
            yuv.y = texture2D(uTexture, vTexCoord).r - 0.5;
            yuv.z = texture2D(vTexture, vTexCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.3945, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;
            gl_FragColor = vec4(rgb, 1.0);
        }
);

GLint InitShader(const char *code, GLint type) {
    GLuint shader = glCreateShader(type);
    if (shader == 0) {
        LOGCATE("glCreateShader %d fail", type);
        return 0;
    }
    // 加载 shader
    glShaderSource(shader, 1, &code, nullptr);
    // 编译 shader
    glCompileShader(shader);
    // 获取编译情况
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        LOGCATE("compile sharder fail");
        return 0;
    } else {
        LOGCATI("compile sharder success");
    }
    return shader;
}

extern "C"
JNIEXPORT void JNICALL
Java_me_jiahuan_android_audiovideosample_ffmpeg_XPlay_nativeOpen2(JNIEnv *env, jobject thiz,
                                                                  jstring jUrl, jobject jSurface) {
    const char *path = env->GetStringUTFChars(jUrl, nullptr);
    FILE *fp = fopen(path, "rb");
    if (fp == nullptr) {
        LOGCATE("open file fail, %s", path);
        return;
    }

    // 获取原始窗口
    ANativeWindow *aNativeWindow = ANativeWindow_fromSurface(env, jSurface);

    // * Display
    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOGCATE("eglGetDisplay fail");
        return;
    }
    // 后面两个参数是版本号
    if (eglInitialize(display, nullptr, nullptr) != EGL_TRUE) {
        LOGCATE("eglInitialize fail");
        return;
    }

    // * Surface
    // 窗口配置
    EGLConfig config; // 输出的配置项
    EGLint configNum; // 输出的配置项
    EGLint configSpec[] = {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT, EGL_NONE
    };
    if (eglChooseConfig(display, configSpec, &config, 1, &configNum) != EGL_TRUE) {
        LOGCATE("eglChooseConfig fail");
        return;
    }
    // 创建 Surface
    EGLSurface surface = eglCreateWindowSurface(display, config, aNativeWindow, nullptr);
    if (surface == EGL_NO_SURFACE) {
        LOGCATE("eglCreateWindowSurface fail");
        return;
    }

    // * Context
    const EGLint ctxAttr[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE
    };
    EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctxAttr);
    if (context == EGL_NO_CONTEXT) {
        LOGCATE("eglCreateContext fail");
        return;
    }
    if (eglMakeCurrent(display, surface, surface, context) != EGL_TRUE) {
        LOGCATE("eglMakeCurrent fail");
        return;
    }
    LOGCATI("EGL INIT SUCCESS");

    GLint vtShader = InitShader(vertexShader, GL_VERTEX_SHADER);
    GLint fragShader = InitShader(fragmentYUV420PShader, GL_FRAGMENT_SHADER);

    GLint program = glCreateProgram();
    if (program == 0) {
        LOGCATE("glCreateProgram fail");
        return;
    }
    // 加入着色器
    glAttachShader(program, vtShader);
    glAttachShader(program, fragShader);

    // 链接
    glLinkProgram(program);

    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        LOGCATE("glLinkProgram fail");
        return;
    }

    // 激活
    glUseProgram(program);

    // 设置顶点坐标
    static float vertexCoords[] = {
            1.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f
    };
    GLint aPositionLocation = glGetAttribLocation(program, "aPosition");
    glEnableVertexAttribArray(aPositionLocation);
    glVertexAttribPointer(aPositionLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          vertexCoords);

    // 设置纹理坐标
    static float textureCoords[] = {
            1.0f, 1.0f,
            0.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
    };
    GLint aTextCoordLocation = glGetAttribLocation(program, "aTexCoord");
    glEnableVertexAttribArray(aTextCoordLocation);
    glVertexAttribPointer(aTextCoordLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                          textureCoords);
    // 设置层
    glUniform1i(glGetUniformLocation(program, "yTexture"), 0);
    glUniform1i(glGetUniformLocation(program, "uTexture"), 1);
    glUniform1i(glGetUniformLocation(program, "vTexture"), 2);

    // 创建三个纹理
    GLuint texts[3] = {0};
    glGenTextures(3, texts);

    // 纹理标准步骤
    // 调用此代码之后的所有纹理设置都相对于此纹理
    glBindTexture(GL_TEXTURE_2D, texts[0]);

    // 设置纹理采样方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width = 424;
    int height = 240;

    // 加载数据
    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_LUMINANCE, // 灰度
            width,
            height,
            0,
            GL_LUMINANCE,
            GL_UNSIGNED_BYTE,
            nullptr
    );
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 纹理标准步骤
    // 调用此代码之后的所有纹理设置都相对于此纹理
    glBindTexture(GL_TEXTURE_2D, texts[1]);

    // 设置纹理采样方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 加载数据
    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_LUMINANCE, // 灰度
            width / 2,
            height / 2,
            0,
            GL_LUMINANCE,
            GL_UNSIGNED_BYTE,
            nullptr
    );
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 纹理标准步骤
    // 调用此代码之后的所有纹理设置都相对于此纹理
    glBindTexture(GL_TEXTURE_2D, texts[2]);

    // yuv420p  yyyy u v

    // 设置纹理采样方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 加载数据
    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_LUMINANCE, // 灰度
            width / 2,
            height / 2,
            0,
            GL_LUMINANCE,
            GL_UNSIGNED_BYTE,
            nullptr
    );
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    unsigned char *buf[3] = {nullptr};
    buf[0] = new unsigned char[width * height];
    buf[1] = new unsigned char[width * height / 4];
    buf[2] = new unsigned char[width * height / 4];

    for (int i = 0; i < 10000; i++) {
//        memset(buf[0], i, width * height);
//        memset(buf[1], i, width * height / 4);
//        memset(buf[2], i, width * height / 4);

        if (feof(fp) == 0) {
            fread(buf[0], 1, width * height, fp);
            fread(buf[1], 1, width * height / 4, fp);
            fread(buf[2], 1, width * height / 4, fp);
        }


        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texts[0]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_LUMINANCE, GL_UNSIGNED_BYTE,
                        buf[0]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texts[1]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_LUMINANCE,
                        GL_UNSIGNED_BYTE, buf[1]);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, texts[2]);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width / 2, height / 2, GL_LUMINANCE,
                        GL_UNSIGNED_BYTE, buf[2]);


        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        eglSwapBuffers(display, surface);
    }

    env->ReleaseStringUTFChars(jUrl, path);
}