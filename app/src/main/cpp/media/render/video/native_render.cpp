//
// Created by 赵云龙 on 1/24/25.
//
#include <cstring>
#include "native_render.h"

extern "C" {
#include <libavutil/mem.h>
}


NativeRender::NativeRender(JNIEnv *env, jobject surface) {
    m_surface_ref = env->NewGlobalRef(surface);
}

NativeRender::~NativeRender() {

}

void NativeRender::InitRender(JNIEnv *env, int video_width, int video_height, int *dst_size) {
    m_native_window = ANativeWindow_fromSurface(env, m_surface_ref);

    int windowWidth = ANativeWindow_getWidth(m_native_window);
    int windowHeight = ANativeWindow_getHeight(m_native_window);
    LOGI(TAG, "Surface window width: %d, height: %d", windowWidth, windowHeight)
    LOGI(TAG, "Video width: %d, height: %d", video_width, video_height)

    m_dst_w = windowWidth;
    // dst height based on video size
    m_dst_h = m_dst_w * video_height / video_width;

    // make sure dst size not go beyond window size
    if (m_dst_h > windowHeight) {
        m_dst_h = windowHeight;
        m_dst_w = windowHeight * video_width / video_height;
    }

    ANativeWindow_setBuffersGeometry(m_native_window, windowWidth, windowHeight, WINDOW_FORMAT_RGBA_8888);

    dst_size[0] = m_dst_w;
    dst_size[1] = m_dst_h;
}

void NativeRender::Render(OneFrame *one_frame) {
    LOGI(TAG, "----------------Begin Render-----------------")
    ANativeWindow_lock(m_native_window, &m_out_buffer, nullptr);
    uint8_t *dst = (uint8_t *)m_out_buffer.bits;
    int dstStride = m_out_buffer.stride * 4;
    int srcStride = one_frame->line_size;

    for (int h = 0; h < m_dst_h; h++) {
        memcpy(dst + h * dstStride, one_frame->data + h * srcStride, srcStride);
    }

    ANativeWindow_unlockAndPost(m_native_window);
    LOGI(TAG, "----------------End Render-----------------")
}

void NativeRender::ReleaseRender() {
    if (m_native_window != NULL) {
        ANativeWindow_release(m_native_window);
    }
    av_free(&m_out_buffer);
}