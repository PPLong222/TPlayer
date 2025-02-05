//
// Created by 赵云龙 on 2/5/25.
//

#ifndef TPLAYER_VIDEO_DRAWER_H
#define TPLAYER_VIDEO_DRAWER_H


#include "drawer.h"
#include "../egl/egl_surface.h"
#include "../../media/render/video/video_render.h"

class VideoDrawer: public Drawer, public VideoRender {
public:

    VideoDrawer();
    ~VideoDrawer();

    void InitRender(JNIEnv *env, int video_width, int video_height, int *dst_size) override ;
    void Render(OneFrame *one_frame) override ;
    void ReleaseRender() override ;

    const char* GetVertexShader() override;
    const char* GetFragmentShader() override;
    void InitCstShaderHandler() override;
    void BindTexture() override;
    void PrepareDraw() override;
    void DoneDraw() override;
};



#endif //TPLAYER_VIDEO_DRAWER_H
