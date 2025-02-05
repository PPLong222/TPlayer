//
// Created by 赵云龙 on 2/5/25.
//

#ifndef TPLAYER_OPENGL_RENDER_H
#define TPLAYER_OPENGL_RENDER_H


#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <memory>
#include "../../../../opengl/egl/egl_surface.h"
#include "opengl_pixel_receiver.h"
#include "../../../../opengl/drawer/proxy/drawer_proxy.h"

class OpenGLRender: public std::enable_shared_from_this<OpenGLRender> {

private:
    const char *TAG = "OpenGLRender";
    enum STATE {
        NO_SURFACE,
        FRESH_SURFACE,
        RENDERING,
        SURFACE_DESTROY,
        STOP
    };

    JNIEnv *m_env = nullptr;
    JavaVM *m_jvm_for_thread = nullptr;
    jobject m_surface_ref = nullptr;
    ANativeWindow *m_native_window = nullptr;
    CustomEGLSurface *m_egl_surface = nullptr;
    DrawerProxy *m_drawer_proxy = nullptr;

    int m_window_w = 0;
    int m_window_h = 0;

    STATE m_state = NO_SURFACE;

    bool m_need_output_pixels = false;

    OpenGLPixelReceiver *m_pixel_receiver = nullptr;



    bool InitEGL();

    void InitDisplayWindow(JNIEnv *env);

    void CreateSurface();

    void DestroySurface();

    void Render();

    void ReleaseRender();

    void ReleaseDrawers();

    void ReleaseSurface();

    void ReleaseWindow();

    void sRenderThread();

public:
    OpenGLRender(JNIEnv *env, DrawerProxy *drawer_proxy);

    ~OpenGLRender();

    void InitRenderThread();

    void SetPixelReceiver(OpenGLPixelReceiver *receiver) {
        m_pixel_receiver = receiver;
    }

    void SetSurface(jobject surface);

    void SetOffScreenSize(int width, int height);

    void RequestRGBAData();

    void Stop();
};


#endif //TPLAYER_OPENGL_RENDER_H
