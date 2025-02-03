//
// Created by 赵云龙 on 2/1/25.
//

#ifndef TPLAYER_CONST_H
#define TPLAYER_CONST_H



extern "C" {
#include <libavutil/channel_layout.h>
#include <libavutil/samplefmt.h>
};

// AUDIO_DEST_FORMAT: FLOAT
static const AVSampleFormat ENCODE_AUDIO_DEST_FORMAT = AV_SAMPLE_FMT_FLTP;

static const int ENCODE_AUDIO_DEST_SAMPLE_RATE = 44100;

static const int ENCODE_AUDIO_DEST_CHANNEL_COUNTS = 2;

static const int ENCODE_AUDIO_DEST_BIT_RATE = 64000;

static const int ACC_NB_SAMPLES = 1024;

static const int ENCODE_VIDEO_FPS = 25;
#endif //TPLAYER_CONST_H
