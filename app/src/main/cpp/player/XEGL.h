#ifndef XEGL_H
#define XEGL_H


class XEGL {
public:
    static XEGL *Get();

    virtual bool Init(void *window) = 0;

    virtual void Draw() = 0;
protected:
    XEGL() {}
};


#endif
