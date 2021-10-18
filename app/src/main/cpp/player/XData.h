
#ifndef XDATA_H
#define XDATA_H


struct XData {
    unsigned char * data = nullptr;
    int size = 0;
    void Drop();
};


#endif
