//
// Created by 赵云龙 on 1/24/25.
//

#include "player.h"
#include "../render/video/native_render.h"
#include "../render/audio/open_sl_render.h"
#include <unistd.h>

Player::Player(JNIEnv *env, jstring path, jobject surface) {
    m_v_decoder = std::make_shared<VideoDecoder>(env, path);
    m_v_render = new NativeRender(env, surface);
    m_v_decoder->SetRender(m_v_render);
    m_v_decoder->CreateDecodeThread();

    m_a_decoder = std::make_shared<AudioDecoder>(env, path, false);
    m_a_render = new OpenSLRender();
    m_a_decoder->SetRender(m_a_render);
    m_a_decoder->CreateDecodeThread();
}

Player::~Player() {

}

void Player::play() {

    if (m_a_decoder != nullptr) {
        m_v_decoder->GoOn();
        m_a_decoder->GoOn();

    }
}

void Player::pause() {
    if (m_v_decoder != nullptr) {
        m_v_decoder->Pause();
        m_a_decoder->Pause();
    }
}

void Player::resume() {
    m_v_decoder->GoOn();
    m_a_decoder->GoOn();
}

long Player::GetDuration() {
    return m_v_decoder->GetDuration();
}

long Player::GetCurPos() {
    return m_a_decoder->GetCurPos();
}

void Player::seek(long targetTimeMs) {
    m_v_decoder->targetPos = targetTimeMs;
    m_a_decoder->targetPos = targetTimeMs;
    LOGI("Player", "Target: %ld", targetTimeMs);
}
