//
// Created by 赵云龙 on 2/5/25.
//

#ifndef TPLAYER_EGL_SURFACE_H
#define TPLAYER_EGL_SURFACE_H

#include <android/native_window.h>
#include "egl_core.h"

class CustomEGLSurface {
private:
    const char *TAG = "CustomEGLSurface";

    ANativeWindow *m_native_window = nullptr;

    EGLCore *m_core;

    EGLSurface m_surface;

public:
    CustomEGLSurface();
    ~CustomEGLSurface();

    bool Init();
    void CreateEglSurface(ANativeWindow *native_window, int width, int height);
    void MakeCurrent();
    void SwapBuffers();
    void DestroyEglSurface();
    void Release();
};


#endif //TPLAYER_EGL_SURFACE_H
