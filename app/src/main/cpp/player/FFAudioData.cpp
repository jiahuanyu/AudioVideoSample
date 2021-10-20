#include "FFAudioData.h"
#include <string>

bool FFAudioData::Alloc(int size, const char *value) {
    Drop();
    if (size <= 0) {
        return false;
    }
    this->data = new unsigned char[size];
    if (this->data == nullptr) {
        return false;
    }
    if (value != nullptr) {
        memcpy(this->data, value, size);
    }
    return true;
}

void FFAudioData::Drop() {
    if (data == nullptr) {
        return;
    }
    delete data;
    data = nullptr;
}
