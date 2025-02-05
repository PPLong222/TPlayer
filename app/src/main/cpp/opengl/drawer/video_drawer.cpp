//
// Created by 赵云龙 on 2/5/25.
//

#include "video_drawer.h"

VideoDrawer::VideoDrawer() : Drawer(0, 0) {
}

VideoDrawer::~VideoDrawer() {

}

void VideoDrawer::InitRender(JNIEnv *env, int video_width, int video_height, int *dst_size) {
    SetSize(video_width, video_height);
    dst_size[0] = video_width;
    dst_size[1] = video_height;
}

void VideoDrawer::Render(OneFrame *one_frame) {
    LOGI("123123", "Video Drawer Render");
    cst_data = one_frame->data;
}

void VideoDrawer::ReleaseRender() {
}

const char *VideoDrawer::GetVertexShader() {
    static const GLbyte shader[] = "attribute vec4 aPosition;\n"
                            "attribute vec2 aCoordinate;\n"
                            "varying vec2 vCoordinate;\n"
                            "void main() {\n"
                            "  gl_Position = aPosition;\n"
                            "  vCoordinate = aCoordinate;\n"
                            "}";
    return (char *) shader;
}

const char *VideoDrawer::GetFragmentShader() {
    static const GLbyte shader[] = "precision mediump float;\n"
                            "uniform sampler2D uTexture;\n"
                            "varying vec2 vCoordinate;\n"
                            "void main() {\n"
                            "gl_FragColor = texture2D(uTexture, vCoordinate);\n"
                            "}";
    return (char *) shader;
}

void VideoDrawer::InitCstShaderHandler() {

}

void VideoDrawer::BindTexture() {
    ActivateTexture();
}

void VideoDrawer::PrepareDraw() {
    if (cst_data != nullptr) {
        LOGI("TAG", "Prepare Draw, width: %d, height:%d", width(), height())
        glTexImage2D(GL_TEXTURE_2D, 0, // level一般为0
                     GL_RGBA, //纹理内部格式
                     width(), height(), // 画面宽高
                     0, // 必须为0
                     GL_RGBA, // 数据格式，必须和上面的纹理格式保持一直
                     GL_UNSIGNED_BYTE, // RGBA每位数据的字节数，这里是BYTE​: 1 byte
                     cst_data);// 画面数据

        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            LOGE("SSS", "glTexImage2D failed: 0x%x", error);
        }
    }
}

void VideoDrawer::DoneDraw() {
}