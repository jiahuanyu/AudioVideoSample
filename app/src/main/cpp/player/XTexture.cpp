#include "XTexture.h"
#include "XLog.h"
#include "XEGL.h"
#include "XShader.h"

class CXTexture : public XTexture {
public:
    XShader shader;

    bool Init(void *window) override {
        if (window == nullptr) {
            XLOGE("XTexture Init fail");
        }
        if (!XEGL::Get()->Init(window)) {
            return false;
        }
        if (!shader.Init()) {
            return false;
        }
        return true;
    }

    void Draw(unsigned char *data[], int width, int height) override {
        shader.GetTexture(0, width, height, data[0]);
        shader.GetTexture(1, width >> 1, height >> 1, data[1]);
        shader.GetTexture(2, width >> 1, height >> 1, data[2]);
        shader.Draw();
        XEGL::Get()->Draw();
    }
};

XTexture *XTexture::Create() {
    return new CXTexture();
}
