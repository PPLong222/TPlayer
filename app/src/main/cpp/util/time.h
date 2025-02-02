//
// Created by 赵云龙 on 1/23/25.
//

#ifndef TPLAYER_TIME_H
#define TPLAYER_TIME_H

extern "C" {
#include "sys/time.h"

int64_t GetCurMsTime() {
    struct timeval tv{};
    gettimeofday(&tv, NULL);
    int64_t ts = (int64_t) tv.tv_sec * 1000 + tv.tv_usec / 1000;
    return ts;
}
}

#endif //TPLAYER_TIME_H
