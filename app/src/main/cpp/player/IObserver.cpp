#include "IObserver.h"

void IObserver::AddObserver(IObserver *observer) {
    if (observer == nullptr) {
        return;
    }
    mutex.lock();
    observers.push_back(observer);
    mutex.unlock();
}

void IObserver::Notify(XData data) {
    mutex.lock();
    for (int i = 0; i < observers.size(); i++) {
        observers[i]->Update(data);
    }
    mutex.unlock();
}
