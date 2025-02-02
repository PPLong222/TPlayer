//
// Created by 赵云龙 on 1/24/25.
//

#ifndef TPLAYER_VIDEO_RENDER_H
#define TPLAYER_VIDEO_RENDER_H

#include <jni.h>
#include <cstdint>
#include "../../one_frame.h"


class VideoRender {
public:
    virtual void InitRender(JNIEnv *env, int video_width, int video_height, int *dst_size) = 0;
    virtual void Render(OneFrame *one_frame) = 0;
    virtual void ReleaseRender() = 0;

protected:
    const char* TAG = "VideoRender";
};
#endif //TPLAYER_VIDEO_RENDER_H
