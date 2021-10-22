#ifndef GLUTIL_H
#define GLUTIL_H

#include <GLES2/gl2.h>

class GLUtil {
public:
    static GLuint LoadShader(const char *code, GLint type);
};

#endif
