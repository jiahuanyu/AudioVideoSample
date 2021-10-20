#ifndef IOBSERVER_H
#define IOBSERVER_H

#include "XThread.h"

// 观察者
template <class T>
class IObserver {
public:
    virtual void Update(T value) {

    }
};


#endif
