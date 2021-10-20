#ifndef XSHADER_H
#define XSHADER_H


class XShader {
public:
    virtual bool Init();

    virtual void GetTexture(unsigned int index, int width, int height, unsigned char *buf);

    virtual void Draw();

protected:
    unsigned int vsh = 0;
    unsigned int fsh = 0;
    unsigned int program = 0;
    // 纹理
    unsigned int textureIds[100] = {0};
};

#endif
