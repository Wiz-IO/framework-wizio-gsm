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

#include "interface.h"
#include "Dev.h"
extern DeviceClass Dev;

extern void setup();
extern void loop();

extern "C"
{

    static struct
    {
        uint32_t wait;
        uint32_t event;
        ST_MSG msg;
    } arduino = {10 /* default task wait mSec */, 0, {0, 0, 0, 0}};

    void arduinoSetWait(u32 wait)
    {
        arduino.wait = wait == 0 ? 1 : wait;
    }

    void arduinoDispatchMessages(void)
    {
        switch (arduino.msg.message)
        {
        case MSG_ID_URC_INDICATION:
            Dev.m_Urc(arduino.msg.param1, arduino.msg.param2);
            break;
        default:
            Dev.m_Message(&arduino.msg);
            break;
        }
    }

    void arduinoProcessMessages(unsigned int wait) // UART, TIMER ... etc process callbacks
    {
        uint32_t id = Ql_OS_GetActiveTaskId();
        if (ARDUINO_TASK_ID == id)
        {
            Ql_OS_GetMessage(&arduino.msg);                    // there is always more messages (send one empty message)
            Ql_OS_SendMessage(id, MSG_PROCESS_MESSAGES, 0, 0); // send one empty message
            arduinoDispatchMessages();
        }
        Ql_Sleep(wait); // default is 10 mSec, make more -> arduinoSetWait(mSec) if Arduino loop() is not critical
    }

    void delayEx(unsigned int ms)
    {
        if (ms < 500)
        {
            Ql_Sleep(ms);
            arduinoProcessMessages(1);
        }
        else
        {
#define BLOCK_TIME 100
            unsigned int count = ms / BLOCK_TIME;
            while (count--)
                arduinoProcessMessages(BLOCK_TIME);  // step
            arduinoProcessMessages(ms % BLOCK_TIME); // remain
        }
    }

    // Arduino Task
    void proc_arduino(int id)
    {
        while (arduino.event == 0)
            Ql_Sleep(10);
        Ql_OS_WaitEvent(arduino.event, 1); // block & wait ril ready

        Ql_OS_SendMessage(id, MSG_PROCESS_MESSAGES, 0, 0); // send one empty message

        arduinoProcessMessages(arduino.wait);
        setup();

        while (true)
        {
            arduinoProcessMessages(arduino.wait);
            loop();
        }
    }

    // Main Task
    void proc_main_task(int taskId)
    {
        ST_MSG m;
        __libc_init_array();
        arduino.event = Ql_OS_CreateEvent((char *)"ARDUINO_EVENT");
        srand(HAL_SEED);
        while (true)
        {
            Ql_OS_GetMessage(&m);
            switch (m.message)
            {
            case MSG_ID_RIL_READY:
                Ql_RIL_Initialize();
                Ql_OS_SetEvent(arduino.event, 1); // start arduino task
                break;
            case MSG_ID_URC_INDICATION:
                if (m.message > URC_GPRS_NW_STATE_IND)                                 // ignore first urc-s
                    Ql_OS_SendMessage(ARDUINO_TASK_ID, m.message, m.param1, m.param2); // resend to arduino task
                break;
            default:
                Ql_OS_SendMessage(ARDUINO_TASK_ID, m.message, m.param1, m.param2); // resend to arduino task
                break;
            }
        }
    }

} // extern "C"