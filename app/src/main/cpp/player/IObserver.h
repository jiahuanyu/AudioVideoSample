#ifndef IOBSERVER_H
#define IOBSERVER_H

#include "XData.h"
#include <vector>
#include <mutex>
#include "XThread.h"

class IObserver: public XThread {
public:
    virtual void Update(XData data) {}

    void AddObserver(IObserver *observer);

    void Notify(XData data);

protected:
    std::vector<IObserver *> observers;
    std::mutex mutex;
};


#endif
