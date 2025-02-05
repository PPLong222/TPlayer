//
// Created by 赵云龙 on 2/5/25.
//

#include "egl_surface.h"
#include "../../util/logger.h"

CustomEGLSurface::CustomEGLSurface() {
    m_core = new EGLCore();
}

CustomEGLSurface::~CustomEGLSurface() {
    delete m_core;
}

bool CustomEGLSurface::Init() {
    return m_core->Init(nullptr);
}

void CustomEGLSurface::CreateEglSurface(ANativeWindow *native_window, int width, int height) {
    if (native_window != nullptr) {
        LOGI(TAG, "native_window not null")
        m_native_window = native_window;
        m_surface = m_core->CreateWindowSurface(m_native_window);
    } else {
        m_surface = m_core->CreateOffScreenSurface(width, height);
    }

    if (m_surface == nullptr) {
        LOGE(TAG, "EGL custom_egl_surface create window surface fail");
        Release();
    }
    MakeCurrent();
}

void CustomEGLSurface::MakeCurrent() {
    m_core->MakeCurrent(m_surface);
}

void CustomEGLSurface::SwapBuffers() {
    m_core->SwapBuffers(m_surface);
}

void CustomEGLSurface::DestroyEglSurface() {
    if (m_surface != nullptr) {
        if (m_core != nullptr) {
            m_core->DestroySurface(m_surface);
        }
        m_surface = nullptr;
    }
}

void CustomEGLSurface::Release() {
    DestroyEglSurface();
    if (m_core != nullptr) {
        m_core->Release();
    }
}


