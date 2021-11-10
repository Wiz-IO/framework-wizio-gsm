////////////////////////////////////////////////////////////////////////////////////////
//
//      2021 Georgi Angelov
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////////////////

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
