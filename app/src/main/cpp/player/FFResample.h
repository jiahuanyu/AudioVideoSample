#ifndef FFRESAMPLE_H
#define FFRESAMPLE_H

#include "IObserver.h"
#include "FFParameter.h"
#include "IObservable.h"
#include "FFFrameData.h"
#include "FFAudioData.h"

struct SwrContext;

class FFResample : public IObserver<FFFrameData>, public IObservable<FFAudioData> {
public:
    virtual bool Open(FFParameter in, FFParameter out = {});

    virtual FFAudioData Resample(FFFrameData data);

    virtual void Update(FFFrameData data);

protected:
    SwrContext *swrContext;
};


#endif
