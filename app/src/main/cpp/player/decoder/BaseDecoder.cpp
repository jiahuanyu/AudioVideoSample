#include "BaseDecoder.h"
#include "../../util/log_util.h"
#include <string>

/**
 * 开始解码
 */
void BaseDecoder::Start() {
    if (!m_Thread) {
        StartDecodingThread();
    } else {
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_DecoderState = STATE_DECODING;
        m_Cond.notify_all();
    }
}

void BaseDecoder::Pause() {
    std::unique_lock<std::mutex> lock(m_Mutex);
    m_DecoderState = STATE_PAUSE;
}

void BaseDecoder::Stop() {
    LOGCATI("BaseDecoder::Stop");
    std::unique_lock<std::mutex> lock(m_Mutex);
    m_DecoderState = STATE_STOP;
    m_Cond.notify_all();
}

void BaseDecoder::StartDecodingThread() {
    m_Thread = new std::thread(DoAVDecoding, this);
}

void BaseDecoder::DoAVDecoding(BaseDecoder *decoder) {
    LOGCATI("BaseDecoder::DoAVDecoding");
    if (decoder->InitDecoder() != 0) {
        return;
    }
    decoder->OnDecoderReady();
    decoder->DecodingLoop();
    decoder->UnInitDecoder();
    decoder->OnDecoderDone();
}

int BaseDecoder::InitDecoder() {
    int result = -1;
    do {
        avformat_network_init();

        // - 创建封装格式上下文
        m_AVFormatContext = avformat_alloc_context();

        // - 打开文件
        if (avformat_open_input(&m_AVFormatContext, m_Url, nullptr, nullptr) != 0) {
            LOGCATE("BaseDecoder::InitDecoder avformat_open_input fail.");
            break;
        }

        // - 获取音视频流信息
        if (avformat_find_stream_info(m_AVFormatContext, nullptr) < 0) {
            LOGCATE("BaseDecoder::InitDecoder avformat_find_stream_info fail.");
            break;
        }

        // - 获取音视频索引
        for (int i = 0; i < m_AVFormatContext->nb_streams; i++) {
            if (m_AVFormatContext->streams[i]->codecpar->codec_type == m_MediaType) {
                m_StreamIndex = i;
                break;
            }
        }

        if (m_StreamIndex == -1) {
            LOGCATE("BaseDecoder::InitDecoder Fail to find stream index.");
            break;
        }

        // - 获取解码器参数
        AVCodecParameters *avCodecParameters = m_AVFormatContext->streams[m_StreamIndex]->codecpar;

        // - 获取解码器
        m_AVCodec = avcodec_find_decoder(avCodecParameters->codec_id);
        if (!m_AVCodec) {
            LOGCATE("BaseDecoder::InitDecoder avcodec_find_decoder fail.");
            break;
        }

        // 创建解码器上下文
        m_AVCodecContext = avcodec_alloc_context3(m_AVCodec);

        if (avcodec_parameters_to_context(m_AVCodecContext, avCodecParameters) != 0) {
            LOGCATE("BaseDecoder::InitDecoder avcodec_parameters_to_context fail.");
            break;
        }

        // 打开解码器
        result = avcodec_open2(m_AVCodecContext, m_AVCodec, nullptr);
        if (result < 0) {
            LOGCATE("BaseDecoder::InitDecoder avcodec_open2 fail. result=%d", result);
            break;
        }
        result = 0;

        m_Packet = av_packet_alloc();
        m_Frame = av_frame_alloc();
    } while (false);
    return result;
}

void BaseDecoder::UnInitDecoder() {
    LOGCATI("BaseDecoder::UnInitDecoder");
    if (m_Frame) {
        av_frame_free(&m_Frame);
        m_Frame = nullptr;
    }

    if (m_Packet) {
        av_packet_free(&m_Packet);
        m_Packet = nullptr;
    }

    if (m_AVCodecContext) {
        avcodec_close(m_AVCodecContext);
        avcodec_free_context(&m_AVCodecContext);
        m_AVCodecContext = nullptr;
        m_AVCodec = nullptr;
    }

    if (m_AVFormatContext) {
        avformat_close_input(&m_AVFormatContext);
        avformat_free_context(m_AVFormatContext);
        m_AVFormatContext = nullptr;
    }
}

// 构造函数调用
int BaseDecoder::Init(const char *url, AVMediaType mediaType) {
    LOGCATI("BaseDecoder::Init url = %s, mediaType = %d", url, mediaType);
    strcpy(m_Url, url);
    m_MediaType = mediaType;
    return 0;
}

// 虚构函数调用
void BaseDecoder::UnInit() {
    LOGCATI("BaseDecoder::UnInit m_MediaType = %d", m_MediaType);
    if (m_Thread) {
        Stop();
        m_Thread->join();
        delete m_Thread;
        m_Thread = nullptr;
    }
}

void BaseDecoder::DecodingLoop() {
    LOGCATI("BaseDecoder::DecodingLoop start, m_MediaType=%d", m_MediaType);
    {
        std::unique_lock<std::mutex> lock(m_Mutex);
        m_DecoderState = STATE_DECODING;
        lock.unlock();
    }

    for (;;) {
        while (m_DecoderState == STATE_PAUSE) {
            std::unique_lock<std::mutex> lock(m_Mutex);
            LOGCATE("BaseDecoder::DecodingLoop waiting, m_MediaType=%d", m_MediaType);
            m_Cond.wait_for(lock, std::chrono::milliseconds(10));
        }

        if (m_DecoderState == STATE_STOP) {
            break;
        }

        if (DecodeOnePacket() != 0) {
            // 解码结束，暂停解码器
            std::unique_lock<std::mutex> lock(m_Mutex);
            m_DecoderState = STATE_PAUSE;
        }
    }
    LOGCATE("DecoderBase::DecodingLoop end");
}

int BaseDecoder::DecodeOnePacket() {
    LOGCATI("BaseDecoder::DecodeOnePacket m_MediaType = %d", m_MediaType);
    int result = av_read_frame(m_AVFormatContext, m_Packet);
    // == 0 ok
    while (result == 0) {
        if (m_Packet->stream_index == m_StreamIndex) {
            if (avcodec_send_packet(m_AVCodecContext, m_Packet) == AVERROR_EOF) {
                // 解码结束
                result = -1;
                goto __EXIT;
            }
            int frameCount = 0;
            while (avcodec_receive_frame(m_AVCodecContext, m_Frame) == 0) {
                OnFrameAvailable(m_Frame);
                frameCount++;
            }
            LOGCATI("BaseDecoder::DecodeOnePacket frameCount = %d", frameCount);
            // 判断一个 packet 是否解码完成
            if (frameCount > 0) {
                result = 0;
                goto __EXIT;
            }
        }
        av_packet_unref(m_Packet);
        result = av_read_frame(m_AVFormatContext, m_Packet);
    }
    __EXIT:
    av_packet_unref(m_Packet);
    return result;
}
