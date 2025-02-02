//
// Created by 赵云龙 on 1/23/25.
//

#ifndef TPLAYER_I_DECODER_H
#define TPLAYER_I_DECODER_H

class IDecoder {
public:
    virtual void GoOn() = 0;
    virtual void Pause() = 0;
    virtual void Stop() = 0;
    virtual bool IsRunning() = 0;
    virtual long GetDuration() = 0;
    virtual long GetCurPos() = 0;

};
#endif //TPLAYER_I_DECODER_H
