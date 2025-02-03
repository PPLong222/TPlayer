//
// Created by 赵云龙 on 2/1/25.
//

#ifndef TPLAYER_OPEN_SL_RENDER_H
#define TPLAYER_OPEN_SL_RENDER_H

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include "audio_render.h"
#include <string>
#include <queue>

class OpenSLRender : public AudioRender {
private:
    class PcmData {
    public:
        PcmData(uint8_t *pcm, int size) {
            this->pcm = pcm;
            this->size = size;
        }

        ~PcmData() {
            if (pcm != nullptr) {
                free(pcm);
                pcm = nullptr;
                used = false;
            }
        }

        uint8_t *pcm = nullptr;
        int size = 0;
        bool used = false;
    };

    const char *TAG = "OpenSLRender";

    SLObjectItf m_engine_obj = nullptr;
    SLEngineItf m_engine = nullptr;

    SLObjectItf m_output_mix_obj = nullptr;
    SLEnvironmentalReverbItf m_output_mix_evn_reverb = nullptr;
    SLEnvironmentalReverbSettings m_reverb_settings = SL_I3DL2_ENVIRONMENT_PRESET_DEFAULT;

    SLObjectItf m_pcm_player_obj = nullptr;
    SLPlayItf m_pcm_player = nullptr;
    SLVolumeItf m_pcm_player_volume = nullptr;

    SLAndroidSimpleBufferQueueItf m_pcm_buffer;

    std::queue<PcmData *> m_data_queue;


    pthread_mutex_t m_cache_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t m_cache_cond_empty = PTHREAD_COND_INITIALIZER;
    pthread_cond_t m_cache_cond_full = PTHREAD_COND_INITIALIZER;

    const SLuint32 SL_QUEUE_BUFFER_COUNT = 2;
    const int MAX_CACHE_CAPACITY = 2;

    bool CreateEngine();

    bool CreateOutputMixer();

    bool CreatePlayer();

    void StartRender();

    void BlockEnqueue();

    bool CheckError(SLresult result, std::string hint);

    void SendCacheReadySignal();

    void static sRenderPcm(OpenSLRender *that);

    void static sReadPcmBufferCBFun(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context);

public:
    OpenSLRender();

    ~OpenSLRender() override;

    void InitRender() override;

    void Render(uint8_t *pcm, int size) override;

    void ReleaseRender() override;
};

#endif //TPLAYER_OPEN_SL_RENDER_H
