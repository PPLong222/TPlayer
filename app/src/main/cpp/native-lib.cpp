#include <jni.h>
#include <string>
#include "media/player/player.h"
#include "test/test.h"
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
Java_indi_pplong_tplayer_ffmpeg_FFmpegActivity_createPlayer(JNIEnv *env, jobject thiz, jstring path,
                                                            jobject surface) {
    Player *player = new Player(env, path, surface);
    jlong handle = reinterpret_cast<jlong>(player);
    return handle;
}
extern "C"
JNIEXPORT void JNICALL
Java_indi_pplong_tplayer_ffmpeg_FFmpegActivity_play(JNIEnv *env, jobject thiz, jlong player) {
    Player *p = (Player *) player;
    p->play();
}
extern "C"
JNIEXPORT void JNICALL
Java_indi_pplong_tplayer_ffmpeg_FFmpegActivity_pause(JNIEnv *env, jobject thiz, jlong player) {
    Player *p = (Player *) player;
    p->pause();
}
extern "C"
JNIEXPORT void JNICALL
Java_indi_pplong_tplayer_ffmpeg_FFmpegActivity_test(JNIEnv *env, jobject thiz) {
    Test *test = new Test();
}