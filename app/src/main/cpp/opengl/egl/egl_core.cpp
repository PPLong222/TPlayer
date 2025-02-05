//
// Created by 赵云龙 on 2/5/25.
//

#include "egl_core.h"
#include "../../util/logger.h"

bool EGLCore::Init(EGLContext share_ctx) {
    if (m_egl_display != EGL_NO_DISPLAY) {
        LOGE(TAG, "EGL already set");
        return true;
    }
    if (share_ctx == nullptr) {
        share_ctx = EGL_NO_CONTEXT;
    }

    m_egl_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    if (m_egl_display == EGL_NO_DISPLAY || eglGetError() != EGL_SUCCESS) {
        LOGE(TAG, "EGL init fail when creating display");
        return false;
    }

    EGLint major_ver, minor_ver;
    EGLBoolean success = eglInitialize(m_egl_display, &major_ver, &minor_ver);
    if (success != EGL_TRUE || eglGetError() != EGL_SUCCESS) {
        LOGE(TAG, "EGL init fail when init version");
        return false;
    }

    LOGI(TAG, "EGL version: %d.%d", major_ver, minor_ver)

    m_egl_cfg = GetEGLConfig();

    const EGLint attr[] = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};
    m_egl_cxt = eglCreateContext(m_egl_display, m_egl_cfg, share_ctx, attr);
    if (m_egl_cxt == EGL_NO_CONTEXT) {
        LOGE(TAG, "EGL init fail when creating context");
        return false;
    }

    EGLint egl_format;
    success = eglGetConfigAttrib(m_egl_display, m_egl_cfg, EGL_NATIVE_VISUAL_ID, &egl_format);
    if (success != EGL_TRUE || eglGetError() != EGL_SUCCESS) {
        LOGE(TAG, "EGL init fail when init egl_format");
        return false;
    }

    LOGI(TAG, "EGL init success");
    return true;
}

EGLConfig EGLCore::GetEGLConfig() {
    EGLint numConfigs;
    EGLConfig config;

    static const EGLint CONFIG_ATTRS[]{
            EGL_BUFFER_SIZE, EGL_DONT_CARE,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 16,
            EGL_STENCIL_SIZE, EGL_DONT_CARE,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_NONE
    };

    EGLBoolean success = eglChooseConfig(m_egl_display, CONFIG_ATTRS, &config, 1, &numConfigs);
    if (!success || eglGetError() != EGL_SUCCESS) {
        LOGE(TAG, "EGL config init error")
        return nullptr;
    }
    LOGI(TAG, "EGL config init success")
    return config;
}

EGLSurface EGLCore::CreateWindowSurface(ANativeWindow *window) {
    EGLSurface surface = eglCreateWindowSurface(m_egl_display, m_egl_cfg, window, 0);
    if (eglGetError() != EGL_SUCCESS) {
        LOGE(TAG, "EGL create window surface fail")
        return nullptr;
    }
    return surface;
}

EGLSurface EGLCore::CreateOffScreenSurface(int width, int height) {
    int CONFIG_ATTRS[] = {
            EGL_WIDTH, width,
            EGL_HEIGHT, height,
            EGL_NONE
    };

    EGLSurface surface = eglCreatePbufferSurface(m_egl_display, m_egl_cfg, CONFIG_ATTRS);
    if (eglGetError() != EGL_SUCCESS) {
        LOGE(TAG, "EGL create offScreen surface fail")
        return nullptr;
    }
    return surface;
}

void EGLCore::MakeCurrent(EGLSurface egl_surface) {
    if (!eglMakeCurrent(m_egl_display, egl_surface, egl_surface, m_egl_cxt)) {
        LOGE(TAG, "EGL make current fail")
    } else {
        LOGI(TAG, "EGL Make Current Successfully")
    }
}

void EGLCore::SwapBuffers(EGLSurface egl_surface) {
    LOGI(TAG, "SwapBuffers")

    EGLBoolean res = eglSwapBuffers(m_egl_display, egl_surface);
    if (res == EGL_FALSE) {
        LOGE(TAG, "eglSwapBuffers failed")
    }

}

void EGLCore::DestroySurface(EGLSurface egl_surface) {
    eglMakeCurrent(m_egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(m_egl_display, egl_surface);
}

void EGLCore::Release() {
    if (m_egl_display != EGL_NO_DISPLAY) {
        eglMakeCurrent(m_egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(m_egl_display, m_egl_cxt);
        eglReleaseThread();
        eglTerminate(m_egl_display);
    }
    m_egl_display = EGL_NO_DISPLAY;
    m_egl_cxt = EGL_NO_CONTEXT;
    m_egl_cfg = nullptr;
}

EGLCore::EGLCore() {

}

EGLCore::~EGLCore() {

}


