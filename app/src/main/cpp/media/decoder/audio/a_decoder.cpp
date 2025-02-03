//
// Created by 赵云龙 on 2/1/25.
//

#include "a_decoder.h"
#include "../../render/audio/audio_render.h"
#include "../../const.h"
#include "../../../util/logger.h"

extern "C" {
#include <libavutil/opt.h>
}


AudioDecoder::AudioDecoder(JNIEnv *env, const jstring path, bool forSynthesizer) : BaseDecoder(env,
                                                                                               path,
                                                                                               forSynthesizer) {

}

AudioDecoder::~AudioDecoder() {
    if (m_render != nullptr) {
        delete m_render;
    }
}

void AudioDecoder::SetRender(AudioRender *render) {
    m_render = render;
}

void AudioDecoder::Prepare(JNIEnv *env) {
    InitSwr();
    InitOutBuffer();
    InitRender();
}

void AudioDecoder::InitSwr() {
    AVCodecContext *codecContext = codec_cxt();

    m_swr_ctx = swr_alloc();

    int res = av_opt_set_chlayout(m_swr_ctx, "in_chlayout", &codecContext->ch_layout, 0);
    LOGI(TAG, "RES: %d", res)
    AVChannelLayout out_layout = AV_CHANNEL_LAYOUT_STEREO;
    av_channel_layout_from_mask(&out_layout, AV_CH_LAYOUT_STEREO);
    // Another init method:
    // av_channel_layout_default(&out_layout, 2);

    // this could lead to some problem
    //    res = swr_alloc_set_opts2(&m_swr_ctx, &out_layout, AV_SAMPLE_FMT_S16, 44100,
    //                        &codecContext->ch_layout, codecContext->sample_fmt,
    //                        codecContext->sample_rate, 0,
    //                        nullptr);
    LOGI(TAG, "RES: %d", res)
    res = av_opt_set_chlayout(m_swr_ctx, "out_chlayout", &out_layout, 0);
    LOGI(TAG, "RES: %d", res)
    res = av_opt_set_int(m_swr_ctx, "in_sample_rate", codecContext->sample_rate, 0);
    LOGI(TAG, "RES: %d", res)
    // This should be same with SLDataFormat_PCM setting
    res = av_opt_set_int(m_swr_ctx, "out_sample_rate", ENCODE_AUDIO_DEST_SAMPLE_RATE, 0);
    LOGI(TAG, "RES: %d", res)
    res = av_opt_set_sample_fmt(m_swr_ctx, "in_sample_fmt", codecContext->sample_fmt, 0);
    LOGI(TAG, "RES: %d", res)
    res = av_opt_set_sample_fmt(m_swr_ctx, "out_sample_fmt", GetSampleFmt(), 0);
    LOGI(TAG, "RES: %d", res)

    LOGI(TAG, "sample rate: %d, format: %d nb_channels: %d",
         codecContext->sample_rate, codecContext->sample_fmt, codecContext->ch_layout.nb_channels)
    swr_init(m_swr_ctx);
}

void AudioDecoder::InitOutBuffer() {
    m_dest_nb_sample = (int) av_rescale_rnd(
            swr_get_delay(m_swr_ctx, codec_cxt()->sample_rate) + ACC_NB_SAMPLES,
            GetSampleRate(codec_cxt()->sample_rate),
            codec_cxt()->sample_rate, AV_ROUND_UP);
    AVChannelLayout out_layout = AV_CHANNEL_LAYOUT_STEREO;
    av_channel_layout_from_mask(&out_layout, AV_CH_LAYOUT_STEREO);
    // FATAL ERROR here: Using AV_CH_LAYOUT_STEREO to indicate nb_channels, which is totally wrong and will lead to play error.
    m_dest_data_size = av_samples_get_buffer_size(nullptr,
                                                           out_layout.nb_channels,
                                                           m_dest_nb_sample, GetSampleFmt(), 1);

    m_out_buffer[0] = (uint8_t *) malloc(m_dest_data_size);
    LOGI(TAG, "InitOutBuffer: dest_nb_samples is: %d, dest_data_size is: %d", m_dest_nb_sample,
         m_dest_data_size);
}

void AudioDecoder::InitRender() {
    LOGI(TAG, "Init Render");
    m_render->InitRender();
}


void AudioDecoder::Render(AVFrame *frame) {
    LOGI(TAG, "m_dest_data_size: %d", m_dest_data_size)
    int suffix = ENCODE_AUDIO_DEST_CHANNEL_COUNTS * sizeof(uint16_t);
    int ret = swr_convert(m_swr_ctx, m_out_buffer, m_dest_data_size / suffix,
                          (const uint8_t **) frame->data,
                          frame->nb_samples);
    LOGI(TAG, "Render ret: %d", ret)
    if (ret > 0) {
        m_render->Render(m_out_buffer[0], (ret * suffix));
    }
}

void AudioDecoder::Release() {
    if (m_swr_ctx != nullptr) {
        swr_free(&m_swr_ctx);
    }
    if (m_render != nullptr) {
        m_render->ReleaseRender();
    }
    ReleaseOutBuffer();
}

void AudioDecoder::ReleaseOutBuffer() {
    if (m_out_buffer[0] != nullptr) {
        free(m_out_buffer[0]);
        m_out_buffer[0] = nullptr;

    }
}

bool AudioDecoder::NeedLoopDecode() {
    return true;
}

int AudioDecoder::GetSampleRate(int spr) {
    return spr;
}

AVSampleFormat AudioDecoder::GetSampleFmt() {
    return AV_SAMPLE_FMT_S16;
}

AVMediaType AudioDecoder::GetMediaType() {
    return AVMEDIA_TYPE_AUDIO;
}



