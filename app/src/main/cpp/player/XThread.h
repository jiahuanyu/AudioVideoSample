
#ifndef XTHREAD_H
#define XTHREAD_H

void XSleep(int mis);

class XThread {
public:
    virtual void Start();
    virtual void Stop();
    virtual void Main(){}

protected:
    bool isExist = false;
    bool isRunning = false;
private:
    void ThreadMain();
};


#endif
