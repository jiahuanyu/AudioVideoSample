
#include "GLUtil.h"
#include "log_util.h"

GLuint GLUtil::LoadShader(const char *code, GLint type) {
    GLuint shader = glCreateShader(type);
    if (shader == 0) {
        LOGCATE("glCreateShader %d fail", type);
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
        LOGCATE("compile sharder fail");
        return 0;
    } else {
        LOGCATI("compile sharder success");
    }
    return shader;
}

