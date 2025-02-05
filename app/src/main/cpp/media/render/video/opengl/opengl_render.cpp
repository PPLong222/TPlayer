//
// Created by 赵云龙 on 2/5/25.
//

#include <thread>
#include "opengl_render.h"
#include "../../../../util/logger.h"

OpenGLRender::OpenGLRender(JNIEnv *env, DrawerProxy *drawer_proxy) :
        m_drawer_proxy(drawer_proxy) {
    m_env = env;
    env->GetJavaVM(&m_jvm_for_thread);
}

OpenGLRender::~OpenGLRender() {
    delete m_egl_surface;
}

void OpenGLRender::InitRenderThread() {
    auto self = shared_from_this();
    std::thread t(&OpenGLRender::sRenderThread, self);
    t.detach();
}

void OpenGLRender::sRenderThread() {
    JNIEnv *env;
    if (m_jvm_for_thread->AttachCurrentThread(&env, nullptr) != JNI_OK) {
        LOGE(TAG, "Init Error when attach current thread")
        return;
    }

    if (!InitEGL()) {
        m_jvm_for_thread->DetachCurrentThread();
        return;
    }

    while (true) {
        LOGI(TAG, "m_state: %d", m_state);
        switch (m_state) {
            case FRESH_SURFACE:
                InitDisplayWindow(env);
                CreateSurface();
                m_state = RENDERING;
                break;
            case RENDERING:
                Render();
                break;
            case SURFACE_DESTROY:
                DestroySurface();
                m_state = NO_SURFACE;
                break;
            case STOP:
                ReleaseRender();
                m_jvm_for_thread->DetachCurrentThread();
                return;
            case NO_SURFACE:
            default:
                break;
        }
        usleep(20 * 1000);
    }
}

bool OpenGLRender::InitEGL() {
    m_egl_surface = new CustomEGLSurface();
    return m_egl_surface->Init();
}

void OpenGLRender::InitDisplayWindow(JNIEnv *env) {
    if (m_surface_ref != nullptr) {

        m_native_window = ANativeWindow_fromSurface(env, m_surface_ref);
        m_window_w = ANativeWindow_getWidth(m_native_window);
        m_window_h = ANativeWindow_getHeight(m_native_window);

        ANativeWindow_setBuffersGeometry(m_native_window, m_window_w, m_window_h,
                                         WINDOW_FORMAT_RGBA_8888);
        LOGI(TAG, "InitDisplayWindow Successfully")
    }
}

void OpenGLRender::CreateSurface() {
    m_egl_surface->CreateEglSurface(m_native_window, m_window_w, m_window_h);
    glViewport(0, 0, m_window_w, m_window_h);
}

void OpenGLRender::Render() {
    if (m_state == RENDERING) {
        m_drawer_proxy->Draw();
        m_egl_surface->SwapBuffers();

        if (m_need_output_pixels && m_pixel_receiver != nullptr) {
            m_need_output_pixels = false;
            Render();
            size_t size = m_window_w * m_window_h * 4 * sizeof(uint8_t);
            uint8_t *rgb = (uint8_t *) malloc(size);
            if (rgb == nullptr) {
                realloc(rgb ,size);
                LOGE(TAG, "Error when alloc")
            }
            glReadPixels(0, 0, m_window_w, m_window_h, GL_RGBA, GL_UNSIGNED_BYTE, rgb);
            m_pixel_receiver->ReceiverPixel(rgb);
        }
    }
}

void OpenGLRender::ReleaseRender() {
    ReleaseDrawers();
    ReleaseSurface();
    ReleaseWindow();
}

void OpenGLRender::ReleaseSurface() {
    if (m_egl_surface != nullptr) {
        m_egl_surface->Release();
        delete m_egl_surface;
        m_egl_surface = nullptr;
    }
}

void OpenGLRender::ReleaseDrawers() {
    if (m_drawer_proxy != nullptr) {
        m_drawer_proxy->Release();
        delete m_drawer_proxy;
        m_drawer_proxy = nullptr;
    }
}

void OpenGLRender::ReleaseWindow() {
    if (m_native_window != nullptr) {
        ANativeWindow_release(m_native_window);
        m_native_window = nullptr;
    }
}

void OpenGLRender::SetSurface(jobject surface) {
    if (surface != nullptr) {
        m_surface_ref = m_env->NewGlobalRef(surface);
        m_state = FRESH_SURFACE;
    } else {
        m_env->DeleteGlobalRef(m_surface_ref);
        m_state = SURFACE_DESTROY;
    }
}

void OpenGLRender::SetOffScreenSize(int width, int height) {
    m_window_w = width;
    m_window_h = height;
    m_state = FRESH_SURFACE;
}

void OpenGLRender::RequestRGBAData() {
    m_need_output_pixels = true;
}

void OpenGLRender::Stop() {
    m_state = STOP;
}

void OpenGLRender::DestroySurface() {
    m_egl_surface->DestroyEglSurface();
    ReleaseWindow();
}
