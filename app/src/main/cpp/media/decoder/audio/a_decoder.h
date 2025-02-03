//
// Created by 赵云龙 on 2/1/25.
//

#ifndef TPLAYER_A_DECODER_H
#define TPLAYER_A_DECODER_H

#include "../base_decoder.h"
#include "../../render/audio/audio_render.h"

extern "C" {
#include <libswresample/swresample.h>
#include <libavutil/audio_fifo.h>
};
class AudioDecoder : public BaseDecoder {
private:
    const char *TAG = "AudioDecoder";

    SwrContext  *m_swr_ctx = nullptr;
    AudioRender *m_render = nullptr;
    uint8_t  *m_out_buffer[1] = {nullptr};

    int m_dest_nb_sample = 1024;

    int m_dest_data_size = 0;

    void InitSwr();

    void InitOutBuffer();

    void InitRender();

    void ReleaseOutBuffer();

    AVSampleFormat GetSampleFmt();

    int GetSampleRate(int spr);

public:
    AudioDecoder(JNIEnv *env, const jstring path, bool forSynthesizer);
    ~AudioDecoder() override;

    void SetRender(AudioRender *render);

protected:
    void Prepare(JNIEnv *env) override;
    void Render(AVFrame *frame) override;
    void Release() override;
    bool NeedLoopDecode() override;

    AVMediaType GetMediaType() override;
};


#endif //TPLAYER_A_DECODER_H
