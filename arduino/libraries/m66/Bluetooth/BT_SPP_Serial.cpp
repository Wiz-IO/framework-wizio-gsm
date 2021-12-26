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

#include "BT_SPP_Serial.h"

SPP spp(&BT);

#define DEBUG_SPP ::printf

#define DEBUG_SPP_FUNC() 
//DEBUG_SPP("[SPP] %s()\n", __func__);

/* CRC (reversed crc) lookup table as calculated by the table generator in ETSI TS 101 369 V6.3.0. */
const uint8_t rfcomm_crc_table[256] PROGMEM =
    {/* reversed, 8-bit, poly=0x07 */
     0x00, 0x91, 0xE3, 0x72, 0x07, 0x96, 0xE4, 0x75, 0x0E, 0x9F, 0xED, 0x7C, 0x09, 0x98, 0xEA, 0x7B,
     0x1C, 0x8D, 0xFF, 0x6E, 0x1B, 0x8A, 0xF8, 0x69, 0x12, 0x83, 0xF1, 0x60, 0x15, 0x84, 0xF6, 0x67,
     0x38, 0xA9, 0xDB, 0x4A, 0x3F, 0xAE, 0xDC, 0x4D, 0x36, 0xA7, 0xD5, 0x44, 0x31, 0xA0, 0xD2, 0x43,
     0x24, 0xB5, 0xC7, 0x56, 0x23, 0xB2, 0xC0, 0x51, 0x2A, 0xBB, 0xC9, 0x58, 0x2D, 0xBC, 0xCE, 0x5F,
     0x70, 0xE1, 0x93, 0x02, 0x77, 0xE6, 0x94, 0x05, 0x7E, 0xEF, 0x9D, 0x0C, 0x79, 0xE8, 0x9A, 0x0B,
     0x6C, 0xFD, 0x8F, 0x1E, 0x6B, 0xFA, 0x88, 0x19, 0x62, 0xF3, 0x81, 0x10, 0x65, 0xF4, 0x86, 0x17,
     0x48, 0xD9, 0xAB, 0x3A, 0x4F, 0xDE, 0xAC, 0x3D, 0x46, 0xD7, 0xA5, 0x34, 0x41, 0xD0, 0xA2, 0x33,
     0x54, 0xC5, 0xB7, 0x26, 0x53, 0xC2, 0xB0, 0x21, 0x5A, 0xCB, 0xB9, 0x28, 0x5D, 0xCC, 0xBE, 0x2F,
     0xE0, 0x71, 0x03, 0x92, 0xE7, 0x76, 0x04, 0x95, 0xEE, 0x7F, 0x0D, 0x9C, 0xE9, 0x78, 0x0A, 0x9B,
     0xFC, 0x6D, 0x1F, 0x8E, 0xFB, 0x6A, 0x18, 0x89, 0xF2, 0x63, 0x11, 0x80, 0xF5, 0x64, 0x16, 0x87,
     0xD8, 0x49, 0x3B, 0xAA, 0xDF, 0x4E, 0x3C, 0xAD, 0xD6, 0x47, 0x35, 0xA4, 0xD1, 0x40, 0x32, 0xA3,
     0xC4, 0x55, 0x27, 0xB6, 0xC3, 0x52, 0x20, 0xB1, 0xCA, 0x5B, 0x29, 0xB8, 0xCD, 0x5C, 0x2E, 0xBF,
     0x90, 0x01, 0x73, 0xE2, 0x97, 0x06, 0x74, 0xE5, 0x9E, 0x0F, 0x7D, 0xEC, 0x99, 0x08, 0x7A, 0xEB,
     0x8C, 0x1D, 0x6F, 0xFE, 0x8B, 0x1A, 0x68, 0xF9, 0x82, 0x13, 0x61, 0xF0, 0x85, 0x14, 0x66, 0xF7,
     0xA8, 0x39, 0x4B, 0xDA, 0xAF, 0x3E, 0x4C, 0xDD, 0xA6, 0x37, 0x45, 0xD4, 0xA1, 0x30, 0x42, 0xD3,
     0xB4, 0x25, 0x57, 0xC6, 0xB3, 0x22, 0x50, 0xC1, 0xBA, 0x2B, 0x59, 0xC8, 0xBD, 0x2C, 0x5E, 0xCF};

SPP::SPP(BTD *p, const char *name, const char *pin) : BluetoothService(p) // Pointer to BTD class instance - mandatory
{
        pBtd->btdName = name;
        pBtd->btdPin = pin;

        /* Set device cid for the SDP and RFCOMM channelse */
        sdp_dcid[0] = 0x50;    // 0x0050
        sdp_dcid[1] = 0x00;    //
        rfcomm_dcid[0] = 0x51; // 0x0051
        rfcomm_dcid[1] = 0x00; //

        Reset();
}

void SPP::Reset()
{
        connected = false;
        RFCOMMConnected = false;
        SDPConnected = false;
        waitForLastCommand = false;
        l2cap_sdp_state = L2CAP_SDP_WAIT;
        l2cap_rfcomm_state = L2CAP_RFCOMM_WAIT;
        l2cap_event_flag = 0;
        sppIndex = 0;
        creditSent = false;
}

void SPP::Disconnect()
{
        connected = false;

        // First the two L2CAP channels has to be disconnected and then the HCI connection
        if (RFCOMMConnected)
                pBtd->l2cap_disconnection_request(hci_handle, ++identifier, rfcomm_scid, rfcomm_dcid);

        if (RFCOMMConnected && SDPConnected)
                delay(1); // Add delay between commands

        if (SDPConnected)
                pBtd->l2cap_disconnection_request(hci_handle, ++identifier, sdp_scid, sdp_dcid);

        l2cap_sdp_state = L2CAP_DISCONNECT_RESPONSE;
}

void SPP::ACLData(uint8_t *l2capinbuf)
{
        if (!connected)
        {
                if (l2capinbuf[8] == L2CAP_CMD_CONNECTION_REQUEST)
                {
                        if ((l2capinbuf[12] | (l2capinbuf[13] << 8)) == SDP_PSM && !pBtd->sdpConnectionClaimed)
                        {
                                //DEBUG_SPP("[SPP] L2CAP_CMD_CONNECTION_REQUEST: SDP PSM\n");
                                pBtd->sdpConnectionClaimed = true;
                                hci_handle = pBtd->hci_handle;    // Store the HCI Handle for the connection
                                l2cap_sdp_state = L2CAP_SDP_WAIT; // Reset state
                        }
                        else if ((l2capinbuf[12] | (l2capinbuf[13] << 8)) == RFCOMM_PSM && !pBtd->rfcommConnectionClaimed)
                        {
                                //DEBUG_SPP("[SPP] L2CAP_CMD_CONNECTION_REQUEST: RFCOMM PSM\n");
                                pBtd->rfcommConnectionClaimed = true;
                                hci_handle = pBtd->hci_handle;          // Store the HCI Handle for the connection
                                l2cap_rfcomm_state = L2CAP_RFCOMM_WAIT; // Reset state
                        }
                        else
                        {
                                //DEBUG_SPP("[ERROR] L2CAP_CMD_CONNECTION_REQUEST\n");
                        }
                }
        }

        if (checkHciHandle(l2capinbuf, hci_handle))
        {
                if ((l2capinbuf[6] | (l2capinbuf[7] << 8)) == 0x0001U) // L2CAP
                {
                        // l2cap_control - Channel ID for ACL-U
                        if (l2capinbuf[8] == L2CAP_CMD_COMMAND_REJECT)
                        {
                                //DEBUG_SPP("[ERROR] L2CAP_CMD_COMMAND_REJECT\n");
                        }
                        else if (l2capinbuf[8] == L2CAP_CMD_CONNECTION_REQUEST)
                        {
                                if ((l2capinbuf[12] | (l2capinbuf[13] << 8)) == SDP_PSM)
                                {
                                        //DEBUG_SPP("[SPP] L2CAP_CMD_CONNECTION_REQUEST: SDP_PSM\n");
                                        // It doesn't matter if it receives another reqeust,
                                        // since it waits for the channel to disconnect in the L2CAP_SDP_DONE state,
                                        // and the l2cap_event_flag will be cleared if so
                                        identifier = l2capinbuf[9];
                                        sdp_scid[0] = l2capinbuf[14];
                                        sdp_scid[1] = l2capinbuf[15];
                                        l2cap_set_flag(L2CAP_FLAG_CONNECTION_SDP_REQUEST);
                                }
                                else if ((l2capinbuf[12] | (l2capinbuf[13] << 8)) == RFCOMM_PSM)
                                {
                                        //DEBUG_SPP("[SPP] L2CAP_CMD_CONNECTION_REQUEST: RFCOMM_PSM\n");
                                        identifier = l2capinbuf[9];
                                        rfcomm_scid[0] = l2capinbuf[14];
                                        rfcomm_scid[1] = l2capinbuf[15];
                                        l2cap_set_flag(L2CAP_FLAG_CONNECTION_RFCOMM_REQUEST);
                                }
                                else
                                {
                                        //DEBUG_SPP("[ERROR] L2CAP_CMD_CONNECTION_REQUEST\n");
                                }
                        }
                        else if (l2capinbuf[8] == L2CAP_CMD_CONFIG_RESPONSE)
                        {
                                if ((l2capinbuf[16] | (l2capinbuf[17] << 8)) == 0x0000)
                                {
                                        // Success
                                        if (l2capinbuf[12] == sdp_dcid[0] && l2capinbuf[13] == sdp_dcid[1])
                                        {
                                                DEBUG_SPP("[SPP] SDP Configuration Complete\n");
                                                l2cap_set_flag(L2CAP_FLAG_CONFIG_SDP_SUCCESS);
                                        }
                                        else if (l2capinbuf[12] == rfcomm_dcid[0] && l2capinbuf[13] == rfcomm_dcid[1])
                                        {
                                                DEBUG_SPP("[SPP] RFCOMM Configuration Complete\n");
                                                l2cap_set_flag(L2CAP_FLAG_CONFIG_RFCOMM_SUCCESS);
                                        }
                                        else
                                        {
                                                //DEBUG_SPP("[ERROR] L2CAP_CMD_CONFIG_RESPONSE\n");
                                        }
                                }
                        }
                        else if (l2capinbuf[8] == L2CAP_CMD_CONFIG_REQUEST)
                        {
                                if (l2capinbuf[12] == sdp_dcid[0] && l2capinbuf[13] == sdp_dcid[1])
                                {
                                        DEBUG_SPP("[SPP] SDP Configuration Request\n");
                                        pBtd->l2cap_config_response(hci_handle, l2capinbuf[9], sdp_scid);
                                }
                                else if (l2capinbuf[12] == rfcomm_dcid[0] && l2capinbuf[13] == rfcomm_dcid[1])
                                {
                                        DEBUG_SPP("[SPP] RFCOMM Configuration Request\n");
                                        pBtd->l2cap_config_response(hci_handle, l2capinbuf[9], rfcomm_scid);
                                }
                                else
                                {
                                        //DEBUG_SPP("[ERROR] L2CAP_CMD_CONFIG_REQUEST\n");
                                }
                        }
                        else if (l2capinbuf[8] == L2CAP_CMD_DISCONNECT_REQUEST)
                        {
                                if (l2capinbuf[12] == sdp_dcid[0] && l2capinbuf[13] == sdp_dcid[1])
                                {
                                        DEBUG_SPP("[SPP] Disconnect Request SDP Channel\n");
                                        identifier = l2capinbuf[9];
                                        l2cap_set_flag(L2CAP_FLAG_DISCONNECT_SDP_REQUEST);
                                }
                                else if (l2capinbuf[12] == rfcomm_dcid[0] && l2capinbuf[13] == rfcomm_dcid[1])
                                {
                                        DEBUG_SPP("[SPP] Disconnect Request RFCOMM Channel\n");
                                        identifier = l2capinbuf[9];
                                        l2cap_set_flag(L2CAP_FLAG_DISCONNECT_RFCOMM_REQUEST);
                                }
                                else
                                {
                                        //DEBUG_SPP("[ERROR] L2CAP_CMD_DISCONNECT_REQUEST\n");
                                }
                        }
                        else if (l2capinbuf[8] == L2CAP_CMD_DISCONNECT_RESPONSE)
                        {
                                if (l2capinbuf[12] == sdp_scid[0] && l2capinbuf[13] == sdp_scid[1])
                                {
                                        DEBUG_SPP("[SPP] Disconnect Response SDP Channel\n");
                                        identifier = l2capinbuf[9];
                                        l2cap_set_flag(L2CAP_FLAG_DISCONNECT_RESPONSE);
                                }
                                else if (l2capinbuf[12] == rfcomm_scid[0] && l2capinbuf[13] == rfcomm_scid[1])
                                {
                                        DEBUG_SPP("[SPP] Disconnect Response RFCOMM Channel\n");
                                        identifier = l2capinbuf[9];
                                        l2cap_set_flag(L2CAP_FLAG_DISCONNECT_RESPONSE);
                                }
                                else
                                {
                                        //DEBUG_SPP("[ERROR] L2CAP_CMD_DISCONNECT_RESPONSE\n");
                                }
                        }
                        else if (l2capinbuf[8] == L2CAP_CMD_INFORMATION_REQUEST)
                        {
                                DEBUG_SPP("[SPP] Information request\n");
                                identifier = l2capinbuf[9];
                                pBtd->l2cap_information_response(hci_handle, identifier, l2capinbuf[12], l2capinbuf[13]);
                        }
                        else
                        {
                                //DEBUG_SPP("[ERROR] L2CAP_CMD_COMMAND\n");
                        }
                }
                else if (l2capinbuf[6] == sdp_dcid[0] && l2capinbuf[7] == sdp_dcid[1]) // SDP
                {
                        if (l2capinbuf[8] == SDP_SERVICE_SEARCH_ATTRIBUTE_REQUEST)
                        {
                                if (((l2capinbuf[16] << 8 | l2capinbuf[17]) == SERIALPORT_UUID) || ((l2capinbuf[16] << 8 | l2capinbuf[17]) == 0x0000 && (l2capinbuf[18] << 8 | l2capinbuf[19]) == SERIALPORT_UUID))
                                {
                                        // https://www.bluetooth.org/Technical/AssignedNumbers/service_discovery.htm,
                                        //DEBUG_SPP("[SPP] SDP_SERVICE_SEARCH_ATTRIBUTE_REQUEST SERIAL PORT UUID (%d)\n", (int)firstMessage);
                                        if (firstMessage)
                                        {
                                                serialPortResponse1(l2capinbuf[9], l2capinbuf[10]); // 1
                                                firstMessage = false;
                                        }
                                        else
                                        {
                                                serialPortResponse2(l2capinbuf[9], l2capinbuf[10]); // 2
                                                firstMessage = true;
                                        }
                                }
                                else if (((l2capinbuf[16] << 8 | l2capinbuf[17]) == L2CAP_UUID) || ((l2capinbuf[16] << 8 | l2capinbuf[17]) == 0x0000 && (l2capinbuf[18] << 8 | l2capinbuf[19]) == L2CAP_UUID))
                                {
                                        //DEBUG_SPP("[SPP] SDP_SERVICE_SEARCH_ATTRIBUTE_REQUEST: L2CAP UUID (%d)\n", (int)firstMessage);
                                        if (firstMessage)
                                        {
                                                l2capResponse1(l2capinbuf[9], l2capinbuf[10]);
                                                firstMessage = false;
                                        }
                                        else
                                        {
                                                l2capResponse2(l2capinbuf[9], l2capinbuf[10]);
                                                firstMessage = true;
                                        }
                                }
                                else
                                {
                                        serviceNotSupported(l2capinbuf[9], l2capinbuf[10]); // The service is not supported
                                        //DEBUG_SPP("[ERROR] SDP SERVICE\n");
                                }
                        }
                }
                else if (l2capinbuf[6] == rfcomm_dcid[0] && l2capinbuf[7] == rfcomm_dcid[1]) // RFCOMM
                {
                        rfcommChannel = l2capinbuf[8] & 0xF8;
                        rfcommDirection = l2capinbuf[8] & 0x04;
                        rfcommCommandResponse = l2capinbuf[8] & 0x02;
                        rfcommChannelType = l2capinbuf[9] & 0xEF;
                        rfcommPfBit = l2capinbuf[9] & 0x10;
                        if (rfcommChannel >> 3 != 0x00)
                        {
                                rfcommChannelConnection = rfcommChannel;
                        }
                        if (rfcommChannelType == RFCOMM_DISC)
                        {
                                DEBUG_SPP("[SPP] Received Disconnect RFCOMM Command on channel: %02X\n", (int)rfcommChannel >> 3);
                                connected = false;
                                sendRfcomm(rfcommChannel, rfcommDirection, rfcommCommandResponse, RFCOMM_UA, rfcommPfBit, rfcommbuf, 0x00); // UA Command
                        }
                        if (connected)
                        {
                                /* Read the incoming message */
                                if (rfcommChannelType == RFCOMM_UIH && rfcommChannel == rfcommChannelConnection)
                                {
                                        uint8_t length = l2capinbuf[10] >> 1;        // Get length
                                        uint8_t offset = l2capinbuf[4] - length - 4; // Check if there is credit
                                        if (checkFcs(&l2capinbuf[8], l2capinbuf[11 + length + offset]))
                                        {
                                                uint8_t i = 0;
                                                for (; i < length; i++)
                                                {
                                                        if (rfcommAvailable + i >= sizeof(rfcommDataBuffer))
                                                        {
                                                                DEBUG_SPP("[SPP] Warning: Buffer is full!\n");
                                                                break;
                                                        }
                                                        rfcommDataBuffer[rfcommAvailable + i] = l2capinbuf[11 + i + offset];
                                                }
                                                rfcommAvailable += i;
                                        }
                                        else
                                        {
                                                DEBUG_SPP("[SPP] Error in FCS checksum!\n");
                                        }

                                        // print the report send to the Arduino via Bluetooth
                                        DEBUG_SPP("[SPP] RX DATA: ");
                                        for (uint8_t i = 0; i < length; i++)
                                                DEBUG_SPP("%02X ", l2capinbuf[i + 11 + offset]);
                                        DEBUG_SPP("\n");
                                }
                                else if (rfcommChannelType == RFCOMM_UIH && l2capinbuf[11] == BT_RFCOMM_RPN_CMD)
                                {
                                        // UIH Remote Port Negotiation Command
                                        DEBUG_SPP("[SPP] Received UIH Remote Port Negotiation Command\n");
                                        rfcommbuf[0] = BT_RFCOMM_RPN_RSP; // Command
                                        rfcommbuf[1] = l2capinbuf[12];    // Length and shiftet like so: length << 1 | 1
                                        rfcommbuf[2] = l2capinbuf[13];    // Channel: channel << 1 | 1
                                        rfcommbuf[3] = l2capinbuf[14];    // Pre difined for Bluetooth, see 5.5.3 of TS 07.10 Adaption for RFCOMM
                                        rfcommbuf[4] = l2capinbuf[15];    // Priority
                                        rfcommbuf[5] = l2capinbuf[16];    // Timer
                                        rfcommbuf[6] = l2capinbuf[17];    // Max Fram Size LSB
                                        rfcommbuf[7] = l2capinbuf[18];    // Max Fram Size MSB
                                        rfcommbuf[8] = l2capinbuf[19];    // MaxRatransm.
                                        rfcommbuf[9] = l2capinbuf[20];    // Number of Frames

                                        sendRfcomm(rfcommChannel, rfcommDirection, 0, RFCOMM_UIH, rfcommPfBit, rfcommbuf, 0x0A); // UIH Remote Port Negotiation Response
                                }
                                else if (rfcommChannelType == RFCOMM_UIH && l2capinbuf[11] == BT_RFCOMM_MSC_CMD)
                                {
                                        // UIH Modem Status Command
                                        DEBUG_SPP("[SPP] Send UIH Modem Status Response\n");
                                        rfcommbuf[0] = BT_RFCOMM_MSC_RSP; // UIH Modem Status Response
                                        rfcommbuf[1] = 2 << 1 | 1;        // Length and shiftet like so: length << 1 | 1
                                        rfcommbuf[2] = l2capinbuf[13];    // Channel: (1 << 0) | (1 << 1) | (0 << 2) | (channel << 3)
                                        rfcommbuf[3] = l2capinbuf[14];    //

                                        sendRfcomm(rfcommChannel, rfcommDirection, 0, RFCOMM_UIH, rfcommPfBit, rfcommbuf, 0x04);
                                }
                        }
                        else
                        {
                                if (rfcommChannelType == RFCOMM_SABM)
                                {
                                        // SABM Command - this is sent twice: once for channel 0 and then for the channel to establish
                                        DEBUG_SPP("[SPP] Received SABM Command\n");

                                        sendRfcomm(rfcommChannel, rfcommDirection, rfcommCommandResponse, RFCOMM_UA, rfcommPfBit, rfcommbuf, 0x00); // UA Command
                                }
                                else if (rfcommChannelType == RFCOMM_UIH && l2capinbuf[11] == BT_RFCOMM_PN_CMD)
                                {
                                        // UIH Parameter Negotiation Command
                                        DEBUG_SPP("[SPP] Received UIH Parameter Negotiation Command\n");
                                        rfcommbuf[0] = BT_RFCOMM_PN_RSP;     // UIH Parameter Negotiation Response
                                        rfcommbuf[1] = l2capinbuf[12];       // Length and shiftet like so: length << 1 | 1
                                        rfcommbuf[2] = l2capinbuf[13];       // Channel: channel << 1 | 1
                                        rfcommbuf[3] = 0xE0;                 // Pre difined for Bluetooth, see 5.5.3 of TS 07.10 Adaption for RFCOMM
                                        rfcommbuf[4] = 0x00;                 // Priority
                                        rfcommbuf[5] = 0x00;                 // Timer
                                        rfcommbuf[6] = BULK_MAXPKTSIZE - 14; // Max Fram Size LSB - set to the size of received data (50)
                                        rfcommbuf[7] = 0x00;                 // Max Fram Size MSB
                                        rfcommbuf[8] = 0x00;                 // MaxRatransm.
                                        rfcommbuf[9] = 0x00;                 // Number of Frames

                                        sendRfcomm(rfcommChannel, rfcommDirection, 0, RFCOMM_UIH, rfcommPfBit, rfcommbuf, 0x0A);
                                }
                                else if (rfcommChannelType == RFCOMM_UIH && l2capinbuf[11] == BT_RFCOMM_MSC_CMD)
                                {
                                        // UIH Modem Status Command
                                        DEBUG_SPP("[SPP] Send UIH Modem Status Response\n");
                                        rfcommbuf[0] = BT_RFCOMM_MSC_RSP; // UIH Modem Status Response
                                        rfcommbuf[1] = 2 << 1 | 1;        // Length and shiftet like so: length << 1 | 1
                                        rfcommbuf[2] = l2capinbuf[13];    // Channel: (1 << 0) | (1 << 1) | (0 << 2) | (channel << 3)
                                        rfcommbuf[3] = l2capinbuf[14];    //

                                        sendRfcomm(rfcommChannel, rfcommDirection, 0, RFCOMM_UIH, rfcommPfBit, rfcommbuf, 0x04);

                                        delay(1);

                                        DEBUG_SPP("[SPP] Send UIH Modem Status Command\n");
                                        rfcommbuf[0] = BT_RFCOMM_MSC_CMD; // UIH Modem Status Command
                                        rfcommbuf[1] = 2 << 1 | 1;        // Length and shiftet like so: length << 1 | 1
                                        rfcommbuf[2] = l2capinbuf[13];    // Channel: (1 << 0) | (1 << 1) | (0 << 2) | (channel << 3)
                                        rfcommbuf[3] = 0x8D;              // Can receive frames (YES), Ready to Communicate (YES), Ready to Receive (YES), Incomig Call (NO), Data is Value (YES)

                                        sendRfcomm(rfcommChannel, rfcommDirection, 0, RFCOMM_UIH, rfcommPfBit, rfcommbuf, 0x04);
                                }
                                else if (rfcommChannelType == RFCOMM_UIH && l2capinbuf[11] == BT_RFCOMM_MSC_RSP)
                                {
                                        // UIH Modem Status Response
                                        if (!creditSent)
                                        {
                                                DEBUG_SPP("[SPP] Send UIH Command with credit\n");
                                                sendRfcommCredit(rfcommChannelConnection, rfcommDirection, 0, RFCOMM_UIH, 0x10, sizeof(rfcommDataBuffer)); // Send credit
                                                creditSent = true;
                                                timer = (uint32_t)millis();
                                                waitForLastCommand = true;
                                        }
                                }
                                else if (rfcommChannelType == RFCOMM_UIH && l2capinbuf[10] == 0x01)
                                {
                                        // UIH Command with credit
                                        DEBUG_SPP("[SPP] Received UIH Command with credit\n");
                                }
                                else if (rfcommChannelType == RFCOMM_UIH && l2capinbuf[11] == BT_RFCOMM_RPN_CMD)
                                {
                                        // UIH Remote Port Negotiation Command
                                        DEBUG_SPP("[SPP] Received UIH Remote Port Negotiation Command\n");

                                        rfcommbuf[0] = BT_RFCOMM_RPN_RSP; // Command
                                        rfcommbuf[1] = l2capinbuf[12];    // Length and shiftet like so: length << 1 | 1
                                        rfcommbuf[2] = l2capinbuf[13];    // Channel: channel << 1 | 1
                                        rfcommbuf[3] = l2capinbuf[14];    // Pre difined for Bluetooth, see 5.5.3 of TS 07.10 Adaption for RFCOMM
                                        rfcommbuf[4] = l2capinbuf[15];    // Priority
                                        rfcommbuf[5] = l2capinbuf[16];    // Timer
                                        rfcommbuf[6] = l2capinbuf[17];    // Max Fram Size LSB
                                        rfcommbuf[7] = l2capinbuf[18];    // Max Fram Size MSB
                                        rfcommbuf[8] = l2capinbuf[19];    // MaxRatransm.
                                        rfcommbuf[9] = l2capinbuf[20];    // Number of Frames

                                        sendRfcomm(rfcommChannel, rfcommDirection, 0, RFCOMM_UIH, rfcommPfBit, rfcommbuf, 0x0A); // UIH Remote Port Negotiation Response
                                        
                                        DEBUG_SPP("[SPP] RFCOMM Connection is now established\n");
                                        onInit();
                                }
                                else
                                {
                                        //DEBUG_SPP("[ERROR] RFCOMM\n");
                                }
                        }
                }
                else
                {
                        //DEBUG_SPP("[ERROR] SPP Command\n");
                }
                SDP_task();
                RFCOMM_task();
        }
        else
        {
                //DEBUG_SPP("[ERROR] SPP Handle\n");
        }
}

void SPP::Run()
{
        if (waitForLastCommand && (int32_t)((uint32_t)millis() - timer) > 100)
        {
                // We will only wait 100ms and see if the UIH Remote Port Negotiation Command is send, as some deviced don't send it
                DEBUG_SPP("[SPP] RFCOMM Connection is now established - Automatic\n");
                onInit();
        }
        send(); // Send all bytes currently in the buffer
}

void SPP::onInit()
{
        creditSent = false;
        waitForLastCommand = false;
        connected = true; // The RFCOMM channel is now established
        sppIndex = 0;
        if (pFuncOnInit)
                pFuncOnInit(); // Call the user function
};

void SPP::SDP_task()
{
        switch (l2cap_sdp_state)
        {
        case L2CAP_SDP_WAIT:
        {
                if (l2cap_check_flag(L2CAP_FLAG_CONNECTION_SDP_REQUEST))
                {
                        DEBUG_SPP("[SPP] SDP Incoming Connection Request\n");
                        l2cap_clear_flag(L2CAP_FLAG_CONNECTION_SDP_REQUEST); // Clear flag
                        pBtd->l2cap_connection_response(hci_handle, identifier, sdp_dcid, sdp_scid, PENDING);
                        delay(1);
                        pBtd->l2cap_connection_response(hci_handle, identifier, sdp_dcid, sdp_scid, SUCCESSFUL);
                        identifier++;
                        delay(1);
                        pBtd->l2cap_config_request(hci_handle, identifier, sdp_scid);
                        l2cap_sdp_state = L2CAP_SDP_SUCCESS;
                }
                else if (l2cap_check_flag(L2CAP_FLAG_DISCONNECT_SDP_REQUEST))
                {
                        DEBUG_SPP("[SPP] Disconnected SDP Channel\n");
                        l2cap_clear_flag(L2CAP_FLAG_DISCONNECT_SDP_REQUEST); // Clear flag
                        SDPConnected = false;
                        pBtd->l2cap_disconnection_response(hci_handle, identifier, sdp_dcid, sdp_scid);
                }
                break;
        }

        case L2CAP_SDP_SUCCESS:
        {
                if (l2cap_check_flag(L2CAP_FLAG_CONFIG_SDP_SUCCESS))
                {
                        DEBUG_SPP("[SPP] SDP Successfully Configured\n");
                        l2cap_clear_flag(L2CAP_FLAG_CONFIG_SDP_SUCCESS); // Clear flag
                        firstMessage = true;                             // Reset bool
                        SDPConnected = true;
                        l2cap_sdp_state = L2CAP_SDP_WAIT;
                }
                break;
        }

        case L2CAP_DISCONNECT_RESPONSE: // This is for both disconnection response from the RFCOMM and SDP channel if they were connected
        {
                if (l2cap_check_flag(L2CAP_FLAG_DISCONNECT_RESPONSE))
                {

                        DEBUG_SPP("[SPP] Disconnected L2CAP Connection\n");
                        pBtd->hci_disconnect(hci_handle);
                        hci_handle = -1; // Reset handle
                        Reset();
                }
                break;
        }
        }
}

void SPP::RFCOMM_task()
{
        switch (l2cap_rfcomm_state)
        {
        case L2CAP_RFCOMM_WAIT:
        {
                if (l2cap_check_flag(L2CAP_FLAG_CONNECTION_RFCOMM_REQUEST))
                {
                        DEBUG_SPP("[SPP] RFCOMM Incoming Connection Request\n");
                        l2cap_clear_flag(L2CAP_FLAG_CONNECTION_RFCOMM_REQUEST); // Clear flag
                        pBtd->l2cap_connection_response(hci_handle, identifier, rfcomm_dcid, rfcomm_scid, PENDING);
                        delay(1);
                        pBtd->l2cap_connection_response(hci_handle, identifier, rfcomm_dcid, rfcomm_scid, SUCCESSFUL);
                        identifier++;
                        delay(1);
                        pBtd->l2cap_config_request(hci_handle, identifier, rfcomm_scid);
                        l2cap_rfcomm_state = L2CAP_RFCOMM_SUCCESS;
                }
                else if (l2cap_check_flag(L2CAP_FLAG_DISCONNECT_RFCOMM_REQUEST))
                {
                        DEBUG_SPP("[SPP] Disconnected RFCOMM Channel\n");
                        l2cap_clear_flag(L2CAP_FLAG_DISCONNECT_RFCOMM_REQUEST); // Clear flag
                        RFCOMMConnected = false;
                        connected = false;
                        pBtd->l2cap_disconnection_response(hci_handle, identifier, rfcomm_dcid, rfcomm_scid);
                }
                break;
        }

        case L2CAP_RFCOMM_SUCCESS:
        {
                if (l2cap_check_flag(L2CAP_FLAG_CONFIG_RFCOMM_SUCCESS))
                {
                        DEBUG_SPP("[SPP] RFCOMM Successfully Configured\n");
                        l2cap_clear_flag(L2CAP_FLAG_CONFIG_RFCOMM_SUCCESS); // Clear flag
                        rfcommAvailable = 0;                                // Reset number of bytes available
                        bytesRead = 0;                                      // Reset number of bytes received
                        RFCOMMConnected = true;
                        l2cap_rfcomm_state = L2CAP_RFCOMM_WAIT;
                }
                break;
        }
        }
}

/************************************************************/
/*                    SDP Commands                          */

/************************************************************/

void SPP::SDP_Command(uint8_t *data, uint8_t nbytes)
{
        //DEBUG_SPP_FUNC();
        pBtd->L2CAP_Command(hci_handle, data, nbytes, sdp_scid[0], sdp_scid[1]);
}

void SPP::serviceNotSupported(uint8_t transactionIDHigh, uint8_t transactionIDLow)
{
        // See page 235 in the Bluetooth specs
        //DEBUG_SPP_FUNC();
        l2capoutbuf[0] = SDP_SERVICE_SEARCH_ATTRIBUTE_RESPONSE;
        l2capoutbuf[1] = transactionIDHigh;
        l2capoutbuf[2] = transactionIDLow;
        l2capoutbuf[3] = 0x00; // MSB Parameter Length
        l2capoutbuf[4] = 0x05; // LSB Parameter Length = 5
        l2capoutbuf[5] = 0x00; // MSB AttributeListsByteCount
        l2capoutbuf[6] = 0x02; // LSB AttributeListsByteCount = 2

        /* Attribute ID/Value Sequence: */
        l2capoutbuf[7] = 0x35; // Data element sequence - length in next byte
        l2capoutbuf[8] = 0x00; // Length = 0
        l2capoutbuf[9] = 0x00; // No continuation state

        SDP_Command(l2capoutbuf, 10);
}

void SPP::serialPortResponse1(uint8_t transactionIDHigh, uint8_t transactionIDLow)
{
        //DEBUG_SPP_FUNC();
        l2capoutbuf[0] = SDP_SERVICE_SEARCH_ATTRIBUTE_RESPONSE;
        l2capoutbuf[1] = transactionIDHigh;
        l2capoutbuf[2] = transactionIDLow;
        l2capoutbuf[3] = 0x00; // MSB Parameter Length
        l2capoutbuf[4] = 0x2B; // LSB Parameter Length = 43
        l2capoutbuf[5] = 0x00; // MSB AttributeListsByteCount
        l2capoutbuf[6] = 0x26; // LSB AttributeListsByteCount = 38

        /* Attribute ID/Value Sequence: */
        l2capoutbuf[7] = 0x36; // Data element sequence - length in next two bytes
        l2capoutbuf[8] = 0x00; // MSB Length
        l2capoutbuf[9] = 0x3C; // LSB Length = 60

        l2capoutbuf[10] = 0x36; // Data element sequence - length in next two bytes
        l2capoutbuf[11] = 0x00; // MSB Length
        l2capoutbuf[12] = 0x39; // LSB Length = 57

        l2capoutbuf[13] = 0x09; // Unsigned Integer - length 2 bytes
        l2capoutbuf[14] = 0x00; // MSB ServiceRecordHandle
        l2capoutbuf[15] = 0x00; // LSB ServiceRecordHandle
        l2capoutbuf[16] = 0x0A; // Unsigned int - length 4 bytes
        l2capoutbuf[17] = 0x00; // ServiceRecordHandle value - TODO: Is this related to HCI_Handle?
        l2capoutbuf[18] = 0x01;
        l2capoutbuf[19] = 0x00;
        l2capoutbuf[20] = 0x06;

        l2capoutbuf[21] = 0x09; // Unsigned Integer - length 2 bytes
        l2capoutbuf[22] = 0x00; // MSB ServiceClassIDList
        l2capoutbuf[23] = 0x01; // LSB ServiceClassIDList
        l2capoutbuf[24] = 0x35; // Data element sequence - length in next byte
        l2capoutbuf[25] = 0x03; // Length = 3
        l2capoutbuf[26] = 0x19; // UUID (universally unique identifier) - length = 2 bytes
        l2capoutbuf[27] = 0x11; // MSB SerialPort
        l2capoutbuf[28] = 0x01; // LSB SerialPort

        l2capoutbuf[29] = 0x09; // Unsigned Integer - length 2 bytes
        l2capoutbuf[30] = 0x00; // MSB ProtocolDescriptorList
        l2capoutbuf[31] = 0x04; // LSB ProtocolDescriptorList
        l2capoutbuf[32] = 0x35; // Data element sequence - length in next byte
        l2capoutbuf[33] = 0x0C; // Length = 12

        l2capoutbuf[34] = 0x35; // Data element sequence - length in next byte
        l2capoutbuf[35] = 0x03; // Length = 3
        l2capoutbuf[36] = 0x19; // UUID (universally unique identifier) - length = 2 bytes
        l2capoutbuf[37] = 0x01; // MSB L2CAP
        l2capoutbuf[38] = 0x00; // LSB L2CAP

        l2capoutbuf[39] = 0x35; // Data element sequence - length in next byte
        l2capoutbuf[40] = 0x05; // Length = 5
        l2capoutbuf[41] = 0x19; // UUID (universally unique identifier) - length = 2 bytes
        l2capoutbuf[42] = 0x00; // MSB RFCOMM
        l2capoutbuf[43] = 0x03; // LSB RFCOMM
        l2capoutbuf[44] = 0x08; // Unsigned Integer - length 1 byte

        l2capoutbuf[45] = 0x02; // ContinuationState - Two more bytes
        l2capoutbuf[46] = 0x00; // MSB length
        l2capoutbuf[47] = 0x19; // LSB length = 25 more bytes to come

        SDP_Command(l2capoutbuf, 48);
}

void SPP::serialPortResponse2(uint8_t transactionIDHigh, uint8_t transactionIDLow)
{
        l2capoutbuf[0] = SDP_SERVICE_SEARCH_ATTRIBUTE_RESPONSE;
        l2capoutbuf[1] = transactionIDHigh;
        l2capoutbuf[2] = transactionIDLow;
        l2capoutbuf[3] = 0x00; // MSB Parameter Length
        l2capoutbuf[4] = 0x1C; // LSB Parameter Length = 28
        l2capoutbuf[5] = 0x00; // MSB AttributeListsByteCount
        l2capoutbuf[6] = 0x19; // LSB AttributeListsByteCount = 25

        /* Attribute ID/Value Sequence: */
        l2capoutbuf[7] = 0x01; // Channel 1 - TODO: Try different values, so multiple servers can be used at once

        l2capoutbuf[8] = 0x09;  // Unsigned Integer - length 2 bytes
        l2capoutbuf[9] = 0x00;  // MSB LanguageBaseAttributeIDList
        l2capoutbuf[10] = 0x06; // LSB LanguageBaseAttributeIDList
        l2capoutbuf[11] = 0x35; // Data element sequence - length in next byte
        l2capoutbuf[12] = 0x09; // Length = 9

        // Identifier representing the natural language = en = English - see: "ISO 639:1988"
        l2capoutbuf[13] = 0x09; // Unsigned Integer - length 2 bytes
        l2capoutbuf[14] = 0x65; // 'e'
        l2capoutbuf[15] = 0x6E; // 'n'

        // "The second element of each triplet contains an identifier that specifies a character encoding used for the language"
        // Encoding is set to 106 (UTF-8) - see: http://www.iana.org/assignments/character-sets/character-sets.xhtml
        l2capoutbuf[16] = 0x09; // Unsigned Integer - length 2 bytes
        l2capoutbuf[17] = 0x00; // MSB of character encoding
        l2capoutbuf[18] = 0x6A; // LSB of character encoding (106)

        // Attribute ID that serves as the base attribute ID for the natural language in the service record
        // "To facilitate the retrieval of human-readable universal attributes in a principal language, the base attribute ID value for the primary language supported by a service record shall be 0x0100"
        l2capoutbuf[19] = 0x09; // Unsigned Integer - length 2 bytes
        l2capoutbuf[20] = 0x01;
        l2capoutbuf[21] = 0x00;

        l2capoutbuf[22] = 0x09; // Unsigned Integer - length 2 bytes
        l2capoutbuf[23] = 0x01; // MSB ServiceDescription
        l2capoutbuf[24] = 0x00; // LSB ServiceDescription

        l2capoutbuf[25] = 0x25; // Text string - length in next byte
        l2capoutbuf[26] = 0x05; // Name length
        l2capoutbuf[27] = 'C';
        l2capoutbuf[28] = 'O';
        l2capoutbuf[29] = 'M';
        l2capoutbuf[30] = '0';
        l2capoutbuf[31] = '1';
        l2capoutbuf[32] = 0x00; // No continuation state

        SDP_Command(l2capoutbuf, 33);
}

void SPP::l2capResponse1(uint8_t transactionIDHigh, uint8_t transactionIDLow)
{
        serialPortResponse1(transactionIDHigh, transactionIDLow); // These has to send all the supported functions, since it only supports virtual serialport it just sends the message again
}

void SPP::l2capResponse2(uint8_t transactionIDHigh, uint8_t transactionIDLow)
{
        serialPortResponse2(transactionIDHigh, transactionIDLow); // Same data as serialPortResponse2
}

/************************************************************/
/*                    RFCOMM Commands                       */
/************************************************************/

void SPP::RFCOMM_Command(uint8_t *data, uint8_t nbytes)
{
        //DEBUG_SPP_FUNC();
        pBtd->L2CAP_Command(hci_handle, data, nbytes, rfcomm_scid[0], rfcomm_scid[1]);
}

void SPP::sendRfcomm(uint8_t channel, uint8_t direction, uint8_t CR, uint8_t channelType, uint8_t pfBit, uint8_t *data, uint8_t length)
{
        l2capoutbuf[0] = channel | direction | CR | RFCOMM_EXTEND_ADDRESS; // RFCOMM Address
        l2capoutbuf[1] = channelType | pfBit;                              // RFCOMM Control
        l2capoutbuf[2] = length << 1 | 0x01;                               // Length and format (always 0x01 bytes format)
        uint8_t i = 0;
        for (; i < length; i++)
                l2capoutbuf[i + 3] = data[i];
        l2capoutbuf[i + 3] = calcFcs(l2capoutbuf);
        RFCOMM_Command(l2capoutbuf, length + 4);
}

void SPP::sendRfcommCredit(uint8_t channel, uint8_t direction, uint8_t CR, uint8_t channelType, uint8_t pfBit, uint8_t credit)
{
        l2capoutbuf[0] = channel | direction | CR | RFCOMM_EXTEND_ADDRESS; // RFCOMM Address
        l2capoutbuf[1] = channelType | pfBit;                              // RFCOMM Control
        l2capoutbuf[2] = 0x01;                                             // Length = 0
        l2capoutbuf[3] = credit;                                           // Credit
        l2capoutbuf[4] = calcFcs(l2capoutbuf);
        RFCOMM_Command(l2capoutbuf, 5);
}

/* --------------------------- CRC --------------------------- */

/* CRC on 2 bytes */
uint8_t SPP::crc(uint8_t *data)
{
        return (pgm_read_byte(&rfcomm_crc_table[pgm_read_byte(&rfcomm_crc_table[0xFF ^ data[0]]) ^ data[1]]));
}

/* Calculate FCS */
uint8_t SPP::calcFcs(uint8_t *data)
{
        uint8_t temp = crc(data);
        if ((data[1] & 0xEF) == RFCOMM_UIH)
                return (0xFF - temp); // FCS on 2 bytes
        else
                return (0xFF - pgm_read_byte(&rfcomm_crc_table[temp ^ data[2]])); // FCS on 3 bytes
}

/* Check FCS */
bool SPP::checkFcs(uint8_t *data, uint8_t fcs)
{
        uint8_t temp = crc(data);
        if ((data[1] & 0xEF) != RFCOMM_UIH)
                temp = pgm_read_byte(&rfcomm_crc_table[temp ^ data[2]]); // FCS on 3 bytes
        return (pgm_read_byte(&rfcomm_crc_table[temp ^ fcs]) == 0xCF);
}

/* --------------------------- Serial commands --------------------------- */

void SPP::discard(void)
{
        rfcommAvailable = 0;
}

void SPP::send()
{
        if (!connected || !sppIndex)
                return;

        uint8_t length;     // This is the length of the string we are sending
        uint8_t offset = 0; // This is used to keep track of where we are in the string

        l2capoutbuf[0] = rfcommChannelConnection | 0 | 0 | RFCOMM_EXTEND_ADDRESS; // RFCOMM Address
        l2capoutbuf[1] = RFCOMM_UIH;                                              // RFCOMM Control

        while (sppIndex)
        {                                                 // We will run this while loop until this variable is 0
                if (sppIndex > (sizeof(l2capoutbuf) - 4)) // Check if the string is larger than the outgoing buffer
                {
                        length = sizeof(l2capoutbuf) - 4;
                }
                else
                {
                        length = sppIndex;
                }

                l2capoutbuf[2] = length << 1 | 1; // Length

                uint8_t i = 0;
                for (; i < length; i++)
                {
                        l2capoutbuf[i + 3] = sppOutputBuffer[i + offset];
                }
                l2capoutbuf[i + 3] = calcFcs(l2capoutbuf); // Calculate checksum

                RFCOMM_Command(l2capoutbuf, length + 4);

                sppIndex -= length;
                offset += length; // Increment the offset
        }
}

size_t SPP::write(uint8_t data)
{
        return write(&data, 1);
}

size_t SPP::write(const uint8_t *data, size_t size)
{
        for (uint8_t i = 0; i < size; i++)
        {
                if (sppIndex >= sizeof(sppOutputBuffer) / sizeof(sppOutputBuffer[0]))
                {
                        send(); // Send the current data in the buffer
                }
                sppOutputBuffer[sppIndex++] = data[i]; // All the bytes are put into a buffer and then send using the send() function
        }
        return size;
}

int SPP::available(void)
{
        return rfcommAvailable;
};

int SPP::peek(void)
{
        if (rfcommAvailable == 0) // Don't read if there is nothing in the buffer
                return -1;
        return rfcommDataBuffer[0];
}

int SPP::read(void)
{
        if (rfcommAvailable == 0) // Don't read if there is nothing in the buffer
                return -1;

        uint8_t output = rfcommDataBuffer[0];
        for (uint8_t i = 1; i < rfcommAvailable; i++)
        {
                rfcommDataBuffer[i - 1] = rfcommDataBuffer[i]; // Shift the buffer one left
        }

        rfcommAvailable--;
        bytesRead++;

        if (bytesRead > (sizeof(rfcommDataBuffer) - 5))
        {
                // We will send the command just before it runs out of credit
                bytesRead = 0;
                sendRfcommCredit(rfcommChannelConnection, rfcommDirection, 0, RFCOMM_UIH, 0x10, sizeof(rfcommDataBuffer)); // Send more credit
        }

        return output;
}
