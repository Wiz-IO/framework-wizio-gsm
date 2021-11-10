#include <GPRS.h>

#define DEBUG_GPRS 
//Serial.printf

GPRS::GPRS(u8 context)
{
    id = context;
    result = -1;
    st_callbacks.Callback_GPRS_Actived = onActive;
    st_callbacks.CallBack_GPRS_Deactived = onDeactive;
    Ql_snprintf((char *)st_apn.apnName, sizeof(st_apn.apnName), "name");
    Ql_snprintf((char *)st_apn.apnUserId, sizeof(st_apn.apnUserId), "user");
    Ql_snprintf((char *)st_apn.apnUserId, sizeof(st_apn.apnUserId), "pass");
    event = Ql_OS_CreateEvent((char *)"GPRS_EVENT");
    Ql_GPRS_Config(id, &st_apn);
    Ql_GPRS_Register(context, &st_callbacks, this);
};

void GPRS::onActive(u8 id, s32 ret, void *gprs)
{
    DEBUG_GPRS("[GPRS] onActive( %d %d )", id, ret);
    if (gprs)
    {
        GPRS *p = (GPRS *)gprs;
        p->result = (ret == GPRS_PDP_ALREADY || ret == GPRS_PDP_SUCCESS);
        Ql_OS_SetEvent(p->event, 1);
    }
}

void GPRS::onDeactive(u8 id, s32 ret, void *user)
{
    DEBUG_GPRS("[GPRS] onDeactivedGPRS( %d %d )", id, ret);
}

bool GPRS::act()
{
    int res = Ql_GPRS_ActivateEx(id, true); // max 150 sec
    if (GPRS_PDP_SUCCESS == res || GPRS_PDP_SUCCESS == res)
        return true;
    DEBUG_GPRS("[GPRS] Ql_GPRS_ActivateEx( %d ) %d", id, res);
    Ql_OS_WaitEvent(event, 1);
    return result;
}

bool GPRS::deact(void)
{
    int res = Ql_GPRS_Deactivate(id);
    if (GPRS_PDP_SUCCESS == res || GPRS_PDP_SUCCESS == res)
        return true;
    DEBUG_GPRS("[GPRS] Ql_GPRS_Deactivate( %d ) %d", id, res); // 2 = GPRS_PDP_WOULDBLOCK
    Ql_OS_WaitEvent(event, 1);
    return result;
}

bool GPRS::begin(const char *name, const char *user, const char *pass)
{
    if (name)
        Ql_snprintf((char *)st_apn.apnName, sizeof(st_apn.apnName), name);
    else
        Ql_snprintf((char *)st_apn.apnName, sizeof(st_apn.apnName), "name");

    if (user)
        Ql_snprintf((char *)st_apn.apnUserId, sizeof(st_apn.apnUserId), user);
    else
        Ql_snprintf((char *)st_apn.apnUserId, sizeof(st_apn.apnUserId), "user");

    if (pass)
        Ql_snprintf((char *)st_apn.apnUserId, sizeof(st_apn.apnUserId), pass);
    else
        Ql_snprintf((char *)st_apn.apnUserId, sizeof(st_apn.apnUserId), "pass");
    if (0 == event)
        event = Ql_OS_CreateEvent((char *)"GPRS_EVENT");
    int res = Ql_GPRS_Config(id, &st_apn);
    return (res == GPRS_PDP_ALREADY || res == GPRS_PDP_SUCCESS);
}
bool GPRS::begin() { return begin(NULL, NULL, NULL); }
bool GPRS::begin(const char *name) { return begin(name, NULL, NULL); }

GPRS gprs(0);