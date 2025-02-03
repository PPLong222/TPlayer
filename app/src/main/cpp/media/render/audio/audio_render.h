//
// Created by 赵云龙 on 2/1/25.
//

#ifndef TPLAYER_AUDIO_RENDER_H
#define TPLAYER_AUDIO_RENDER_H

#include <cstdint>

class AudioRender {
public:
    virtual void InitRender() = 0;
    virtual void Render(uint8_t *pcm, int size) = 0;
    virtual void ReleaseRender() = 0;
    virtual ~AudioRender() = default;
};
#endif //TPLAYER_AUDIO_RENDER_H
