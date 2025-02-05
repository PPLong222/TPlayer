//
// Created by 赵云龙 on 1/23/25.
//

#include "v_decoder.h"
#include "../util/logger.h"

extern "C" {
#include <libavutil/time.h>
}


VideoDecoder::VideoDecoder(JNIEnv *env, jstring path,
                           bool for_synthesizer) : BaseDecoder(env, path, for_synthesizer)  {

}

void VideoDecoder::Prepare(JNIEnv *env) {
    InitRender(env);
    InitBuffer();
    InitSws();
}

void VideoDecoder::InitBuffer() {
    m_rgb_frame = av_frame_alloc();
    int numBytes = av_image_get_buffer_size(DST_FORMAT, m_dst_w, m_dst_h, 1);
    m_buf_for_rgba_frame = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(m_rgb_frame->data, m_rgb_frame->linesize, m_buf_for_rgba_frame, DST_FORMAT,
                         m_dst_w, m_dst_h, 1);
}

void VideoDecoder::InitSws() {
    LOGI(TAG, "Init SWS: current srcFormat: %s", av_get_pix_fmt_name(video_pixel_format()))
    m_sws_ctx = sws_getContext(width(), height(), video_pixel_format(),
                               m_dst_w, m_dst_h, DST_FORMAT, SWS_FAST_BILINEAR, nullptr, nullptr,
                               nullptr);
}

void VideoDecoder::Release() {
    if (m_rgb_frame != nullptr) {
        av_frame_free(&m_rgb_frame);
        m_rgb_frame = nullptr;
    }
    if (m_buf_for_rgba_frame != nullptr) {
        free(m_buf_for_rgba_frame);
        m_buf_for_rgba_frame = nullptr;
    }
    if (m_sws_ctx != nullptr) {
        sws_freeContext(m_sws_ctx);
        m_sws_ctx = nullptr;
    }
    if (m_video_render != nullptr) {
        m_video_render->ReleaseRender();
        m_video_render = nullptr;
    }
}

void VideoDecoder::SetRender(VideoRender *render) {
    m_video_render = render;
}

void VideoDecoder::InitRender(JNIEnv *env) {
    if (m_video_render != nullptr) {
        int dst_size[2] = {-1, -1};
        m_video_render->InitRender(env, width(), height(), dst_size);

        m_dst_w = dst_size[0];
        m_dst_h = dst_size[1];

        if (m_dst_w == -1) {
            m_dst_w = width();
        }
        if (m_dst_h == -1) {
            // TODO is this right?
            m_dst_w = height();
        }
    } else {

    }
}

void VideoDecoder::Render(AVFrame *frame) {
    // convert ratio and format(yuv->rgb)
    // TODO: question, how does it convert?
    sws_scale(m_sws_ctx, frame->data, frame->linesize, 0, height(), m_rgb_frame->data,
              m_rgb_frame->linesize);
    LOGI("TAG", "frame linesize: %lu, frame height: %d", sizeof(frame->linesize), height())

    OneFrame *one_frame = new OneFrame(m_rgb_frame->data[0], m_rgb_frame->linesize[0], frame->pts,
                                       time_base(),
                                       nullptr, false);
    m_video_render->Render(one_frame);

//    LOGD("TAG", "Render: Render Before Wait");
//    Wait(0, 200);
//    LOGD("TAG", "Render: Render After Wait");
}

VideoDecoder::~VideoDecoder() {
    delete m_video_render;
}

AVMediaType VideoDecoder::GetMediaType() {
    return AVMEDIA_TYPE_VIDEO;
}

bool VideoDecoder::NeedLoopDecode() {
    return true;
}
