//
// Created by 赵云龙 on 1/24/25.
//

#include "player.h"
#include "../render/video/native_render.h"
#include <unistd.h>

Player::Player(JNIEnv *env, jstring path, jobject surface) {
    m_v_decoder = std::make_shared<VideoDecoder>(env, path);
    m_v_render = new NativeRender(env, surface);
    m_v_decoder->SetRender(m_v_render);
    m_v_decoder->CreateDecodeThread();
}

Player::~Player() {

}

void Player::play() {

    if (m_v_decoder != nullptr) {
        m_v_decoder->GoOn();
    }
}

void Player::pause() {
    if (m_v_decoder != nullptr) {
        m_v_decoder->Pause();
    }
}
