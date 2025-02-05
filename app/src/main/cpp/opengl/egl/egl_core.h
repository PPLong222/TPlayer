//
// Created by 赵云龙 on 2/5/25.
//

#ifndef TPLAYER_EGL_CORE_H
#define TPLAYER_EGL_CORE_H

extern "C" {
#include <EGL/egl.h>
#include <EGL/eglext.h>
};
class EGLCore {
private:
    const char* TAG = "EGLCore";

    EGLDisplay m_egl_display = EGL_NO_DISPLAY;

    EGLContext m_egl_cxt = EGL_NO_CONTEXT;

    EGLConfig m_egl_cfg;

    EGLConfig GetEGLConfig();
public:
    EGLCore();
    ~EGLCore();

    bool Init(EGLContext share_ctx);

    EGLSurface CreateWindowSurface(ANativeWindow *window);

    EGLSurface CreateOffScreenSurface(int width, int height);

    void MakeCurrent(EGLSurface egl_surface);

    void SwapBuffers(EGLSurface egl_surface);

    void DestroySurface(EGLSurface egl_surface);

    void Release();
};


#endif //TPLAYER_EGL_CORE_H
