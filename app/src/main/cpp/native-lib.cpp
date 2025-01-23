#include <jni.h>
#include <string>

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