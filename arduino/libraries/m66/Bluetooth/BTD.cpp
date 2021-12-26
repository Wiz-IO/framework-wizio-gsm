/* Copyright (C) 2012 Kristian Lauszus, TKJ Electronics. All rights reserved.

 This software may be distributed and modified under the terms of the GNU
 General Public License version 2 (GPL2) as published by the Free Software
 Foundation and appearing in the file GPL2.TXT included in the packaging of
 this file. Please note that GPL2 Section 2[b] requires that all works based
 on this software must also be made publicly available under the terms of
 the GPL2 ("Copyleft").

 Contact information
 -------------------

 Kristian Lauszus, TKJ Electronics
 Web      :  https://github.com/felis/USB_Host_Shield_2.0/blob/master/BTD.cpp
 e-mail   :  lauszus@gmail.com
 */

#include "BTD.h"

BT_HCI_Serial HCI;
BTD BT(&HCI);

BTD::BTD(BT_HCI_Serial *p) : useSimplePairing(false),
                             bAddress(0), // Device address - mandatory
                             qNextPollTime(0),
                             pollInterval(0),
                             simple_pairing_supported(1),
                             bPollEnable(false)
{
        pSerial = p;
        for (uint8_t i = 0; i < BTD_NUM_SERVICES; i++)
                btService[i] = NULL;
        Initialize(); // Set all variables, endpoint structs etc. to default values
}

uint8_t BTD::begin()
{
        hci_num_reset_loops = 100; // only loop 100 times before trying to send the hci reset command
        hci_counter = 0;
        hci_state = HCI_INIT_STATE;
        waitingForConnection = false;
        bPollEnable = true;
        if (pSerial)
        {
                pSerial->begin();
                return 0;
        }
        return -1;
}

void BTD::Initialize()
{
        for (int i = 0; i < BTD_NUM_SERVICES; i++)
        {
                if (btService[i])
                        btService[i]->Reset(); // Reset all Bluetooth services
        }
        bAddress = 0;      // Clear device address
        qNextPollTime = 0; // Reset next poll time
        pollInterval = 0;
        bPollEnable = false;
        simple_pairing_supported = 1;
}

uint8_t BTD::Release()
{
        Initialize(); // Set all variables, endpoint structs etc. to default values
        return 0;
}

uint8_t BTD::Poll()
{
        if (!bPollEnable)
                return 0;
        if ((int32_t)((uint32_t)millis() - qNextPollTime) >= 0L)
        {                                                          // Don't poll if shorter than polling interval
                qNextPollTime = (uint32_t)millis() + pollInterval; // Set new poll time
                HCI_event_task();                                  // Poll the HCI event pipe
                HCI_task();                                        // HCI state machine
                ACL_event_task();                                  // Poll the ACL input pipe too
        }
        return 0;
}

void BTD::Disconnect()
{
        for (uint8_t i = 0; i < BTD_NUM_SERVICES; i++)
                if (btService[i])
                        btService[i]->Disconnect();
};

void BTD::HCI_event_task()
{
        uint16_t length = 0;
        if (pSerial->available() < 3) // min 4 bytes
                return;
        if (pSerial->peek() != HCI_EVENT_PKT) // 4
                return;
        pSerial->read(); // skip first byte

        DEBUG_BTD("[BTD] EVN DATA ");
        hcibuf[length++] = pSerial->read(); // [0]
        hcibuf[length++] = pSerial->read(); // [1]
        for (int i = 0; i < hcibuf[1]; i++) // [len]
                hcibuf[length++] = pSerial->read();

        DEBUG_BTD("%02X ", (int)hcibuf[0]);
        DEBUG_BTD("%02X ", (int)hcibuf[1]);
        DEBUG_BTD("\n");

        switch (hcibuf[0])
        {
        case EV_COMMAND_COMPLETE:
        {
                if (!hcibuf[5]) // Check if command succeeded
                {
                        //DEBUG_BTD("[BTD] EV_COMMAND_COMPLETE\n");
                        hci_set_flag(HCI_FLAG_CMD_COMPLETE);            // Set command complete flag
                        if ((hcibuf[3] == 0x01) && (hcibuf[4] == 0x10)) // Parameters from read local version information
                        {
                                hci_version = hcibuf[6];
                                DEBUG_BTD("[BTD] HCI Version: %d\n", (int)hci_version); // 6 = Bluetooth Core Specification 4.0
                                hci_set_flag(HCI_FLAG_READ_VERSION);
                        }
                        else if ((hcibuf[3] == 0x04) && (hcibuf[4] == 0x10)) // Parameters from read local extended features
                        {
                                // Parameters from read local extended features
                                if (!hci_check_flag(HCI_FLAG_LOCAL_EXTENDED_FEATURES))
                                {
                                        if (hcibuf[6] == 0)
                                        {
                                                // Page 0
                                                if (hcibuf[8 + 6] & (1U << 3))
                                                {
                                                        simple_pairing_supported = true;
                                                        DEBUG_BTD("[BTD] simple pairing supported TRUE\n");
                                                }
                                                else
                                                {
                                                        simple_pairing_supported = false;
                                                        DEBUG_BTD("[BTD] simple pairing supported FALSE\n");
                                                }
                                        }
                                }
                                hci_set_flag(HCI_FLAG_LOCAL_EXTENDED_FEATURES);
                        }
                        else if ((hcibuf[3] == 0x09) && (hcibuf[4] == 0x10))
                        {
                                DEBUG_BTD("[BTD] Local Address: ");
                                for (uint8_t i = 0; i < 6; i++)
                                {
                                        my_bdaddr[i] = hcibuf[6 + i];
                                        DEBUG_BTD("%02X ", (int)my_bdaddr[i]);
                                }
                                DEBUG_BTD("\n");
                                hci_set_flag(HCI_FLAG_READ_BDADDR);
                        }
                }
                break;
        }

        case EV_CONNECT_REQUEST:
        {
                //DEBUG_BTD("[BTD] EV_INCOMING_CONNECT\n");
                DEBUG_BTD("[BTD] Remote Address: "); // 6D E9 93 2B D4 08
                for (uint8_t i = 0; i < 6; i++)
                {
                        disc_bdaddr[i] = hcibuf[i + 2];
                        DEBUG_BTD("%02X ", (int)disc_bdaddr[i]);
                }
                DEBUG_BTD("\n");

                DEBUG_BTD("[BTD] Remote Class: "); // 0C 02 5A
                for (uint8_t i = 0; i < 3; i++)
                {
                        classOfDevice[i] = hcibuf[i + 8];
                        DEBUG_BTD("%02X ", (int)classOfDevice[i]);
                }
                DEBUG_BTD("\n");

                hci_set_flag(HCI_FLAG_INCOMING_REQUEST);
                break;
        }

        case EV_PIN_CODE_REQ:
        {
                //DEBUG_BTD("[BTD] EV_PIN_CODE_REQUEST\n");
                if (btdPin != NULL)
                {
                        hci_pin_code_request_reply();
                }
                else
                {
                        hci_pin_code_negative_request_reply();
                }
                break;
        }

        case EV_LINK_KEY_REQ:
        {
                //DEBUG_BTD("[BTD] EV_LINK_KEY_REQUEST\n");
                hci_link_key_request_negative_reply();
                break;
        }

        case EV_AUTH_COMPLETE:
        {
                //DEBUG_BTD("[BTD] EV_AUTHENTICATION_COMPLETE\n");
                if (!hcibuf[2])
                {
                        // Check if pairing was successful
                }
                else
                {
                        hci_disconnect(hci_handle);
                        hci_state = HCI_DISCONNECT_STATE;
                }
                break;
        }

        case EV_REMOTE_NAME_REQ_COMPLETE:
        {
                if (!hcibuf[2])
                {
                        //DEBUG_BTD("[BTD] EV_REMOTE_NAME_COMPLETE\n");
                        for (uint8_t i = 0; i < min(sizeof(remote_name), sizeof(hcibuf) - 9); i++)
                        {
                                remote_name[i] = hcibuf[9 + i];
                                if (remote_name[i] == '\0') // End of string
                                        break;
                        }
                        remote_name[29] = 0;
                        DEBUG_BTD("[BTD] Remote Name: %s\n", remote_name);
                        hci_set_flag(HCI_FLAG_REMOTE_NAME_COMPLETE);
                }
                break;
        }

        case EV_DISCONNECT_COMPLETE:
        {
                if (!hcibuf[2])
                {
                        DEBUG_BTD("[BTD] EV_DISCONNECT_COMPLETE\n");
                        hci_set_flag(HCI_FLAG_DISCONNECT_COMPLETE); // Set disconnect command complete flag
                        hci_clear_flag(HCI_FLAG_CONNECT_COMPLETE);  // Clear connection complete flag
                }
                break;
        }

        case EV_CONNECT_COMPLETE:
        {
                hci_set_flag(HCI_FLAG_CONNECT_EVENT);
                if (!hcibuf[2])
                {
                        hci_handle = hcibuf[3] | ((hcibuf[4] & 0x0F) << 8);
                        DEBUG_BTD("[BTD] Connection established: HANDLE = 0x%04X\n", (int)hci_handle);
                        hci_set_flag(HCI_FLAG_CONNECT_COMPLETE);
                }
                else
                {
                        hci_state = HCI_CHECK_DEVICE_SERVICE;
                        DEBUG_BTD("[BTD-ERROR] Connection Failed\n");
                }
                break;
        }

        case EV_INQUIRY_COMPLETE: // NU
        {
                DEBUG_BTD("[BTD] EV_INQUIRY_COMPLETE: %d\n", (int)inquiry_counter);
                inquiry_counter++;
                break;
        }

        case EV_INQUIRY_RESULT:
        case EV_INQUIRY_RESULT_WITH_EIR: // NU
        {
                if (hcibuf[2])
                {
                        DEBUG_BTD("[BTD] EV_INQUIRY_RESULTS\n");
                        DEBUG_BTD("[BTD] Number of responses: %d\n", (int)hcibuf[2]);
                        for (uint8_t i = 0; i < hcibuf[2]; i++)
                        {
                                uint8_t classOfDevice_offset;
                                if (hcibuf[0] == EV_INQUIRY_RESULT)
                                        classOfDevice_offset = 9 * hcibuf[2]; // 6-byte bd_addr, 1 byte page_scan_repetition_mode, 2 byte reserved
                                else
                                        classOfDevice_offset = 8 * hcibuf[2]; // 6-byte bd_addr, 1 byte page_scan_repetition_mode, 1 byte reserved

                                for (uint8_t j = 0; j < 3; j++)
                                        classOfDevice[j] = hcibuf[3 + classOfDevice_offset + 3 * i + j];

                                DEBUG_BTD("[BTD] Class of device: %02X %02X %02X\n", (int)classOfDevice[2], (int)classOfDevice[1], (int)classOfDevice[0]);

                                ////
                        }
                }
                break;
        }

        case EV_IO_CAPABILITY_REQUEST:
        {
                DEBUG_BTD("[BTD] EV_IO_CAPABILITY_REQUEST\n");
                hci_io_capability_request_reply();
                break;
        }

        case EV_USER_CONFIRM_REQUSEST:
        {
                DEBUG_BTD("[BTD] EV_USER_CONFIRMATION_REQUEST\n");
                hci_user_confirmation_request_reply();
                break;
        }

        case 0x08:
        case 0x0F:
        case 0x12:
        case 0x13:
        case 0x18:
        case 0x1B:
        case 0x1D:
        {
                break;
        }

        default:
        {
                DEBUG_BTD("[BTD-DEFAULT] HCI EVENT: 0x%02X\n", (int)hcibuf[0]);
                break;
        }

        } // Switch
}

void BTD::HCI_task()
{
        switch (hci_state)
        {
        case HCI_INIT_STATE: // 0
        {
                ////DEBUG_BTD("[BTD] HCI_INIT_STATE\n");
                hci_counter++;
                if (hci_counter > hci_num_reset_loops)
                {
                        hci_reset();
                        hci_state = HCI_RESET_STATE;
                        hci_counter = 0;
                }
                break;
        }

        case HCI_RESET_STATE: // 1
        {
                ////DEBUG_BTD("[BTD] HCI_RESET_STATE\n");
                hci_counter++;
                if (hci_check_flag(HCI_FLAG_CMD_COMPLETE))
                {
                        hci_counter = 0;
                        hci_state = HCI_CLASS_STATE;
                        hci_write_class_of_device();
                }
                else if (hci_counter > hci_num_reset_loops)
                {
                        hci_num_reset_loops *= 10;
                        if (hci_num_reset_loops > 2000)
                                hci_num_reset_loops = 2000;
                        hci_state = HCI_INIT_STATE;
                        hci_counter = 0;
                }
                break;
        }

        case HCI_CLASS_STATE: // 2
        {
                if (hci_check_flag(HCI_FLAG_CMD_COMPLETE))
                {
                        //DEBUG_BTD("[BTD] HCI_CLASS_STATE\n");
                        hci_state = HCI_BDADDR_STATE;
                        hci_read_bdaddr();
                }
                break;
        }

        case HCI_BDADDR_STATE: // 3
        {
                if (hci_check_flag(HCI_FLAG_READ_BDADDR))
                {
                        //DEBUG_BTD("[BTD] HCI_BDADDR_STATE\n");
                        hci_read_local_version_information();
                        hci_state = HCI_LOCAL_VERSION_STATE;
                }
                break;
        }

        case HCI_LOCAL_VERSION_STATE: // 4
        {
                if (hci_check_flag(HCI_FLAG_READ_VERSION))
                {
                        //DEBUG_BTD("[BTD] HCI_LOCAL_VERSION_STATE\n");
                        if (btdName != NULL)
                        {
                                hci_write_local_name(btdName);
                                hci_state = HCI_WRITE_NAME_STATE;
                        }
                        else if (useSimplePairing)
                        {
                                hci_read_local_extended_features(0); // "Requests the normal LMP features as returned by Read_Local_Supported_Features"
                                //hci_read_local_extended_features(1); // Read page 1
                                hci_state = HCI_LOCAL_EXTENDED_FEATURES_STATE;
                        }
                        else
                        {
                                hci_state = HCI_CHECK_DEVICE_SERVICE;
                        }
                }
                break;
        }

        case HCI_WRITE_NAME_STATE: // 5
        {
                if (hci_check_flag(HCI_FLAG_CMD_COMPLETE))
                {
                        //DEBUG_BTD("[BTD] HCI_WRITE_NAME_STATE\n");
                        if (useSimplePairing)
                        {
                                hci_read_local_extended_features(0); // "Requests the normal LMP features as returned by Read_Local_Supported_Features"
                                //hci_read_local_extended_features(1); // Read page 1
                                hci_state = HCI_LOCAL_EXTENDED_FEATURES_STATE;
                        }
                        else
                        {
                                hci_state = HCI_CHECK_DEVICE_SERVICE;
                        }
                }
                break;
        }

        case HCI_CHECK_DEVICE_SERVICE: // 6
        {
                if (0)
                {
                        //DEBUG_BTD("[BTD] HCI_CHECK_DEVICE_SERVICE\n");
                        hci_inquiry();
                        hci_state = HCI_INQUIRY_STATE;
                }
                else
                {
                        hci_state = HCI_SCANNING_STATE;
                }
                break;
        }

        case HCI_INQUIRY_STATE: // 7
        {
                if (hci_check_flag(HCI_FLAG_DEVICE_FOUND))
                {
                        //DEBUG_BTD("[BTD] HCI_INQUIRY_STATE\n");
                        hci_inquiry_cancel(); // Stop inquiry
                        if (checkRemoteName)
                        {
                                hci_remote_name();
                                hci_state = HCI_REMOTE_NAME_STATE;
                        }
                        else
                        {
                                hci_state = HCI_CONNECT_DEVICE_STATE;
                        }
                }
                break;
        }

        case HCI_CONNECT_DEVICE_STATE: // 8
        {
                if (hci_check_flag(HCI_FLAG_CMD_COMPLETE))
                {
                        //DEBUG_BTD("[BTD] HCI_CONNECT_DEVICE_STATE\n");
                        checkRemoteName = false;
                        hci_connect();
                        hci_state = HCI_CONNECTED_DEVICE_STATE;
                }
                break;
        }

        case HCI_CONNECTED_DEVICE_STATE: // 9
        {
                if (hci_check_flag(HCI_FLAG_CONNECT_EVENT))
                {
                        //DEBUG_BTD("[BTD] HCI_CONNECTED_DEVICE_STATE\n");
                        if (hci_check_flag(HCI_FLAG_CONNECT_COMPLETE))
                        {
                                hci_authentication_request(); // This will start the pairing with the device
                                hci_state = HCI_SCANNING_STATE;
                        }
                        else
                        {
                                hci_connect(); // Try to connect one more time
                        }
                }
                break;
        }

        case HCI_SCANNING_STATE: // 10
        {
                if (1)
                {
                        //DEBUG_BTD("[BTD] HCI_SCANNING_STATE\n");
                        hci_write_scan_enable();
                        waitingForConnection = true;
                        hci_state = HCI_CONNECT_IN_STATE;
                }
                break;
        }

        case HCI_CONNECT_IN_STATE: // 11
        {
                if (hci_check_flag(HCI_FLAG_INCOMING_REQUEST))
                {
                        //DEBUG_BTD("[BTD] HCI_CONNECT_IN_STATE\n");
                        waitingForConnection = false;
                        hci_remote_name();
                        hci_state = HCI_REMOTE_NAME_STATE;
                }
                else
                {
                        if (hci_check_flag(HCI_FLAG_DISCONNECT_COMPLETE))
                        {
                                hci_state = HCI_DISCONNECT_STATE;
                        }
                }
                break;
        }

        case HCI_REMOTE_NAME_STATE: // 12
        {
                if (hci_check_flag(HCI_FLAG_REMOTE_NAME_COMPLETE))
                {
                        //DEBUG_BTD("[BTD] HCI_REMOTE_NAME_STATE\n");
                        hci_accept_connection();
                        hci_state = HCI_CONNECTED_STATE;
                }
                break;
        }

        case HCI_CONNECTED_STATE: // 13
        {
                //DEBUG_BTD("[BTD] HCI_CONNECTED_STATE\n");
                if (hci_check_flag(HCI_FLAG_CONNECT_COMPLETE))
                {
                        // Clear these flags for a new connection
                        l2capConnectionClaimed = false;
                        sdpConnectionClaimed = false;
                        rfcommConnectionClaimed = false;
                        hci_event_flag = 0;
                        hci_state = HCI_DONE_STATE;
                }
                break;
        }

        case HCI_DONE_STATE: // 15
        {
                //DEBUG_BTD("[BTD] HCI_DONE_STATE\n");
                hci_counter++;
                if (hci_counter > 1000)
                {
                        // Wait until we have looped 1000 times to make sure that the L2CAP connection has been started
                        hci_counter = 0;
                        hci_state = HCI_SCANNING_STATE;
                }
                break;
        }

        case HCI_DISCONNECT_STATE: // 16
        {
                //DEBUG_BTD("[BTD] HCI_DISCONNECT_STATE\n");
                if (hci_check_flag(HCI_FLAG_DISCONNECT_COMPLETE))
                {
                        hci_event_flag = 0; // Clear all flags
                        // Reset all buffers
                        memset(hcibuf, 0, BULK_MAXPKTSIZE);
                        memset(l2capinbuf, 0, BULK_MAXPKTSIZE);
                        hci_state = HCI_SCANNING_STATE;
                }
                break;
        }

        case HCI_LOCAL_EXTENDED_FEATURES_STATE: // 17
        {
                if (hci_check_flag(HCI_FLAG_LOCAL_EXTENDED_FEATURES))
                {
                        //DEBUG_BTD("[BTD] HCI_LOCAL_EXTENDED_FEATURES_STATE\n");
                        if (simple_pairing_supported)
                        {
                                hci_write_simple_pairing_mode(true);
                                hci_state = HCI_WRITE_SIMPLE_PAIRING_STATE;
                        }
                        else
                        {
                                hci_state = HCI_CHECK_DEVICE_SERVICE;
                        }
                }
                break;
        }

        case HCI_WRITE_SIMPLE_PAIRING_STATE: // 18
        {
                if (hci_check_flag(HCI_FLAG_CMD_COMPLETE))
                {
                        //DEBUG_BTD("[BTD] HCI_WRITE_SIMPLE_PAIRING_STATE\n");
                        hci_set_event_mask();
                        hci_state = HCI_SET_EVENT_MASK_STATE;
                }
                break;
        }

        case HCI_SET_EVENT_MASK_STATE: // 19
        {
                if (hci_check_flag(HCI_FLAG_CMD_COMPLETE))
                {
                        //DEBUG_BTD("[BTD] HCI_SET_EVENT_MASK_STATE\n");
                        hci_state = HCI_CHECK_DEVICE_SERVICE;
                }
                break;
        }

        default:
                break;
        } // switch
}

void BTD::ACL_event_task()
{
        uint16_t length = 0, len;
        if (pSerial->available() < 4) // min 5 bytes
                return;
        if (pSerial->peek() != HCI_ACLDATA_PKT) // 2
                return;
        pSerial->read(); // skip first byte

        DEBUG_BTD("[BTD] ACL DATA ");
        for (int i = 0; i < 4; i++) // handle[0,1] & len[2,3]
        {
                l2capinbuf[length++] = pSerial->read();
                DEBUG_BTD("%02X ", (int)l2capinbuf[length - 1]);
        }
        len = (l2capinbuf[3] << 8) | l2capinbuf[2];
        for (int i = 0; i < len; i++)
        {
                l2capinbuf[length++] = pSerial->read();
                //DEBUG_BTD("%02X ", (int)l2capinbuf[length - 1]);
        }
        DEBUG_BTD("\n");

        if (length > 0)
        {
                for (int i = 0; i < BTD_NUM_SERVICES; i++)
                {
                        if (btService[i])
                        {
                                //DEBUG_BTD("[BTD] Service[ %d ].ACLData\n", i);
                                btService[i]->ACLData(l2capinbuf);
                        }
                        else
                        {
                                //DEBUG_BTD("[BTD] Service[ %d ].ACLData.NULL\n", i);
                        }
                }
        }

        for (uint8_t i = 0; i < BTD_NUM_SERVICES; i++)
        {
                if (btService[i])
                {
                        //DEBUG_BTD("[BTD] Service[ %d ].Run\n", i);
                        btService[i]->Run();
                }
                else
                {
                        //DEBUG_BTD("[BTD] Service[ %d ].Run.NULL\n", i);
                }
        }
}

/************************************************************/
/*                    HCI Commands                          */
/************************************************************/

void BTD::HCI_Command(uint8_t *data, uint16_t nbytes)
{
        hci_clear_flag(HCI_FLAG_CMD_COMPLETE);

        pSerial->write(HCI_COMMAND_PKT);
        pSerial->write(data, nbytes);
}

void BTD::hci_reset()
{
        hci_event_flag = 0;    // Clear all the flags
        hcibuf[0] = 0x03;      // HCI OCF = 3
        hcibuf[1] = 0x03 << 2; // HCI OGF = 3
        hcibuf[2] = 0x00;      // parameter length = 0
        HCI_Command(hcibuf, 3);
}

void BTD::hci_write_scan_enable()
{
        hci_clear_flag(HCI_FLAG_INCOMING_REQUEST);
        hcibuf[0] = 0x1A;      // HCI OCF = 1A
        hcibuf[1] = 0x03 << 2; // HCI OGF = 3
        hcibuf[2] = 0x01;      // parameter length = 1
        if (btdName != NULL)
                hcibuf[3] = 0x03; // Inquiry Scan enabled. Page Scan enabled.
        else
                hcibuf[3] = 0x02; // Inquiry Scan disabled. Page Scan enabled.
        HCI_Command(hcibuf, 4);
}

void BTD::hci_write_scan_disable()
{
        hcibuf[0] = 0x1A;      // HCI OCF = 1A
        hcibuf[1] = 0x03 << 2; // HCI OGF = 3
        hcibuf[2] = 0x01;      // parameter length = 1
        hcibuf[3] = 0x00;      // Inquiry Scan disabled. Page Scan disabled.
        HCI_Command(hcibuf, 4);
}

void BTD::hci_read_bdaddr()
{
        hci_clear_flag(HCI_FLAG_READ_BDADDR);
        hcibuf[0] = 0x09;      // HCI OCF = 9
        hcibuf[1] = 0x04 << 2; // HCI OGF = 4
        hcibuf[2] = 0x00;
        HCI_Command(hcibuf, 3);
}

void BTD::hci_read_local_version_information()
{
        hci_clear_flag(HCI_FLAG_READ_VERSION);
        hcibuf[0] = 0x01;      // HCI OCF = 1
        hcibuf[1] = 0x04 << 2; // HCI OGF = 4
        hcibuf[2] = 0x00;
        HCI_Command(hcibuf, 3);
}

void BTD::hci_read_local_extended_features(uint8_t page_number)
{
        hci_clear_flag(HCI_FLAG_LOCAL_EXTENDED_FEATURES);
        hcibuf[0] = 0x04;      // HCI OCF = 4
        hcibuf[1] = 0x04 << 2; // HCI OGF = 4
        hcibuf[2] = 0x01;      // parameter length = 1
        hcibuf[3] = page_number;
        HCI_Command(hcibuf, 4);
}

void BTD::hci_accept_connection()
{
        hci_clear_flag(HCI_FLAG_CONNECT_COMPLETE);
        hcibuf[0] = 0x09;           // HCI OCF = 9
        hcibuf[1] = 0x01 << 2;      // HCI OGF = 1
        hcibuf[2] = 0x07;           // parameter length 7
        hcibuf[3] = disc_bdaddr[0]; // 6 octet bdaddr
        hcibuf[4] = disc_bdaddr[1];
        hcibuf[5] = disc_bdaddr[2];
        hcibuf[6] = disc_bdaddr[3];
        hcibuf[7] = disc_bdaddr[4];
        hcibuf[8] = disc_bdaddr[5];
        hcibuf[9] = 0x00; // Switch role to master
        HCI_Command(hcibuf, 10);
}

void BTD::hci_remote_name()
{
        hci_clear_flag(HCI_FLAG_REMOTE_NAME_COMPLETE);
        hcibuf[0] = 0x19;           // HCI OCF = 19
        hcibuf[1] = 0x01 << 2;      // HCI OGF = 1
        hcibuf[2] = 0x0A;           // parameter length = 10
        hcibuf[3] = disc_bdaddr[0]; // 6 octet bdaddr
        hcibuf[4] = disc_bdaddr[1];
        hcibuf[5] = disc_bdaddr[2];
        hcibuf[6] = disc_bdaddr[3];
        hcibuf[7] = disc_bdaddr[4];
        hcibuf[8] = disc_bdaddr[5];
        hcibuf[9] = 0x01;  // Page Scan Repetition Mode
        hcibuf[10] = 0x00; // Reserved
        hcibuf[11] = 0x00; // Clock offset - low byte
        hcibuf[12] = 0x00; // Clock offset - high byte
        HCI_Command(hcibuf, 13);
}

void BTD::hci_write_local_name(const char *name)
{
        hcibuf[0] = 0x13;             // HCI OCF = 13
        hcibuf[1] = 0x03 << 2;        // HCI OGF = 3
        hcibuf[2] = strlen(name) + 1; // parameter length = the length of the string + end byte
        uint8_t i;
        for (i = 0; i < strlen(name); i++)
                hcibuf[i + 3] = name[i];
        hcibuf[i + 3] = 0x00; // End of string
        HCI_Command(hcibuf, 4 + strlen(name));
}

void BTD::hci_set_event_mask()
{
        hcibuf[0] = 0x01;      // HCI OCF = 01
        hcibuf[1] = 0x03 << 2; // HCI OGF = 3
        hcibuf[2] = 0x08;
        // The first 6 bytes are the default of 1FFF FFFF FFFF
        // However we need to set bits 48-55 for simple pairing to work
        hcibuf[3] = 0xFF;
        hcibuf[4] = 0xFF;
        hcibuf[5] = 0xFF;
        hcibuf[6] = 0xFF;
        hcibuf[7] = 0xFF;
        hcibuf[8] = 0x1F;
        hcibuf[9] = 0xFF; // Enable bits 48-55 used for simple pairing
        hcibuf[10] = 0x00;
        HCI_Command(hcibuf, 11);
}

void BTD::hci_write_simple_pairing_mode(bool enable)
{
        hcibuf[0] = 0x56;      // HCI OCF = 56
        hcibuf[1] = 0x03 << 2; // HCI OGF = 3
        hcibuf[2] = 1;         // parameter length = 1
        hcibuf[3] = enable ? 1 : 0;
        HCI_Command(hcibuf, 4);
}

void BTD::hci_inquiry()
{
        hci_clear_flag(HCI_FLAG_DEVICE_FOUND);
        hcibuf[0] = 0x01;
        hcibuf[1] = 0x01 << 2; // HCI OGF = 1
        hcibuf[2] = 0x05;      // Parameter Total Length = 5
        hcibuf[3] = 0x33;      // LAP: Genera/Unlimited Inquiry Access Code (GIAC = 0x9E8B33) - see https://www.bluetooth.org/Technical/AssignedNumbers/baseband.htm
        hcibuf[4] = 0x8B;
        hcibuf[5] = 0x9E;
        hcibuf[6] = 0x30; // Inquiry time = 61.44 sec (maximum)
        hcibuf[7] = 0x0A; // 10 number of responses
        HCI_Command(hcibuf, 8);
}

void BTD::hci_inquiry_cancel()
{
        hcibuf[0] = 0x02;
        hcibuf[1] = 0x01 << 2; // HCI OGF = 1
        hcibuf[2] = 0x00;      // Parameter Total Length = 0
        HCI_Command(hcibuf, 3);
}

void BTD::hci_connect()
{
        hci_connect(disc_bdaddr); // Use last discovered device
}

void BTD::hci_connect(uint8_t *bdaddr)
{
        hci_clear_flag(HCI_FLAG_CONNECT_COMPLETE | HCI_FLAG_CONNECT_EVENT);
        hcibuf[0] = 0x05;      // HCI OCF = 5
        hcibuf[1] = 0x01 << 2; // HCI OGF = 1
        hcibuf[2] = 0x0D;      // parameter Total Length = 13
        hcibuf[3] = bdaddr[0]; // 6 octet bdaddr (LSB)
        hcibuf[4] = bdaddr[1];
        hcibuf[5] = bdaddr[2];
        hcibuf[6] = bdaddr[3];
        hcibuf[7] = bdaddr[4];
        hcibuf[8] = bdaddr[5];
        hcibuf[9] = 0x18;  // DM1 or DH1 may be used
        hcibuf[10] = 0xCC; // DM3, DH3, DM5, DH5 may be used
        hcibuf[11] = 0x01; // Page repetition mode R1
        hcibuf[12] = 0x00; // Reserved
        hcibuf[13] = 0x00; // Clock offset
        hcibuf[14] = 0x00; // Invalid clock offset
        hcibuf[15] = 0x00; // Do not allow role switch

        HCI_Command(hcibuf, 16);
}

void BTD::hci_pin_code_request_reply()
{
        hcibuf[0] = 0x0D;           // HCI OCF = 0D
        hcibuf[1] = 0x01 << 2;      // HCI OGF = 1
        hcibuf[2] = 0x17;           // parameter length 23
        hcibuf[3] = disc_bdaddr[0]; // 6 octet bdaddr
        hcibuf[4] = disc_bdaddr[1];
        hcibuf[5] = disc_bdaddr[2];
        hcibuf[6] = disc_bdaddr[3];
        hcibuf[7] = disc_bdaddr[4];
        hcibuf[8] = disc_bdaddr[5];

        hcibuf[9] = strlen(btdPin); // Length of pin
        uint8_t i;
        for (i = 0; i < strlen(btdPin); i++) // The maximum size of the pin is 16
                hcibuf[i + 10] = btdPin[i];
        for (; i < 16; i++)
                hcibuf[i + 10] = 0x00; // The rest should be 0

        HCI_Command(hcibuf, 26);
}

void BTD::hci_pin_code_negative_request_reply()
{
        hcibuf[0] = 0x0E;           // HCI OCF = 0E
        hcibuf[1] = 0x01 << 2;      // HCI OGF = 1
        hcibuf[2] = 0x06;           // parameter length 6
        hcibuf[3] = disc_bdaddr[0]; // 6 octet bdaddr
        hcibuf[4] = disc_bdaddr[1];
        hcibuf[5] = disc_bdaddr[2];
        hcibuf[6] = disc_bdaddr[3];
        hcibuf[7] = disc_bdaddr[4];
        hcibuf[8] = disc_bdaddr[5];

        HCI_Command(hcibuf, 9);
}

void BTD::hci_link_key_request_negative_reply()
{
        hcibuf[0] = 0x0C;           // HCI OCF = 0C
        hcibuf[1] = 0x01 << 2;      // HCI OGF = 1
        hcibuf[2] = 0x06;           // parameter length 6
        hcibuf[3] = disc_bdaddr[0]; // 6 octet bdaddr
        hcibuf[4] = disc_bdaddr[1];
        hcibuf[5] = disc_bdaddr[2];
        hcibuf[6] = disc_bdaddr[3];
        hcibuf[7] = disc_bdaddr[4];
        hcibuf[8] = disc_bdaddr[5];

        HCI_Command(hcibuf, 9);
}

void BTD::hci_io_capability_request_reply()
{
        hcibuf[0] = 0x2B;      // HCI OCF = 2B
        hcibuf[1] = 0x01 << 2; // HCI OGF = 1
        hcibuf[2] = 0x09;
        hcibuf[3] = disc_bdaddr[0]; // 6 octet bdaddr
        hcibuf[4] = disc_bdaddr[1];
        hcibuf[5] = disc_bdaddr[2];
        hcibuf[6] = disc_bdaddr[3];
        hcibuf[7] = disc_bdaddr[4];
        hcibuf[8] = disc_bdaddr[5];
        hcibuf[9] = 0x03;  // NoInputNoOutput
        hcibuf[10] = 0x00; // OOB authentication data not present
        hcibuf[11] = 0x00; // MITM Protection Not Required – No Bonding. Numeric comparison with automatic accept allowed

        HCI_Command(hcibuf, 12);
}

void BTD::hci_user_confirmation_request_reply()
{
        hcibuf[0] = 0x2C;           // HCI OCF = 2C
        hcibuf[1] = 0x01 << 2;      // HCI OGF = 1
        hcibuf[2] = 0x06;           // parameter length 6
        hcibuf[3] = disc_bdaddr[0]; // 6 octet bdaddr
        hcibuf[4] = disc_bdaddr[1];
        hcibuf[5] = disc_bdaddr[2];
        hcibuf[6] = disc_bdaddr[3];
        hcibuf[7] = disc_bdaddr[4];
        hcibuf[8] = disc_bdaddr[5];

        HCI_Command(hcibuf, 9);
}

void BTD::hci_authentication_request()
{
        hcibuf[0] = 0x11;                                // HCI OCF = 11
        hcibuf[1] = 0x01 << 2;                           // HCI OGF = 1
        hcibuf[2] = 0x02;                                // parameter length = 2
        hcibuf[3] = (uint8_t)(hci_handle & 0xFF);        // connection handle - low byte
        hcibuf[4] = (uint8_t)((hci_handle >> 8) & 0x0F); // connection handle - high byte

        HCI_Command(hcibuf, 5);
}

void BTD::hci_disconnect(uint16_t handle)
{
        // This is called by the different services
        hci_clear_flag(HCI_FLAG_DISCONNECT_COMPLETE);
        hcibuf[0] = 0x06;                            // HCI OCF = 6
        hcibuf[1] = 0x01 << 2;                       // HCI OGF = 1
        hcibuf[2] = 0x03;                            // parameter length = 3
        hcibuf[3] = (uint8_t)(handle & 0xFF);        // connection handle - low byte
        hcibuf[4] = (uint8_t)((handle >> 8) & 0x0F); // connection handle - high byte
        hcibuf[5] = 0x13;                            // reason

        HCI_Command(hcibuf, 6);
}

void BTD::hci_write_class_of_device()
{
        // See http://bluetooth-pentest.narod.ru/software/bluetooth_class_of_device-service_generator.html
        hcibuf[0] = 0x24;      // HCI OCF = 24
        hcibuf[1] = 0x03 << 2; // HCI OGF = 3

        hcibuf[2] = 0x03; // parameter length = 3

        // WiZIO
        hcibuf[3] = 0x00; // Minor Device Class                     //// Toy-Robot ... x804
        hcibuf[4] = 0x02; // Major Device Class Phone x200
        hcibuf[5] = 0x00; // Major Service Class

        HCI_Command(hcibuf, 6);
}

/************************************************************/
/*                    L2CAP Commands                        */
/************************************************************/

void BTD::L2CAP_Command(uint16_t handle, uint8_t *data, uint8_t nbytes, uint8_t channelLow, uint8_t channelHigh)
{
        uint8_t frame[8];
        frame[0] = (uint8_t)(handle & 0xff);                 // HCI handle with PB,BC flag
        frame[1] = (uint8_t)(((handle >> 8) & 0x0f) | 0x20); //
        frame[2] = (uint8_t)((4 + nbytes) & 0xff);           // HCI ACL total data length
        frame[3] = (uint8_t)((4 + nbytes) >> 8);             //
        frame[4] = (uint8_t)(nbytes & 0xff);                 // L2CAP header: Length
        frame[5] = (uint8_t)(nbytes >> 8);                   //
        frame[6] = channelLow;                               //
        frame[7] = channelHigh;                              //

        pSerial->write(HCI_ACLDATA_PKT);
        pSerial->write(frame, 8); // L2CAP C-frame
        pSerial->write(data, nbytes);
}

void BTD::l2cap_connection_request(uint16_t handle, uint8_t rxid, uint8_t *scid, uint16_t psm)
{
        l2capoutbuf[0] = L2CAP_CMD_CONNECTION_REQUEST; // Code
        l2capoutbuf[1] = rxid;                         // Identifier
        l2capoutbuf[2] = 0x04;                         // Length
        l2capoutbuf[3] = 0x00;                         //
        l2capoutbuf[4] = (uint8_t)(psm & 0xff);        // PSM
        l2capoutbuf[5] = (uint8_t)(psm >> 8);          //
        l2capoutbuf[6] = scid[0];                      // Source CID
        l2capoutbuf[7] = scid[1];                      //

        L2CAP_Command(handle, l2capoutbuf, 8);
}

void BTD::l2cap_connection_response(uint16_t handle, uint8_t rxid, uint8_t *dcid, uint8_t *scid, uint8_t result)
{
        l2capoutbuf[0] = L2CAP_CMD_CONNECTION_RESPONSE; // Code
        l2capoutbuf[1] = rxid;                          // Identifier
        l2capoutbuf[2] = 0x08;                          // Length
        l2capoutbuf[3] = 0x00;                          //
        l2capoutbuf[4] = dcid[0];                       // Destination CID
        l2capoutbuf[5] = dcid[1];                       //
        l2capoutbuf[6] = scid[0];                       // Source CID
        l2capoutbuf[7] = scid[1];                       //
        l2capoutbuf[8] = result;                        // Result: Pending or Success
        l2capoutbuf[9] = 0x00;                          //
        l2capoutbuf[10] = 0x00;                         // No further information
        l2capoutbuf[11] = 0x00;                         //

        L2CAP_Command(handle, l2capoutbuf, 12);
}

void BTD::l2cap_config_request(uint16_t handle, uint8_t rxid, uint8_t *dcid)
{
        l2capoutbuf[0] = L2CAP_CMD_CONFIG_REQUEST; // Code
        l2capoutbuf[1] = rxid;                     // Identifier
        l2capoutbuf[2] = 0x08;                     // Length
        l2capoutbuf[3] = 0x00;                     //
        l2capoutbuf[4] = dcid[0];                  // Destination CID
        l2capoutbuf[5] = dcid[1];                  //
        l2capoutbuf[6] = 0x00;                     // Flags
        l2capoutbuf[7] = 0x00;                     //
        l2capoutbuf[8] = 0x01;                     // Config Opt: type = MTU (Maximum Transmission Unit) - Hint
        l2capoutbuf[9] = 0x02;                     // Config Opt: length
        l2capoutbuf[10] = 0xFF;                    // MTU
        l2capoutbuf[11] = 0xFF;                    //

        L2CAP_Command(handle, l2capoutbuf, 12);
}

void BTD::l2cap_config_response(uint16_t handle, uint8_t rxid, uint8_t *scid)
{
        l2capoutbuf[0] = L2CAP_CMD_CONFIG_RESPONSE; // Code
        l2capoutbuf[1] = rxid;                      // Identifier
        l2capoutbuf[2] = 0x0A;                      // Length
        l2capoutbuf[3] = 0x00;                      //
        l2capoutbuf[4] = scid[0];                   // Source CID
        l2capoutbuf[5] = scid[1];                   //
        l2capoutbuf[6] = 0x00;                      // Flag
        l2capoutbuf[7] = 0x00;                      //
        l2capoutbuf[8] = 0x00;                      // Result
        l2capoutbuf[9] = 0x00;                      //
        l2capoutbuf[10] = 0x01;                     // Config
        l2capoutbuf[11] = 0x02;                     //
        l2capoutbuf[12] = 0xA0;                     //
        l2capoutbuf[13] = 0x02;                     //

        L2CAP_Command(handle, l2capoutbuf, 14);
}

void BTD::l2cap_disconnection_request(uint16_t handle, uint8_t rxid, uint8_t *dcid, uint8_t *scid)
{
        l2capoutbuf[0] = L2CAP_CMD_DISCONNECT_REQUEST; // Code
        l2capoutbuf[1] = rxid;                         // Identifier
        l2capoutbuf[2] = 0x04;                         // Length
        l2capoutbuf[3] = 0x00;
        l2capoutbuf[4] = dcid[0];
        l2capoutbuf[5] = dcid[1];
        l2capoutbuf[6] = scid[0];
        l2capoutbuf[7] = scid[1];

        L2CAP_Command(handle, l2capoutbuf, 8);
}

void BTD::l2cap_disconnection_response(uint16_t handle, uint8_t rxid, uint8_t *dcid, uint8_t *scid)
{
        l2capoutbuf[0] = L2CAP_CMD_DISCONNECT_RESPONSE; // Code
        l2capoutbuf[1] = rxid;                          // Identifier
        l2capoutbuf[2] = 0x04;                          // Length
        l2capoutbuf[3] = 0x00;
        l2capoutbuf[4] = dcid[0];
        l2capoutbuf[5] = dcid[1];
        l2capoutbuf[6] = scid[0];
        l2capoutbuf[7] = scid[1];

        L2CAP_Command(handle, l2capoutbuf, 8);
}

void BTD::l2cap_information_response(uint16_t handle, uint8_t rxid, uint8_t infoTypeLow, uint8_t infoTypeHigh)
{
        l2capoutbuf[0] = L2CAP_CMD_INFORMATION_RESPONSE; // Code
        l2capoutbuf[1] = rxid;                           // Identifier
        l2capoutbuf[2] = 0x08;                           // Length
        l2capoutbuf[3] = 0x00;
        l2capoutbuf[4] = infoTypeLow;
        l2capoutbuf[5] = infoTypeHigh;
        l2capoutbuf[6] = 0x00; // Result = success
        l2capoutbuf[7] = 0x00; // Result = success
        l2capoutbuf[8] = 0x00;
        l2capoutbuf[9] = 0x00;
        l2capoutbuf[10] = 0x00;
        l2capoutbuf[11] = 0x00;

        L2CAP_Command(handle, l2capoutbuf, 12);
}
