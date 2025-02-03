//
// Created by 赵云龙 on 2/1/25.
//

#include "open_sl_render.h"
#include "../../../util/logger.h"
#include "../../const.h"
#include <thread>

OpenSLRender::OpenSLRender() {

}

OpenSLRender::~OpenSLRender() {

}

void OpenSLRender::InitRender() {
    if (!CreateEngine()) return;
    if (!CreateOutputMixer()) return;
    if (!CreatePlayer()) return;
    LOGI(TAG, "Successfully init Render")
    std::thread t(sRenderPcm, this);
    t.detach();
}

bool OpenSLRender::CreateEngine() {
    SLresult result = slCreateEngine(&m_engine_obj, 0, nullptr, 0, nullptr, nullptr);
    if (CheckError(result, "Engine")) return false;

    result = (*m_engine_obj)->Realize(m_engine_obj, SL_BOOLEAN_FALSE);
    if (CheckError(result, "Engine Realize"))return false;

    result = (*m_engine_obj)->GetInterface(m_engine_obj, SL_IID_ENGINE, &m_engine);
    if (CheckError(result, "Engine Interface")) return false;

    return true;
}

bool OpenSLRender::CreateOutputMixer() {
    const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};
    SLresult result = (*m_engine)->CreateOutputMix(m_engine, &m_output_mix_obj, 1, mids,
                                                   mreq);
    if (CheckError(result, "Output Mix")) return false;

    result = (*m_output_mix_obj)->Realize(m_output_mix_obj, SL_BOOLEAN_FALSE);
    if (CheckError(result, "Output Mix Realize")) return false;

    result = (*m_output_mix_obj)->GetInterface(m_output_mix_obj, SL_IID_ENVIRONMENTALREVERB, &m_output_mix_evn_reverb);
    if (CheckError(result, "Output Mix Env Reverb")) return false;

    if (result == SL_RESULT_SUCCESS) {
        (*m_output_mix_evn_reverb)->SetEnvironmentalReverbProperties(m_output_mix_evn_reverb, &m_reverb_settings);
    }

    return true;
}

bool OpenSLRender::CreatePlayer() {
    LOGI(TAG, "---------------------Begin OpenSL ES Init---------------------")
    SLDataLocator_AndroidSimpleBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                            SL_QUEUE_BUFFER_COUNT};
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,
            (SLuint32) ENCODE_AUDIO_DEST_CHANNEL_COUNTS,
            SL_SAMPLINGRATE_44_1,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_PCMSAMPLEFORMAT_FIXED_16,
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,
            SL_BYTEORDER_LITTLEENDIAN
    };
    SLDataSource slDataSource = {&android_queue, &pcm};

    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, m_output_mix_obj};
    SLDataSink slDataSink = {&outputMix, nullptr};

    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    SLresult result = (*m_engine)->CreateAudioPlayer(m_engine, &m_pcm_player_obj, &slDataSource,
                                                     &slDataSink, 3, ids, req);
    if (CheckError(result, "Player")) return false;

    result = (*m_pcm_player_obj)->Realize(m_pcm_player_obj, SL_BOOLEAN_FALSE);
    if (CheckError(result, "Player Realize")) return false;

    result = (*m_pcm_player_obj)->GetInterface(m_pcm_player_obj, SL_IID_PLAY, &m_pcm_player);
    if (CheckError(result, "Player Interface}")) return false;

    result = (*m_pcm_player_obj)->GetInterface(m_pcm_player_obj, SL_IID_VOLUME,
                                               &m_pcm_player_volume);
    if (CheckError(result, "Player Volume Interface}")) return false;

    result = (*m_pcm_player_obj)->GetInterface(m_pcm_player_obj, SL_IID_BUFFERQUEUE, &m_pcm_buffer);
    if (CheckError(result, "Player Queue Buffer Interface}")) return false;

    result = (*m_pcm_buffer)->RegisterCallback(m_pcm_buffer, sReadPcmBufferCBFun, this);
    if (CheckError(result, "Register Callback Interface}")) return false;

    LOGI(TAG, "---------------------OpenSL ES Init Success---------------------")
    return true;
}

void OpenSLRender::sRenderPcm(OpenSLRender *that) {
    that->StartRender();
}

void OpenSLRender::StartRender() {
    pthread_mutex_lock(&m_cache_mutex);
    while (m_data_queue.empty()) {
        LOGI(TAG, "Wait in StartRender")
        pthread_cond_wait(&m_cache_cond_empty, &m_cache_mutex);
    }
    (*m_pcm_player)->SetPlayState(m_pcm_player, SL_PLAYSTATE_PLAYING);
    pthread_mutex_unlock(&m_cache_mutex);
    sReadPcmBufferCBFun(m_pcm_buffer, this);
}

// TODO: Modify and delete
void OpenSLRender::SendCacheReadySignal() {
    LOGI(TAG, "Begin Signal")
    pthread_mutex_lock(&m_cache_mutex);
    pthread_cond_signal(&m_cache_cond_empty);
    pthread_mutex_unlock(&m_cache_mutex);
}

void
OpenSLRender::sReadPcmBufferCBFun(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context) {
    OpenSLRender *player = (OpenSLRender *) context;
    pthread_mutex_lock(&player->m_cache_mutex);
    if (!player->m_data_queue.empty()) {
        PcmData* pcm = player->m_data_queue.front();
        pcm->used = true;
    }
    // TODO: if it's necessary cause we will lock later
    pthread_mutex_unlock(&player->m_cache_mutex);
    player->BlockEnqueue();
}

void OpenSLRender::BlockEnqueue() {
    if (m_pcm_player == nullptr) return;

    pthread_mutex_lock(&m_cache_mutex);

    while (!m_data_queue.empty()) {
        PcmData *pcm = m_data_queue.front();
        if (pcm->used) {
            m_data_queue.pop();
            delete pcm;
        } else {
            break;
        }
    }

    while (m_data_queue.empty() && m_pcm_player != nullptr) {
        LOGI(TAG, "Wait in BlockEnqueue")
        pthread_cond_wait(&m_cache_cond_empty, &m_cache_mutex);
    }

    PcmData *pcmData = m_data_queue.front();
    if (pcmData != nullptr && m_pcm_player) {
        SLresult result = (*m_pcm_buffer)->Enqueue(m_pcm_buffer, pcmData->pcm,
                                                   (SLuint32) pcmData->size);
        CheckError(result, "Enqueue PCM");
    }

    pthread_mutex_unlock(&m_cache_mutex);
    pthread_cond_signal(&m_cache_cond_full);
}

void OpenSLRender::Render(uint8_t *pcm, int size) {
    LOGI(TAG, "OpenSLRender Render")
    if (m_pcm_player) {
        if (pcm != nullptr && size > 0) {
            pthread_mutex_lock(&m_cache_mutex);
            LOGI(TAG, "m_data_queue size: %zu", m_data_queue.size())
            while (m_data_queue.size() >= MAX_CACHE_CAPACITY) {
                LOGI(TAG, "Send with usleep")
                pthread_cond_wait(&m_cache_cond_full, &m_cache_mutex);
            }

            auto *data = (uint8_t *) malloc(size);
            if (!data) {
                LOGE(TAG, "Failed to allocate memory for PCM data");
                return;
            }
            memcpy(data, pcm, size);

            auto *pcmData = new PcmData(data, size);
            m_data_queue.push(pcmData);
            pthread_mutex_unlock(&m_cache_mutex);
            pthread_cond_signal(&m_cache_cond_empty);

        }
    } else {
        LOGE(TAG, "PCM == NULL or pcm.size == 0")
        free(pcm);
    }
}

bool OpenSLRender::CheckError(SLresult result, std::string hint) {
    if (SL_RESULT_SUCCESS != result) {
        LOGE(TAG, "OpenSL ES [%s] init fail", hint.c_str())
        return true;
    }
    return false;
}

void OpenSLRender::ReleaseRender() {
    if (m_pcm_player) {
        (*m_pcm_player)->SetPlayState(m_pcm_player, SL_PLAYSTATE_STOPPED);
        m_pcm_player = nullptr;
    }

    // TODO: optimize
    SendCacheReadySignal();


    if (m_pcm_player_obj) {
        (*m_pcm_player_obj)->Destroy(m_pcm_player_obj);
        m_pcm_player_obj = nullptr;
        m_pcm_buffer = nullptr;
    }

    if (m_output_mix_obj) {
        (*m_output_mix_obj)->Destroy(m_output_mix_obj);
        m_output_mix_obj = nullptr;
    }

    if (m_engine_obj) {
        (*m_engine_obj)->Destroy(m_engine_obj);
        m_engine_obj = nullptr;
        m_engine = nullptr;
    }

    for (int i = 0; i < m_data_queue.size(); ++i) {
        PcmData *pcm = m_data_queue.front();
        m_data_queue.pop();
        delete pcm;
    }
}
