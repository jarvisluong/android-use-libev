#ifndef _DEV_OBSERVER_H
#define _DEV_OBSERVER_H


#include "PObject.h"


class DevObserver : virtual public PObject
{
public:
    virtual void OnAttached(SmartDevice *pDevice) = 0;
    virtual void OnDetached(SmartDevice *pDevice) = 0;
    virtual void OnStateChanged(SmartDevice *pDevice) = 0;
};


#endif

