#include <jni.h>
#include <string>
#include "media/player/player.h"
#include "test/test.h"
#include "media/player/opengl_player.h"
// tell compiler compiling by rules of C rather than cpp
// because ffmpeg is written in C
extern "C" {
#include <libavutil/avutil.h>
}


extern "C" JNIEXPORT jstring JNICALL
Java_indi_pplong_tplayer_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}

extern "C"
JNIEXPORT jstring JNICALL
Java_indi_pplong_tplayer_MainActivity_stringFFmpegVersion(JNIEnv *env, jobject thiz) {
    return env->NewStringUTF(av_version_info());
}
extern "C"
JNIEXPORT jlong JNICALL
Java_indi_pplong_tplayer_player_TPlayer_createPlayer(JNIEnv *env, jobject thiz, jstring path,
                                                            jobject surface) {
    GLPlayer *player = new GLPlayer(env, path, surface);
    jlong handle = reinterpret_cast<jlong>(player);
    return handle;
}
extern "C"
JNIEXPORT void JNICALL
Java_indi_pplong_tplayer_player_TPlayer_play(JNIEnv *env, jobject thiz, jlong player) {
    GLPlayer *p = (GLPlayer *) player;
    p->Play();
}
extern "C"
JNIEXPORT void JNICALL
Java_indi_pplong_tplayer_player_TPlayer_pause(JNIEnv *env, jobject thiz, jlong player) {
    GLPlayer *p = (GLPlayer *) player;
    p->Pause();
}

extern "C"
JNIEXPORT void JNICALL
Java_indi_pplong_tplayer_player_TPlayer_resume(JNIEnv *env, jobject thiz, jlong player) {
    GLPlayer *p = (GLPlayer *) player;
    p->Resume();
}
extern "C"
JNIEXPORT jlong JNICALL
Java_indi_pplong_tplayer_player_TPlayer_getPlayTime(JNIEnv *env, jobject thiz,
                                                           jlong player) {
    GLPlayer *p = (GLPlayer *) player;
    return p->GetDuration();
}
extern "C"
JNIEXPORT jlong JNICALL
Java_indi_pplong_tplayer_player_TPlayer_getCurPos(JNIEnv *env, jobject thiz,
                                                         jlong player) {
    GLPlayer *p = (GLPlayer *) player;
    return p->GetCurPos();
}
extern "C"
JNIEXPORT void JNICALL
Java_indi_pplong_tplayer_player_TPlayer_seek(JNIEnv *env, jobject thiz, jlong player,
                                                    jlong progress) {
    GLPlayer *p = (GLPlayer *) player;
    p->Seek(progress);
}