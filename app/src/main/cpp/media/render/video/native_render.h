//
// Created by 赵云龙 on 1/24/25.
//

#ifndef TPLAYER_NATIVE_RENDER_H
#define TPLAYER_NATIVE_RENDER_H

#include "video_render.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>

class NativeRender : public VideoRender {
private:
    jobject m_surface_ref = nullptr;
    ANativeWindow_Buffer m_out_buffer;
    ANativeWindow *m_native_window = nullptr;
    int m_dst_w;
    int m_dst_h;

public:
    NativeRender(JNIEnv *env, jobject surface);

    ~NativeRender();

    void InitRender(JNIEnv *env, int video_width, int video_height, int *dst_size) override;

    void Render(OneFrame *one_frame) override;

    void ReleaseRender() override;
};

#endif //TPLAYER_NATIVE_RENDER_H
