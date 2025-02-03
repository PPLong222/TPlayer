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
        case STOP: return "STOP";
        case PREPARE: return "PREPARE";
        case START: return "START";
        case DECODING: return "DECODING";
        case PAUSE: return "PAUSE";
        case FINISH: return "FINISH";
        default: return "UNKNOWN";
    }
}
#endif //TPLAYER_DECODE_STATE_H

