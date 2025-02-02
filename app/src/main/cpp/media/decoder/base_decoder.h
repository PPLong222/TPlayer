//
// Created by 赵云龙 on 1/23/25.
//

#ifndef TPLAYER_BASE_DECODER_H
#define TPLAYER_BASE_DECODER_H

#include <jni.h>
#include <pthread.h>
#include <__memory/shared_ptr.h>
#include "i_decoder.h"
#include "../decode_state.h"

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>

};


class BaseDecoder : public IDecoder, public std::enable_shared_from_this<BaseDecoder> {
private:
    const char *TAG = "BaseDecoder";

    AVFormatContext *m_format_ctx = nullptr;
    const AVCodec *m_codec = nullptr;
    AVCodecContext *m_codec_ctx = nullptr;
    AVPacket *m_packet = nullptr;
    AVFrame *m_frame = nullptr;

    DecodeState m_state = STOP;
    int m_stream_index = -1;
    int64_t m_cur_t_s = 0;
    long m_duration = 0;
    int64_t m_started_t = -1;
    bool m_for_synthesizer = false;

    void InitFFmpegDecoder(JNIEnv *env);

    void AllocFrameBuffer();

    void LoopDecode();

    void ObtainTimeStamp();

    void DoneDecode(JNIEnv *env);

    void SyncRender();

    void LogVideoInfo();

    // Thread
    JavaVM *m_jvm_for_thread = nullptr;
    jobject m_path_ref = nullptr;
    const char *m_path = nullptr;

    pthread_mutex_t m_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t m_cond = PTHREAD_COND_INITIALIZER;

    void Decode();


public:
    BaseDecoder(JNIEnv *env, jstring path, bool for_synthesizer);

    virtual ~BaseDecoder();

    void GoOn() override;

    void Pause() override;

    void Stop() override;

    bool IsRunning() override;

    long GetDuration() override;

    long GetCurPos() override;

    virtual AVMediaType GetMediaType() = 0;

    void CreateDecodeThread();

    int width() {
        return m_codec_ctx->width;
    }

    int height() {
        return m_codec_ctx->height;
    }

    long duration() {
        return m_duration;
    }

protected:
    void Wait(long second = 0, long ms = 0);

    void SendSignal();

    virtual void Prepare(JNIEnv *env) = 0;

    virtual void Render(AVFrame *frame) = 0;

    virtual void Release() = 0;

    void Init(JNIEnv *pEnv, jstring pJstring);

    AVFrame *DecodeOneFrame();

    AVPixelFormat video_pixel_format() {
        return m_codec_ctx->pix_fmt;
    }

    AVRational time_base() {
        return m_format_ctx->streams[m_stream_index]->time_base;
    }


};


#endif //TPLAYER_BASE_DECODER_H
