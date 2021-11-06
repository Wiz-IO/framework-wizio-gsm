
#include "Dev.h"
DeviceClass Dev;

void DeviceClass::enterPin(const char *pin)
{
    int res;
    if (pin)
    {
        Dev.ril.sendf("AP+CPIN =\"%s\"", pin);
        if (ril.waitForResponse() == 1)
            return;
    }
    else
    {
        abort();
    }
}