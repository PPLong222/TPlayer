//
// Created by 赵云龙 on 2/5/25.
//

#ifndef TPLAYER_OPENGL_PIXEL_RECEIVER_H
#define TPLAYER_OPENGL_PIXEL_RECEIVER_H

#include <cstdint>

class OpenGLPixelReceiver {
public:
    virtual void ReceiverPixel(uint8_t *rgba) = 0;
};

#endif //TPLAYER_OPENGL_PIXEL_RECEIVER_H
