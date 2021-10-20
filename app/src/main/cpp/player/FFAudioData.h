
#ifndef XAUDIODATA_H
#define XAUDIODATA_H


struct FFAudioData {
    unsigned char *data = nullptr;

    bool Alloc(int size, const char *value = nullptr);

    void Drop();
};


#endif
