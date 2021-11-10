#include <Arduino.h>

class GPRS
{
protected:
  u8 id;
  ST_PDPContxt_Callback st_callbacks;
  ST_GprsConfig st_apn;
  static void onDeactive(u8 id, s32 ret, void *gprs);
  static void onActive(u8 id, s32 ret, void *gprs);
  bool result;
  u32 event;

public:
  GPRS(u8 context);
  bool begin();
  bool begin(const char *apn);
  bool begin(const char *apn, const char *name, const char *pass);
  bool act();
  bool deact(void);
};

extern GPRS gprs;
