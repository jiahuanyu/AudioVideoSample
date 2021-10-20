#ifndef IOBSERVABLE_H
#define IOBSERVABLE_H

#include "IObserver.h"
#include <mutex>
#include <vector>

template<class T>
class IObservable {
public:
    // 添加观察者
    void AddObserver(IObserver<T> *observer) {
        if (observer == nullptr) {
            return;
        }
        mutex.lock();
        observers.push_back(observer);
        mutex.unlock();
    };

    // 通知数据变化
    void NotifyValueChanged(T value) {
        mutex.lock();
        for (auto &observer : observers) {
            observer->Update(value);
        }
        mutex.unlock();
    };

protected:
    // 所有观察者
    std::vector<IObserver<T> *> observers;
    // 锁，观察者线程安全
    std::mutex mutex;
};


#endif
