//
// Created by 赵云龙 on 2/5/25.
//

#ifndef TPLAYER_OPENGL_PLAYER_H
#define TPLAYER_OPENGL_PLAYER_H


#include "../decoder/video/v_decoder.h"
#include "../render/video/opengl/opengl_render.h"
#include "../../opengl/drawer/video_drawer.h"
#include "../decoder/audio/a_decoder.h"

class GLPlayer {
private:
    std::shared_ptr<VideoDecoder> m_v_decoder;
    std::shared_ptr<OpenGLRender> m_gl_render;
    DrawerProxy *m_v_drawer_proxy;
    VideoDrawer *m_v_drawer;

    std::shared_ptr<AudioDecoder> m_a_decoder;
    AudioRender *m_a_render;
public:
    GLPlayer(JNIEnv *jniEnv, jstring path, jobject surface);

    ~GLPlayer();

    void Play();

    void Release();

    void Pause();

    void Resume();

    long GetDuration();

    long GetCurPos();

    void Seek(long targetTimeMs);
};


#endif //TPLAYER_OPENGL_PLAYER_H
