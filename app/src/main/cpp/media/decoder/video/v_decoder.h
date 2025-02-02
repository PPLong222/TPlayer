//
// Created by 赵云龙 on 1/23/25.
//

#ifndef TPLAYER_V_DECODER_H
#define TPLAYER_V_DECODER_H

#include "../base_decoder.h"
#include "../../render/video/video_render.h"

extern "C" {
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
};

class VideoDecoder : public BaseDecoder {
private:
    const char *TAG = "VideoDecoder";

    const AVPixelFormat DST_FORMAT = AV_PIX_FMT_RGBA;

    AVFrame *m_rgb_frame = nullptr;

    uint8_t  *m_buf_for_rgba_frame = nullptr;

    SwsContext *m_sws_ctx = nullptr;

    VideoRender *m_video_render = nullptr;

    int m_dst_w;
    int m_dst_h;

    void InitRender(JNIEnv *env);
    void InitBuffer();
    void InitSws();
public:
    VideoDecoder(JNIEnv *env, jstring path, bool for_synthesizer = false);
    ~VideoDecoder() override;
    void SetRender(VideoRender *render);

    AVMediaType GetMediaType() override;
protected:


//    bool NeedLoopDecode() override;

    void Prepare(JNIEnv *env) override;

    void Render(AVFrame *frame) override;

    void Release() override;
};


#endif //TPLAYER_V_DECODER_H
