//
// Created by 赵云龙 on 2/5/25.
//

#include "drawer.h"
#include "../../util/logger.h"

#include <malloc.h>

Drawer::Drawer(int width, int height) :
        m_origin_w(width),
        m_origin_h(height) {

}

Drawer::~Drawer() {

}

void Drawer::SetSize(int width, int height) {
    m_origin_w = width;
    m_origin_h = height;
}

void Drawer::Draw() {
    if (IsReadyToDraw()) {
        CreateTextureId();
        CreateProgram();
        BindTexture();
        PrepareDraw();
        DoDraw();
        DoneDraw();
    }
}

void Drawer::CreateTextureId() {
    if (m_texture_id == 0) {
        glGenTextures(1, &m_texture_id);
        LOGI(TAG, "Create texture id : %d, Success: %x", m_texture_id, glGetError() == GL_NO_ERROR)
    }
}

void Drawer::CreateProgram() {
    if (m_program_id == 0) {
        m_program_id = glCreateProgram();
        if (glGetError() != GL_NO_ERROR) {
            LOGE(TAG, "Error when create program");
            return;
        }

        GLuint vertexShader = LoadShader(GL_VERTEX_SHADER, GetVertexShader());
        GLuint fragmentShader = LoadShader(GL_FRAGMENT_SHADER, GetFragmentShader());

        glAttachShader(m_program_id, vertexShader);
        glAttachShader(m_program_id, fragmentShader);
        glLinkProgram(m_program_id);

        m_vertex_matrix_handler = glGetUniformLocation(m_program_id, "uMatrix");
        m_vertex_pos_handler = glGetAttribLocation(m_program_id, "aPosition");
        m_texture_handler = glGetUniformLocation(m_program_id, "uTexture");
        m_texture_pos_handler = glGetAttribLocation(m_program_id, "aCoordinate");

        InitCstShaderHandler();

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    /*
     * forgot this line and cause a torturing debugging period...
     * finally debugged by glError() logging per line.
     */
    if (m_program_id != 0) {
        glUseProgram(m_program_id);
    }
}

GLuint Drawer::LoadShader(GLenum type, const GLchar *shader_code) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shader_code, nullptr);
    glCompileShader(shader);

    GLint compiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (!compiled) {
        GLint infoLen = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            GLchar *infoLog = static_cast<GLchar *>(malloc(sizeof(GLchar) * infoLen));
            glGetShaderInfoLog(shader, infoLen, nullptr, infoLog);
            LOGE(TAG, "Error compiling shader %s", infoLog);
            free(infoLog);
        }

        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

void Drawer::ActivateTexture(GLenum type, GLuint texture, GLenum index, int texture_handler) {
    if (texture == -1) {
        texture = m_texture_id;
    }
    if (texture_handler == -1) {
        texture_handler = m_texture_handler;
    }
    LOGI(TAG,"Texture ID: %d, texture_handler_id: %d", m_texture_id, texture_handler)
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(type, texture);
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        LOGE("SSS", "glBindTexture failed: 0x%x", error);
    } else {
        LOGE("SSS", "glBindTexture success: ");
    }


    glUniform1i(texture_handler, index);
    glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

}

void Drawer::Release() {
    glDisableVertexAttribArray(m_vertex_pos_handler);
    glDisableVertexAttribArray(m_texture_pos_handler);
    glBindTexture(GL_TEXTURE_2D, 0);
    glDeleteTextures(1, &m_texture_id);
    glDeleteProgram(m_program_id);
}

bool Drawer::IsReadyToDraw() {
    return m_origin_w > 0 && m_origin_h > 0;
}

void Drawer::DoDraw() {

    glEnableVertexAttribArray(m_vertex_pos_handler);
    glEnableVertexAttribArray(m_texture_pos_handler);

    glVertexAttribPointer(m_vertex_pos_handler, 2, GL_FLOAT, GL_FALSE, 0, m_vertex_coors);
    glVertexAttribPointer(m_texture_pos_handler, 2, GL_FLOAT, GL_FALSE, 0, m_texture_coors);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
