#include "XShader.h"
#include "XLog.h"
#include <GLES2/gl2.h>

#define GET_STR(x) #x
static const char *vertexShader = GET_STR(
        attribute vec4 aPosition;
        attribute vec2 aTexCoord;

        varying vec2 vTexCoord;

        void main() {
            vTexCoord = aTexCoord;
            gl_Position = aPosition;
        }
);

// 片源着色器
static const char *fragmentYUV420PShader = GET_STR(
        precision mediump float;
        uniform sampler2D yTexture;
        uniform sampler2D uTexture;
        uniform sampler2D vTexture;

        varying vec2 vTexCoord;

        void main() {
            vec3 yuv;
            vec3 rgb;
            yuv.x = texture2D(yTexture, vTexCoord).r;
            yuv.y = texture2D(uTexture, vTexCoord).r - 0.5;
            yuv.z = texture2D(vTexture, vTexCoord).r - 0.5;
            rgb = mat3(
                    1.0, 1.0, 1.0,
                    0.0, -0.3945, 2.03211,
                    1.13983, -0.5806, 0.0
            ) * yuv;
            gl_FragColor = vec4(rgb, 1.0);
        }
);

static GLuint InitShader(const char *code, GLint type) {
    GLuint shader = glCreateShader(type);
    if (shader == 0) {
        XLOGE("glCreateShader %d fail", type);
        return 0;
    }
    // 加载 shader
    glShaderSource(shader, 1, &code, nullptr);
    // 编译 shader
    glCompileShader(shader);
    // 获取编译情况
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE) {
        XLOGE("compile sharder fail");
        return 0;
    } else {
        XLOGI("compile sharder success");
    }
    return shader;
}

bool XShader::Init() {
    vsh = InitShader(vertexShader, GL_VERTEX_SHADER);
    if (vsh == 0) {
        XLOGE("InitShader GL_VERTEX_SHADER fail");
    }
    XLOGI("InitShader GL_VERTEX_SHADER success");

    fsh = InitShader(fragmentYUV420PShader, GL_FRAGMENT_SHADER);
    if (fsh == 0) {
        XLOGE("InitShader GL_FRAGMENT_SHADER fail");
    }
    XLOGI("InitShader GL_FRAGMENT_SHADER success");

    program = glCreateProgram();
    if (program == 0) {
        XLOGE("glCreateProgram fail");
        return false;
    }
    // 加入着色器
    glAttachShader(program, vsh);
    glAttachShader(program, fsh);

    // 链接
    glLinkProgram(program);


    GLint status;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status != GL_TRUE) {
        XLOGE("glLinkProgram fail");
        return false;
    }

    // 激活
    glUseProgram(program);

    // 设置顶点坐标
    static float vertexCoords[] = {
            1.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 0.0f
    };
    GLint aPositionLocation = glGetAttribLocation(program, "aPosition");
    glEnableVertexAttribArray(aPositionLocation);
    glVertexAttribPointer(aPositionLocation, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          vertexCoords);

    // 设置纹理坐标
    static float textureCoords[] = {
            1.0f, 1.0f,
            0.0f, 1.0f,
            1.0f, 0.0f,
            0.0f, 0.0f,
    };
    GLint aTextCoordLocation = glGetAttribLocation(program, "aTexCoord");
    glEnableVertexAttribArray(aTextCoordLocation);
    glVertexAttribPointer(aTextCoordLocation, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                          textureCoords);

    // 设置层
    glUniform1i(glGetUniformLocation(program, "yTexture"), 0);
    glUniform1i(glGetUniformLocation(program, "uTexture"), 1);
    glUniform1i(glGetUniformLocation(program, "vTexture"), 2);

    XLOGI("init shader success");

    return true;
}

void XShader::Draw() {
    if (program == 0) {
        return;
    }
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void XShader::GetTexture(unsigned int index, int width, int height, unsigned char *buf) {
    if (textureIds[index] == 0) {
        glGenTextures(1, &textureIds[index]);
        glActiveTexture(GL_TEXTURE0 + index);
        glBindTexture(GL_TEXTURE_2D, textureIds[index]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, textureIds[index]);
    glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_LUMINANCE,
            width,
            height,
            0,
            GL_LUMINANCE,
            GL_UNSIGNED_BYTE,
            buf);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, textureIds[index]);
}
