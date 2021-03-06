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

#ifndef _spp_h_
#define _spp_h_

#include "BTD.h"

/* Used for RFCOMM */
#define RFCOMM_SABM             0x2F
#define RFCOMM_UA               0x63
#define RFCOMM_UIH              0xEF
#define RFCOMM_DM               0x0F
#define RFCOMM_DISC             0x43

#define RFCOMM_EXTEND_ADDRESS   0x01 // Always 1

// Multiplexer message types
#define BT_RFCOMM_PN_CMD        0x83
#define BT_RFCOMM_PN_RSP        0x81
#define BT_RFCOMM_MSC_CMD       0xE3
#define BT_RFCOMM_MSC_RSP       0xE1
#define BT_RFCOMM_RPN_CMD       0x93
#define BT_RFCOMM_RPN_RSP       0x91

class SPP : public BluetoothService, public Stream {
public:
        /** Constructor for the SPP class. */
        SPP(BTD *p, const char *name = "Arduino", const char *pin = "0000");

        /** Get number of bytes waiting to be read.  */
        int available(void);

        /** Send out all bytes in the tx buffer. */
        void flush(void) { send(); };

        /** Used to read the next value in the buffer without advancing to the next one. */
        int peek(void);

        /** Used to read the buffer.  */
        int read(void);

        /** Writes the byte to send to a buffer. */
        size_t write(uint8_t data);

        /** Writes the bytes to send to a buffer */
        size_t write(const uint8_t* data, size_t size);
        
        using Print::write;

        /** Used to provide Boolean tests for the class. */
        operator bool() { return connected; }        

        /** Discard all the bytes in the rx buffer. */
        void discard(void);

        /** This will send all the bytes in the buffer. */
        void send(void);

        /** Used this to disconnect the virtual serial port. */
        void Disconnect();

        /** Variable used to indicate if the connection is established. */
        bool connected;

protected:
        /** Used to pass acldata to the services. */
        void ACLData(uint8_t* ACLData);

        /** Used to establish the connection automatically. */
        void Run();

        /** Use this to reset the service. */
        void Reset();
        
        /**
         * Called when a device is successfully initialized.
         * Use attachOnInit(void (*funcOnInit)(void)) to call your own function.
         * This is useful for instance if you want to set the LEDs in a specific way.
         */
        void onInit();

private:
        /* Set true when a channel is created */
        bool SDPConnected;
        bool RFCOMMConnected;

        /* Variables used by L2CAP state machines */
        uint8_t l2cap_sdp_state;
        uint8_t l2cap_rfcomm_state;

        uint8_t l2capoutbuf[BULK_MAXPKTSIZE]; // General purpose buffer for l2cap out data
        uint8_t rfcommbuf[10];                // Buffer for RFCOMM Commands

        /* L2CAP Channels */
        uint8_t sdp_scid[2];    // L2CAP source CID for SDP
        uint8_t sdp_dcid[2];    // 0x0050
        uint8_t rfcomm_scid[2]; // L2CAP source CID for RFCOMM
        uint8_t rfcomm_dcid[2]; // 0x0051

        /* RFCOMM Variables */
        uint8_t rfcommChannel;
        uint8_t rfcommChannelConnection; // This is the channel the SPP channel will be running at
        uint8_t rfcommDirection;
        uint8_t rfcommCommandResponse;
        uint8_t rfcommChannelType;
        uint8_t rfcommPfBit;

        uint32_t timer;
        bool waitForLastCommand;
        bool creditSent;

        uint8_t rfcommAvailable;
        uint8_t rfcommDataBuffer[100]; // Create a 100 sized buffer for incoming data
        uint8_t sppOutputBuffer[100];  // Create a 100 sized buffer for outgoing SPP data
        uint8_t sppIndex;

        bool firstMessage; // Used to see if it's the first SDP request received
        uint8_t bytesRead; // Counter to see when it's time to send more credit

        /* State machines */
        void SDP_task();        // SDP state machine
        void RFCOMM_task();     // RFCOMM state machine

        /* SDP Commands */
        void SDP_Command(uint8_t *data, uint8_t nbytes);
        void serviceNotSupported(uint8_t transactionIDHigh, uint8_t transactionIDLow);
        void serialPortResponse1(uint8_t transactionIDHigh, uint8_t transactionIDLow);
        void serialPortResponse2(uint8_t transactionIDHigh, uint8_t transactionIDLow);
        void l2capResponse1(uint8_t transactionIDHigh, uint8_t transactionIDLow);
        void l2capResponse2(uint8_t transactionIDHigh, uint8_t transactionIDLow);

        /* RFCOMM Commands */
        void RFCOMM_Command(uint8_t *data, uint8_t nbytes);
        void sendRfcomm(uint8_t channel, uint8_t direction, uint8_t CR, uint8_t channelType, uint8_t pfBit, uint8_t *data, uint8_t length);
        void sendRfcommCredit(uint8_t channel, uint8_t direction, uint8_t CR, uint8_t channelType, uint8_t pfBit, uint8_t credit);

        uint8_t calcFcs(uint8_t *data);
        bool    checkFcs(uint8_t *data, uint8_t fcs);
        uint8_t crc(uint8_t *data);
};

extern SPP spp;

#endif
