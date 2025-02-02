//
// Created by 赵云龙 on 1/24/25.
//

#ifndef TPLAYER_PLAYER_H
#define TPLAYER_PLAYER_H


#include "../decoder/video/v_decoder.h"

class Player {
private:
    std::shared_ptr<VideoDecoder> m_v_decoder;
    VideoRender *m_v_render;

public:
    Player(JNIEnv *env, jstring path, jobject surface);
    ~Player();

    void play();
    void pause();
};


#endif //TPLAYER_PLAYER_H
