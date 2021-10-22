#ifndef BASEDECODER_H
#define BASEDECODER_H

#include <thread>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libavutil/time.h>
#include <libavcodec/jni.h>
};

#define MAX_PATH   2048

enum DecoderState {
    STATE_UNKNOWN,
    STATE_DECODING,
    STATE_PAUSE,
    STATE_STOP
};

enum DecoderMsg {
    MSG_DECODER_READY
};

typedef void (*MessageCallback)(void *, int, float);

class BaseDecoder {
public:
    BaseDecoder() = default;

    virtual ~BaseDecoder() = default;

    virtual void Start();

    virtual void Pause();

    virtual void Stop();

    virtual void SetMessageCallback(void *context, MessageCallback callback);

protected:
    virtual int Init(const char *url, AVMediaType mediaType);

    virtual void UnInit();

    virtual void OnDecoderReady() = 0;

    virtual void OnDecoderDone() = 0;

    virtual void OnFrameAvailable(AVFrame *frame) = 0;

    AVCodecContext *GetCodecContext();

    void *m_MsgContext = nullptr;

    MessageCallback m_MsgCallback = nullptr;

private:
    std::thread *m_Thread = nullptr;

    char m_Url[MAX_PATH] = {0};

    AVMediaType m_MediaType = AVMEDIA_TYPE_UNKNOWN;

    void StartDecodingThread();

    static void DoAVDecoding(BaseDecoder *decoder);

    int InitDecoder();

    void UnInitDecoder();

    void DecodingLoop();

    int DecodeOnePacket();

    volatile int m_DecoderState = STATE_UNKNOWN;

    std::mutex m_Mutex;

    std::condition_variable m_Cond;

    AVFormatContext *m_AVFormatContext = nullptr;

    int m_StreamIndex = -1;

    const AVCodec *m_AVCodec = nullptr;

    AVCodecContext *m_AVCodecContext = nullptr;

    // 编码的数据包
    AVPacket *m_Packet = nullptr;
    // 解码的帧
    AVFrame *m_Frame = nullptr;
};


#endif
