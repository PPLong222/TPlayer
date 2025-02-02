//
// Created by 赵云龙 on 1/23/25.
//

#ifndef TPLAYER_DECODE_STATE_H
#define TPLAYER_DECODE_STATE_H

enum DecodeState {
    STOP,
    PREPARE,
    START,
    DECODING,
    PAUSE,
    FINISH
};

inline const char* DecodeStateToString(DecodeState decodeState) {
    switch (decodeState) {
        STOP: return "STOP";
        PREPARE: return "PREPARE";
        START: return "START";
        DECODING: return "DECODING";
        PAUSE: return "PAUSE";
        FINISH: return "FINISH";
        default: return "UNKNOWN";
    }
}
#endif //TPLAYER_DECODE_STATE_H

