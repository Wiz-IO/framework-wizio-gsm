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
private:
    int nwStatGSM, nwStatGPRS;

public:
    DeviceClass()
    {
        onMessage = NULL;
        onUrc = NULL;
        nwStatGSM = nwStatGPRS = wtdID = -1;
    }

    RilClass ril = RilClass(Virtual);
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
        switch (urc)
        {
        case URC_GSM_NW_STATE_IND:
            nwStatGSM = data;
            break;
        case URC_GPRS_NW_STATE_IND:
            nwStatGPRS = data;
            break;
        }
        if (onUrc)
            onUrc(urc, data);
    }
    int getGsmStat() { return nwStatGSM; }
    bool waitGsmReady(uint32_t timeout_ms = -1)
    {
        uint32_t start = millis();
        while (NW_STAT_REGISTERED != nwStatGSM)
        {
            if (NW_STAT_REGISTERED_ROAMING == nwStatGSM)
                break;
            arduinoProcessMessages(10);
            if (millis() - start > timeout_ms)
                return false; // timeout
        }
        arduinoProcessMessages(1);
        return true;
    }

    int getGprsStat() { return nwStatGPRS; }
    bool waitGprsReady(uint32_t timeout_ms = -1)
    {
        uint32_t start = millis();
        while (NW_STAT_REGISTERED != nwStatGPRS)
        {
            if (NW_STAT_REGISTERED_ROAMING == nwStatGPRS)
                break;
            arduinoProcessMessages(10);
            if (millis() - start > timeout_ms)
                return false; // timeout
        }
        arduinoProcessMessages(1);
        return true;
    }

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