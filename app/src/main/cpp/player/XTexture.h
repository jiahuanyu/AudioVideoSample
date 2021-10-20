#ifndef XTEXTURE_H
#define XTEXTURE_H


class XTexture {
public:
    static XTexture *Create();

    virtual bool Init(void *window) = 0;

    virtual void Draw(unsigned char *data[], int width, int height) = 0;
};


#endif
