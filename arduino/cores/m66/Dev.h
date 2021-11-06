#ifndef __DEV_H__
#define __DEV_H__

#include "interface.h"
#include "RilClass.h"
extern HardwareSerial Virtual;

typedef void (*vCallback)(void);
typedef void (*uCallback)(u32, u32);
typedef void (*mCallback)(ST_MSG *msg);

class DeviceClass
{
public:
    DeviceClass()
    {
        onMessage = NULL;
        onUrc = NULL;
        wtdID = -1;
    }

    RilClass ril = RilClass(Virtual);
    RilClass &operator=(RilClass &ril);
    void begin() { ril.begin(); }

    mCallback onMessage;      // public
    void m_Message(ST_MSG *m) // private
    {
        if (onMessage)
            onMessage(m);
    }

    uCallback onUrc;              // public
    void m_Urc(u32 urc, u32 data) // private
    {
        if (onUrc)
            onUrc(urc, data);
    }

    void enterPin(const char *pin);
    void reset() { Ql_Reset(0); }
    void powerOff() { Ql_PowerDown(1); }
    int powerReason() { return Ql_GetPowerOnReason(); }
    void Sleep() { Ql_SleepEnable(); }
    void noSleep() { Ql_SleepDisable(); }

    //for pin pulse
    int watchdog(uint8_t pin, unsigned int interval = 1000)
    {
        if (interval < 200)
            interval == 200;
        return Ql_WTD_Init(0, (Enum_PinName)pin, interval);
    }
    //internal watchdog
    void beginWatchdog(unsigned int interval = 1000)
    {
        if (interval < 400)
            interval = 400;
        wtdID = Ql_WTD_Start(interval);
    }
    void endWatchdog()
    {
        if (wtdID > 0)
            Ql_WTD_Stop(wtdID);
    }
    void kickWatchdog()
    {
        if (wtdID > 0)
            Ql_WTD_Feed(wtdID);
    }

private:
    int wtdID;
};

extern DeviceClass Dev;

#endif //__DEV_H__