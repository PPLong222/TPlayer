//
// Created by 赵云龙 on 2/5/25.
//

#include "opengl_player.h"
#include "../../opengl/drawer/proxy/drawer_proxy_impl.h"
#include "../render/audio/open_sl_render.h"

GLPlayer::GLPlayer(JNIEnv *jniEnv, jstring path, jobject surface) {
    m_v_decoder = std::make_shared<VideoDecoder>(jniEnv, path);
    m_v_drawer = new VideoDrawer();
    m_v_decoder->SetRender(m_v_drawer);
    auto *proxyImpl = new DrawerProxyImpl();
    proxyImpl->AddDrawer(m_v_drawer);


    m_v_drawer_proxy = proxyImpl;

    m_gl_render = std::make_shared<OpenGLRender>(jniEnv, m_v_drawer_proxy);
    m_gl_render->SetSurface(surface);
    m_gl_render->InitRenderThread();
    m_v_decoder->CreateDecodeThread();

    m_a_decoder = std::make_shared<AudioDecoder>(jniEnv, path, false);
    m_a_render = new OpenSLRender();
    m_a_decoder->SetRender(m_a_render);
    m_a_decoder->CreateDecodeThread();
}

GLPlayer::~GLPlayer() {

}

void GLPlayer::Play() {
    if (m_v_decoder != nullptr) {
        m_v_decoder->GoOn();
    }
    if (m_a_decoder != nullptr) {
        m_a_decoder->GoOn();
    }
}

void GLPlayer::Release() {
    m_gl_render->Stop();
    m_v_decoder->Stop();
    m_a_decoder->Stop();
}

void GLPlayer::Pause() {
    if (m_v_decoder != nullptr) {
        m_v_decoder->Pause();
    }
    if (m_a_decoder != nullptr) {
        m_a_decoder->Pause();
    }
}

void GLPlayer::Resume() {
    if (m_v_decoder != nullptr) {
        m_v_decoder->GoOn();
    }
    if (m_a_decoder != nullptr) {
        m_a_decoder->GoOn();
    }
}

long GLPlayer::GetDuration() {
    return m_v_decoder->GetDuration();
}

long GLPlayer::GetCurPos() {
    return m_v_decoder->GetCurPos();
}

void GLPlayer::Seek(long targetTimeMs) {
    m_v_decoder->targetPos = targetTimeMs;
    m_a_decoder->targetPos = targetTimeMs;
    LOGI("Player", "Target: %ld", targetTimeMs);
}
