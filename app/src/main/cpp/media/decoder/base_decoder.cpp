//
// Created by 赵云龙 on 1/23/25.
//

#include "base_decoder.h"
#include <thread>
#include <android/log.h>
#include "../decode_state.h"
#include "../util/logger.h"
#include "../util/time.h"
#include <unistd.h>

extern "C" {
#include <libavutil/time.h>
}



BaseDecoder::BaseDecoder(JNIEnv *env, jstring path, bool for_synthesizer): m_for_synthesizer(for_synthesizer) {
    Init(env, path);
}

BaseDecoder::~BaseDecoder() {
    LOGE(TAG, "BaseDecoder removed")
    pthread_mutex_destroy(&m_mutex);
    pthread_cond_destroy(&m_cond);
}

void BaseDecoder::Init(JNIEnv *env, jstring path) {
    m_path_ref = env->NewGlobalRef(path);
    m_path = env->GetStringUTFChars(path, nullptr);

    env->GetJavaVM(&m_jvm_for_thread);
}

void BaseDecoder::CreateDecodeThread() {
    // what is it?
    auto self = shared_from_this();
    std::thread t(&BaseDecoder::Decode, self);
    t.detach();
}

void BaseDecoder::Decode() {
    JNIEnv *env;
    if (m_jvm_for_thread->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        LOGE("SSS", "Fail to init decode thread");
    }

    InitFFmpegDecoder(env);
    LogVideoInfo();
    AllocFrameBuffer();
    av_usleep(1000);
    Prepare(env);
    LoopDecode();
    DoneDecode(env);

    m_jvm_for_thread->DetachCurrentThread();
}

void BaseDecoder::InitFFmpegDecoder(JNIEnv *env) {
    m_format_ctx = avformat_alloc_context();

    if (avformat_open_input(&m_format_ctx, m_path, nullptr, nullptr) != 0) {
        LOGE(TAG, "Fail to open file");
        DoneDecode(env);
        return;
    }

    if (avformat_find_stream_info(m_format_ctx, nullptr) < 0) {
        LOGE(TAG, "Fail to find stream info");
        DoneDecode(env);
        return;
    }

    int vIdx = -1;
    LOGI(TAG, "Before GetMedia")
    for (int i = 0; i < m_format_ctx->nb_streams; i++) {
        if (m_format_ctx->streams[i]->codecpar->codec_type == GetMediaType()) {
            vIdx = i;
            break;
        }
    }
    LOGI(TAG, "After GetMedia")
    if (vIdx == -1) {
        LOGE(TAG, "Fail to find stream index");
        DoneDecode(env);
        return;
    }

    m_stream_index = vIdx;

    AVCodecParameters *codecPar = m_format_ctx->streams[vIdx]->codecpar;

    m_codec = avcodec_find_decoder(codecPar->codec_id);

    m_codec_ctx = avcodec_alloc_context3(m_codec);
    if (avcodec_parameters_to_context(m_codec_ctx, codecPar) != 0) {
        LOGE(TAG, "Fail to obtain av codec context");
        DoneDecode(env);
        return;
    }

    if (avcodec_open2(m_codec_ctx, m_codec, nullptr) < 0) {
        LOGE(TAG, "Fail to open ava codec");
        DoneDecode(env);
        return;
    }

    m_duration = (m_format_ctx->duration / AV_TIME_BASE * 1000);
}

void BaseDecoder::AllocFrameBuffer() {
    m_packet = av_packet_alloc();
    m_frame = av_frame_alloc();
}

void BaseDecoder::LoopDecode() {
    LOGD(TAG, "Begin LoopDecode")
    if (STOP == m_state) {
        m_state = START;
    }

    while (true) {
        LOGD(TAG, "LoopDecode Current State %s", DecodeStateToString(m_state))
        if (m_state != DECODING &&
            m_state != START &&
            m_state != STOP) {
            Wait();
            // TODO: Why not use usec?
            m_started_t = GetCurMsTime() - m_cur_t_s;
        }

        if (m_state == STOP) {
            break;
        }

        if (m_started_t == -1) {
            m_started_t = GetCurMsTime();
            LOGI(TAG, "Init Start Time: %ld", m_started_t);
        }

        LOGI(TAG, "Try DecodeOneFrame");
        if (DecodeOneFrame() != nullptr) {
            LOGI(TAG, "Try SyncRender");
            // let render render the frame in right time(key to keep fps)
            SyncRender();
            LOGI(TAG, "Try Render");
            Render(m_frame);

            // Only demux first frame if init player(not play)
            if (m_state == START) {
                m_state = PAUSE;
            }
            LOGD(TAG, "Render: LoopDecode: Current State %u", m_state)
        } else {
            LOGI(TAG, "Try DecodeOneFrame Failed");
            // TODO: for synthesizer
            if (0) {

            } else {
                m_state = FINISH;
            }
        }
    }
}

AVFrame *BaseDecoder::DecodeOneFrame() {
    LOGI(TAG, "Begin DecodeOneFrame")
    // read next packet from format_context
    int ret = av_read_frame(m_format_ctx, m_packet);
    while (ret == 0) {
        if (m_packet->stream_index == m_stream_index) {
            // get frame based on packet and also codec(that's where the demux really works)
            switch (avcodec_send_packet(m_codec_ctx, m_packet)) {
                case AVERROR_EOF: {
                    av_packet_unref(m_packet);
                    return nullptr;
                }
                case AVERROR(EAGAIN): {
                    av_packet_unref(m_packet);
                    return nullptr;
                }
                case AVERROR(EINVAL): {
                    av_packet_unref(m_packet);
                    return nullptr;
                }
                case AVERROR(ENOMEM): {
                    av_packet_unref(m_packet);
                    return nullptr;
                }
                default:
                    break;
            }
            // assign demux result to m_frame
            int result = avcodec_receive_frame(m_codec_ctx, m_frame);
            if (result == 0) {
                LOGD(TAG, "Receive frame success");
                ObtainTimeStamp();
                av_packet_unref(m_packet);
                return m_frame;
            } else {
                LOGE(TAG, "Receive frame error");
            }
        }
        av_packet_unref(m_packet);
        ret = av_read_frame(m_format_ctx, m_packet);
    }
    av_packet_unref(m_packet);

    return nullptr;
}

void BaseDecoder::DoneDecode(JNIEnv *env) {
    if (m_packet != nullptr) {
        av_packet_free(&m_packet);
    }
    if (m_frame != nullptr) {
        av_frame_free(&m_frame);
    }
    if (m_codec_ctx != nullptr) {
        avcodec_free_context(&m_codec_ctx);
    }
    if (m_format_ctx != nullptr) {
        avformat_close_input(&m_format_ctx);
        avformat_free_context(m_format_ctx);
    }
    if (m_path != nullptr && m_path_ref != nullptr) {
        env->ReleaseStringUTFChars((jstring) m_path_ref, m_path);
        env->DeleteGlobalRef(m_path_ref);
    }

    Release();
}

void BaseDecoder::Wait(long second, long ms) {
    LOGD(TAG, "Enter Function Wait:", m_state)
    int lockResult = pthread_mutex_lock(&m_mutex);
    if (lockResult != 0) {
        LOGE(TAG, "pthread_mutex_lock failed with error %d", lockResult);
        return;
    }
    if (second > 0 || ms > 0) {
        timeval now{};
        timespec outtime{};
        gettimeofday(&now, nullptr);
        int64_t destNSec = now.tv_usec * 1000 + ms * 1000000;
        outtime.tv_sec = static_cast<__kernel_time_t>(now.tv_sec + second + destNSec / 1000000000);
        outtime.tv_nsec = static_cast<long>(destNSec % 1000000000);
        pthread_cond_timedwait(&m_cond, &m_mutex, &outtime);
    } else {
        pthread_cond_wait(&m_cond, &m_mutex);
    }
    pthread_mutex_unlock(&m_mutex);
    LOGI(TAG, "End of Wait")
}

void BaseDecoder::SendSignal() {
    LOGI(TAG, "SendSignal");
    pthread_mutex_lock(&m_mutex);
    pthread_cond_signal(&m_cond);
    pthread_mutex_unlock(&m_mutex);
}


void BaseDecoder::GoOn() {
    m_state = DECODING;
    SendSignal();
}

void BaseDecoder::Pause() {
    m_state = PAUSE;
}

void BaseDecoder::Stop() {
    m_state = STOP;
    SendSignal();
}

bool BaseDecoder::IsRunning() {
    return DECODING == m_state;
}

long BaseDecoder::GetDuration() {
    return m_duration;
}

long BaseDecoder::GetCurPos() {
    return (long)m_cur_t_s;
}

void BaseDecoder::ObtainTimeStamp() {
    LOGI(TAG, "ObtainTimeStamp m_packet->dts %ld  m_frame->pts: %ld", m_packet->dts, m_frame->pts);
    if (m_frame->pts != AV_NOPTS_VALUE) {
        m_cur_t_s = m_frame->pts;
    } else if(m_frame->pkt_dts != AV_NOPTS_VALUE) {
        // normally won't go there
        m_cur_t_s = m_packet->dts;
    } else {
        m_cur_t_s = 0;
    }
    m_cur_t_s = (int64_t)((m_cur_t_s * av_q2d(m_format_ctx->streams[m_stream_index]->time_base)) * 1000);
    LOGI(TAG, "ObtainTimeStamp m_cur_t_s: %ld", m_cur_t_s)
}

void BaseDecoder::SyncRender() {
    int64_t ct = GetCurMsTime();
    int64_t passTime = ct - m_started_t;
    if (m_cur_t_s > passTime) {
        // TODO: maybe it's better using usec for m_cur_t_s
        av_usleep((unsigned int)((m_cur_t_s - passTime) * 1000));
        LOGI(TAG, "SyncRender: need sleep: %u us", (unsigned int)((m_cur_t_s - passTime) * 1000))
    } else {
        LOGI(TAG, "SyncRender: no need to sleep, render right now")
    }
}

void BaseDecoder::LogVideoInfo() {
    LOGI(TAG, "--------------Enter Function: LogVideoInfo----------------");
    LOGI(TAG, "Streams number: %d", m_format_ctx->nb_streams)
    double duration = m_format_ctx->duration / AV_TIME_BASE;
    LOGI(TAG, "Media duration: %f s", duration)
    double start_time = m_format_ctx->start_time / AV_TIME_BASE;
    LOGI(TAG, "Media start time: %f s", start_time)
    LOGI(TAG, "Media bit-rate: %ld bit/s  %f B/s  %f KB/s", m_format_ctx->bit_rate, m_format_ctx->bit_rate / 8.0, m_format_ctx->bit_rate / 8.0 / 1024)
    LOGI(TAG, "Media format: %s", m_format_ctx->iformat->name)

    AVDictionary* dict = m_format_ctx->metadata;
    AVDictionaryEntry* entry = nullptr;
    while((entry = av_dict_get(dict, "", entry, AV_DICT_IGNORE_SUFFIX))) {
        LOGI("Metadata: Key: %s, Value: %s", entry->key, entry->value)
    }

    for (int i = 0; i < m_format_ctx->nb_streams; i++) {
        AVStream* stream = m_format_ctx->streams[i];
        AVCodecParameters* codecPar = stream->codecpar;

        if (codecPar->codec_type == AVMEDIA_TYPE_VIDEO) {
            LOGI(TAG, "Stream[%d] is a Video Stream", i)
            AVRational frame_rate = stream->avg_frame_rate;
            if (frame_rate.num != 0 && frame_rate.den != 0) {
                double fps = av_q2d(frame_rate);
                LOGI(TAG, "Stream[%d] is %f", i, fps)
            }
        }
    }





    LOGI(TAG, "--------------Exit Function: LogVideoInfo----------------")
}
