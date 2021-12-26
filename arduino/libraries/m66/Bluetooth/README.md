```
#include <Arduino.h>
#include <BT_SPP_Serial.h>

void setup()
{
  Serial.begin(115200, true);
  printf("\n[APP] Bluetooth SPP Serial 2021 Georgi Angelov\n");
  BT.begin();
}

void loop()
{
  BT.Poll();
  if (spp.connected)
  {
    // as Serial ....
  }
}
```