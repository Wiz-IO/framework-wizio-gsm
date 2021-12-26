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

#ifndef _btd_h_
#define _btd_h_

#include <Arduino.h>
#include <BT_HCI_Serial.h>

#define DEBUG_BTD 
//::printf

#define BULK_MAXPKTSIZE                         256 // Max size for ACL data

/* HCI data types */
#define HCI_COMMAND_PKT		                0x01
#define HCI_ACLDATA_PKT		                0x02
#define HCI_SCODATA_PKT		                0x03
#define HCI_EVENT_PKT		                0x04
#define HCI_DIAG_PKT		                0xF0
#define HCI_VENDOR_PKT		                0xFF

/* Bluetooth HCI states for hci_task() */
#define HCI_INIT_STATE                          0
#define HCI_RESET_STATE                         1
#define HCI_CLASS_STATE                         2
#define HCI_BDADDR_STATE                        3
#define HCI_LOCAL_VERSION_STATE                 4
#define HCI_WRITE_NAME_STATE                    5
#define HCI_CHECK_DEVICE_SERVICE                6
#define HCI_INQUIRY_STATE                       7 // next three states are only used if it should pair and connect to a device
#define HCI_CONNECT_DEVICE_STATE                8
#define HCI_CONNECTED_DEVICE_STATE              9
#define HCI_SCANNING_STATE                      10
#define HCI_CONNECT_IN_STATE                    11
#define HCI_REMOTE_NAME_STATE                   12
#define HCI_CONNECTED_STATE                     13
#define HCI_DISABLE_SCAN_STATE                  14 // NU
#define HCI_DONE_STATE                          15
#define HCI_DISCONNECT_STATE                    16
#define HCI_LOCAL_EXTENDED_FEATURES_STATE       17
#define HCI_WRITE_SIMPLE_PAIRING_STATE          18
#define HCI_SET_EVENT_MASK_STATE                19

/* HCI event flags*/
#define HCI_FLAG_CMD_COMPLETE                   (1UL << 0)
#define HCI_FLAG_CONNECT_COMPLETE               (1UL << 1)
#define HCI_FLAG_DISCONNECT_COMPLETE            (1UL << 2)
#define HCI_FLAG_REMOTE_NAME_COMPLETE           (1UL << 3)
#define HCI_FLAG_INCOMING_REQUEST               (1UL << 4)
#define HCI_FLAG_READ_BDADDR                    (1UL << 5)
#define HCI_FLAG_READ_VERSION                   (1UL << 6)
#define HCI_FLAG_DEVICE_FOUND                   (1UL << 7)
#define HCI_FLAG_CONNECT_EVENT                  (1UL << 8)
#define HCI_FLAG_LOCAL_EXTENDED_FEATURES        (1UL << 9)

/* Macros for HCI event flag tests */
#define hci_check_flag(flag)                    (hci_event_flag & (flag))
#define hci_set_flag(flag)                      (hci_event_flag |= (flag))
#define hci_clear_flag(flag)                    (hci_event_flag &= ~(flag))

/* HCI Events MTK BLUEANGEL */
typedef enum // blueangel sdk
{
    EV_INQUIRY_COMPLETE               = 0x01,
    EV_INQUIRY_RESULT                 = 0x02,
    EV_CONNECT_COMPLETE               = 0x03,
    EV_CONNECT_REQUEST                = 0x04,
    EV_DISCONNECT_COMPLETE            = 0x05,
    EV_AUTH_COMPLETE                  = 0x06,
    EV_REMOTE_NAME_REQ_COMPLETE       = 0x07,
    EV_ENCRYPT_CHNG                   = 0x08,
    EV_CHNG_CONN_LINK_KEY_COMPLETE    = 0x09,
    EV_MASTER_LINK_KEY_COMPLETE       = 0x0A,
    EV_READ_REMOTE_FEATURES_COMPLETE  = 0x0B,
    EV_READ_REMOTE_VERSION_COMPLETE   = 0x0C,
    EV_QOS_SETUP_COMPLETE             = 0x0D,
    EV_COMMAND_COMPLETE               = 0x0E,
    EV_COMMAND_STATUS                 = 0x0F, /*Indicates that the command has been received and is being processed in the host controller.*/
    EV_HARDWARE_ERROR                 = 0x10,
    EV_FLUSH_OCCURRED                 = 0x11,
    EV_ROLE_CHANGE                    = 0x12, /*Indicates that the current bluetooth role for a connection has been changed.*/
    EV_NUM_COMPLETED_PACKETS          = 0x13, /*Indicates to the host the number of data packets sent compared to the last time the same event was sent.*/
    EV_MODE_CHNG                      = 0x14,
    EV_RETURN_LINK_KEYS               = 0x15,
    EV_PIN_CODE_REQ                   = 0x16,
    EV_LINK_KEY_REQ                   = 0x17,
    EV_LINK_KEY_NOTIFY                = 0x18,
    EV_LOOPBACK_COMMAND               = 0x19,
    EV_DATA_BUFFER_OVERFLOW           = 0x1A,
    EV_MAX_SLOTS_CHNG                 = 0x1B, /*Informs the host when the LMP_Max_Slots parameter changes.*/
    EV_READ_CLOCK_OFFSET_COMPLETE     = 0x1C,
    EV_CONN_PACKET_TYPE_CHNG          = 0x1D, /*Indicate the completion of the packet type change for a connection.*/
    EV_QOS_VIOLATION                  = 0x1E,
    EV_PAGE_SCAN_MODE_CHANGE          = 0x1F, /* Not in 1.2 */
    EV_PAGE_SCAN_REPETITION_MODE      = 0x20,
    EV_FLOW_SPECIFICATION_COMPLETE    = 0x21, /* 1.2 */
    EV_INQUIRY_RESULT_WITH_RSSI       = 0x22, /* 1.2 */
    EV_READ_REMOTE_EXT_FEAT_COMPLETE  = 0x23, /* 1.2 */
    EV_FIXED_ADDRESS                  = 0x24, /* 1.2 */
    EV_ALIAS_ADDRESS                  = 0x25, /* 1.2 */
    EV_GENERATE_ALIAS_REQ             = 0x26, /* 1.2 */
    EV_ACTIVE_ADDRESS                 = 0x27, /* 1.2 */
    EV_ALLOW_PRIVATE_PAIRING          = 0x28, /* 1.2 */
    EV_ALIAS_ADDRESS_REQ              = 0x29, /* 1.2 */
    EV_ALIAS_NOT_RECOGNIZED           = 0x2A, /* 1.2 */
    EV_FIXED_ADDRESS_ATTEMPT          = 0x2B, /* 1.2 */
    EV_SYNC_CONNECT_COMPLETE          = 0x2C, /* 1.2 */
    EV_SYNC_CONN_CHANGED              = 0x2D, /* 1.2 */
    EV_SNIFF_SUBRATING_EVENT          = 0x2E, /* Posh_assert */
    EV_INQUIRY_RESULT_WITH_EIR        = 0x2F, /* 2.1 */
    EV_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT   = 0x30, /* Posh_assert */
    EV_IO_CAPABILITY_REQUEST          = 0x31, /* 2.1 */
    EV_IO_CAPABILITY_RESPONSE         = 0x32, /* 2.1 */
    EV_USER_CONFIRM_REQUSEST          = 0x33, /* 2.1 */
    EV_USER_PASSKEY_REQUEST_EVENT     = 0x34, /* Posh_assert */
    EV_REMOTE_OOB_DATA_REQUEST_EVENT  = 0x35, /* Posh_assert */
    EV_SIMPLE_PAIRING_COMPLETE        = 0x36, /* 2.1 */
    EV_LINK_SUPERVISION_TIMEOUT_CHNG  = 0x38, /* 2.1 */
    EV_ENHANCED_FLUSH_COMPLETE        = 0x39, /* 2.1 */
    EV_USER_PASSKEY_NOTIFICATION      = 0x3B, /* 2.1 */
    EV_USER_KEYPRESS                  = 0x3C, /* 2.1 */
    EV_REMOTE_HOST_SUPPORTED_FEATURES_NOTIFICATION_EVENT   = 0x3D, /* Posh_assert */
    EV_LE_META_EVENT                  = 0x3E, /* 4.0 */
    EV_PHYSICAL_LINK_COMPLETE         = 0x40, /* 3.0+HS */
    EV_CHANNEL_SELECTED               = 0x41, /* 3.0+HS */
    EV_DISCONNECT_PHYSICAL_LINK       = 0x42, /* 3.0+HS */
    EV_PHYSICAL_LINK_EARLY_WARNING    = 0x43, /* 3.0+HS */
    EV_PHYSICAL_LINK_RECOVERY         = 0x44, /* 3.0+HS */
    EV_LOGICAL_LINK_COMPLETE          = 0x45, /* 3.0+HS */
    EV_DISCONNECT_LOGICAL_LINK        = 0x46, /* 3.0+HS */
    EV_NUM_OF_COMPLETE_DATA_BLOCKS    = 0x48, /* 3.0+HS */
    EV_AMP_STATUS_CHANGE              = 0x4D, /* 3.0+HS */
    EV_TRIGGERED_CLOCK_CAPTURE        = 0x4E,
    EV_SYNCHRONIZATION_TRAIN_COMPLETE = 0x4F,
    EV_SYNCHRONIZATION_TRAIN_RECEIVED = 0x50,
    EV_CONECTIONLESS_SLAVE_BROADCAST_RECEIVED = 0x51,
    EV_CONECTIONLESS_SLAVE_BROADCAST_TIMEOUT = 0x52,
    EV_TRUNCATED_PAGE_COMPLETE        = 0x53,
    EV_SLAVE_PAGE_RESPONSE_TIMEOUT    = 0x54,
    EV_CONNECTIONLESS_SLAVE_BROADCAST_CHANNEL_MAP_CHANGE = 0x55,
    EV_INQUIRY_RESPONSE_NOTIFICATION  = 0x56,
    EV_AUTHENTICATED_PAYLOAD_TIMEOUT  = 0x57, /* BT4.1 */
    EV_UART_SETTING_COMPLETE          = 0xF0,
    EV_LOW_POWER_PRXM_EVENT           = 0xF2,
    EV_BLUETOOTH_LOGO                 = 0xFE,
    EV_VENDOR_SPECIFIC                = 0xFF,
} HciEventType;

/* Bluetooth states for the different Bluetooth drivers */
#define L2CAP_WAIT                      0
#define L2CAP_DONE                      1

/* Used for HID Control channel */
#define L2CAP_CONTROL_CONNECT_REQUEST   2
#define L2CAP_CONTROL_CONFIG_REQUEST    3
#define L2CAP_CONTROL_SUCCESS           4
#define L2CAP_CONTROL_DISCONNECT        5

/* Used for HID Interrupt channel */
#define L2CAP_INTERRUPT_SETUP           6
#define L2CAP_INTERRUPT_CONNECT_REQUEST 7
#define L2CAP_INTERRUPT_CONFIG_REQUEST  8
#define L2CAP_INTERRUPT_DISCONNECT      9

/* Used for SDP channel */
#define L2CAP_SDP_WAIT                  10
#define L2CAP_SDP_SUCCESS               11

/* Used for RFCOMM channel */
#define L2CAP_RFCOMM_WAIT               12
#define L2CAP_RFCOMM_SUCCESS            13

#define L2CAP_DISCONNECT_RESPONSE       14 // Used for both SDP and RFCOMM channel

/* Bluetooth states used by some drivers */
#define TURN_ON_LED                     17
#define PS3_ENABLE_SIXAXIS              18
#define WII_CHECK_MOTION_PLUS_STATE     19
#define WII_CHECK_EXTENSION_STATE       20
#define WII_INIT_MOTION_PLUS_STATE      21

/* L2CAP event flags for HID Control channel */
#define L2CAP_FLAG_CONNECTION_CONTROL_REQUEST           (1UL << 0)
#define L2CAP_FLAG_CONFIG_CONTROL_SUCCESS               (1UL << 1)
#define L2CAP_FLAG_CONTROL_CONNECTED                    (1UL << 2)
#define L2CAP_FLAG_DISCONNECT_CONTROL_RESPONSE          (1UL << 3)

/* L2CAP event flags for HID Interrupt channel */
#define L2CAP_FLAG_CONNECTION_INTERRUPT_REQUEST         (1UL << 4)
#define L2CAP_FLAG_CONFIG_INTERRUPT_SUCCESS             (1UL << 5)
#define L2CAP_FLAG_INTERRUPT_CONNECTED                  (1UL << 6)
#define L2CAP_FLAG_DISCONNECT_INTERRUPT_RESPONSE        (1UL << 7)

/* L2CAP event flags for SDP channel */
#define L2CAP_FLAG_CONNECTION_SDP_REQUEST               (1UL << 8)
#define L2CAP_FLAG_CONFIG_SDP_SUCCESS                   (1UL << 9)
#define L2CAP_FLAG_DISCONNECT_SDP_REQUEST               (1UL << 10)

/* L2CAP event flags for RFCOMM channel */
#define L2CAP_FLAG_CONNECTION_RFCOMM_REQUEST            (1UL << 11)
#define L2CAP_FLAG_CONFIG_RFCOMM_SUCCESS                (1UL << 12)
#define L2CAP_FLAG_DISCONNECT_RFCOMM_REQUEST            (1UL << 13)

#define L2CAP_FLAG_DISCONNECT_RESPONSE                  (1UL << 14)

/* Macros for L2CAP event flag tests */
#define l2cap_check_flag(flag)                          (l2cap_event_flag & (flag))
#define l2cap_set_flag(flag)                            (l2cap_event_flag |= (flag))
#define l2cap_clear_flag(flag)                          (l2cap_event_flag &= ~(flag))

/* L2CAP signaling commands */
#define L2CAP_CMD_COMMAND_REJECT                        0x01
#define L2CAP_CMD_CONNECTION_REQUEST                    0x02
#define L2CAP_CMD_CONNECTION_RESPONSE                   0x03
#define L2CAP_CMD_CONFIG_REQUEST                        0x04
#define L2CAP_CMD_CONFIG_RESPONSE                       0x05
#define L2CAP_CMD_DISCONNECT_REQUEST                    0x06
#define L2CAP_CMD_DISCONNECT_RESPONSE                   0x07
#define L2CAP_CMD_INFORMATION_REQUEST                   0x0A
#define L2CAP_CMD_INFORMATION_RESPONSE                  0x0B

// Used For Connection Response - Remember to Include High Byte
#define PENDING                                         0x01
#define SUCCESSFUL                                      0x00

/* Bluetooth L2CAP PSM - see http://www.bluetooth.org/Technical/AssignedNumbers/logical_link.htm */
#define SDP_PSM                                         0x01 // Service Discovery Protocol PSM Value
#define RFCOMM_PSM                                      0x03 // RFCOMM PSM Value

#define HID_CTRL_PSM                                    0x11 // HID_Control PSM Value
#define HID_INTR_PSM                                    0x13 // HID_Interrupt PSM Value

/* Used for SDP */
#define SDP_SERVICE_SEARCH_REQUEST                      0x02
#define SDP_SERVICE_SEARCH_RESPONSE                     0x03
#define SDP_SERVICE_ATTRIBUTE_REQUEST                   0x04
#define SDP_SERVICE_ATTRIBUTE_RESPONSE                  0x05
#define SDP_SERVICE_SEARCH_ATTRIBUTE_REQUEST            0x06 // See the RFCOMM specs
#define SDP_SERVICE_SEARCH_ATTRIBUTE_RESPONSE           0x07 // See the RFCOMM specs

#define PNP_INFORMATION_UUID                            0x1200
#define SERIALPORT_UUID                                 0x1101 // See http://www.bluetooth.org/Technical/AssignedNumbers/service_discovery.htm
#define L2CAP_UUID                                      0x0100

#define BTD_NUM_SERVICES                                2 // Max number of Bluetooth services - if you need more than 2 simply increase this number

class BluetoothService;

class BTD 
{
public:
        BTD(BT_HCI_Serial *p);
        uint8_t begin();
        uint8_t Release();
        uint8_t Poll();

        virtual uint8_t GetAddress() { return bAddress; };

        virtual bool isReady() { return bPollEnable; };

        /** Disconnects both the L2CAP Channel and the HCI Connection for all Bluetooth services. */
        void Disconnect();

        int8_t registerBluetoothService(BluetoothService *pService) 
        {
                for(uint8_t i = 0; i < BTD_NUM_SERVICES; i++) 
                {
                        if(!btService[i]) {
                                btService[i] = pService;
                                return i; // Return ID
                        }
                }
                return -1; // Error registering BluetoothService
        };


        void HCI_Command(uint8_t* data, uint16_t nbytes);
        void hci_reset();
        void hci_read_bdaddr();
        void hci_read_local_version_information();
        void hci_read_local_extended_features(uint8_t page_number);
        void hci_write_local_name(const char* name);
        void hci_write_simple_pairing_mode(bool enable);
        void hci_set_event_mask();
        void hci_write_scan_enable();
        void hci_write_scan_disable();
        void hci_remote_name();
        void hci_accept_connection();
        void hci_disconnect(uint16_t handle);
        void hci_pin_code_request_reply();
        void hci_pin_code_negative_request_reply();
        void hci_link_key_request_negative_reply();
        void hci_user_confirmation_request_reply();
        void hci_authentication_request();
        void hci_inquiry();
        void hci_inquiry_cancel();
        void hci_connect();
        void hci_io_capability_request_reply();
        void hci_connect(uint8_t *bdaddr);
        void hci_write_class_of_device();

        void L2CAP_Command(uint16_t handle, uint8_t* data, uint8_t nbytes, uint8_t channelLow = 0x01, uint8_t channelHigh = 0x00);
        void l2cap_connection_request(uint16_t handle, uint8_t rxid, uint8_t* scid, uint16_t psm);
        void l2cap_connection_response(uint16_t handle, uint8_t rxid, uint8_t* dcid, uint8_t* scid, uint8_t result);
        void l2cap_config_request(uint16_t handle, uint8_t rxid, uint8_t* dcid);
        void l2cap_config_response(uint16_t handle, uint8_t rxid, uint8_t* scid);
        void l2cap_disconnection_request(uint16_t handle, uint8_t rxid, uint8_t* dcid, uint8_t* scid);
        void l2cap_disconnection_response(uint16_t handle, uint8_t rxid, uint8_t* dcid, uint8_t* scid);
        void l2cap_information_response(uint16_t handle, uint8_t rxid, uint8_t infoTypeLow, uint8_t infoTypeHigh);

        /** Use this to see if it is waiting for a incoming connection. */
        bool waitingForConnection;
        
        /** This is used by the service to know when to store the device information. */
        bool l2capConnectionClaimed;

        /** This is used by the SPP library to claim the current SDP incoming request. */
        bool sdpConnectionClaimed;

        /** This is used by the SPP library to claim the current RFCOMM incoming request. */
        bool rfcommConnectionClaimed;

        /** The name you wish to make the dongle show up as. It is set automatically by the SPP library. */
        const char* btdName;

        /** The pin you wish to make the dongle use for authentication. It is set automatically by the SPP and BTHID library. */
        const char* btdPin;

        /** The bluetooth dongles Bluetooth address. */
        uint8_t my_bdaddr[6];

        /** HCI handle for the last connection. */
        uint16_t hci_handle;

        /** Last incoming devices Bluetooth address. */
        uint8_t disc_bdaddr[6];

        /** First 30 chars of last remote name. */
        char remote_name[30];

        uint8_t hci_version;

        uint8_t readPollInterval() { return pollInterval; };

        /** Used by the drivers to enable simple pairing */
        bool useSimplePairing;

protected:
        BT_HCI_Serial *pSerial;
        uint8_t bAddress;
        uint8_t bConfNum;
        uint32_t qNextPollTime;

private:
        void Initialize(); // Set all variables, endpoint structs etc. to default values
        BluetoothService *btService[BTD_NUM_SERVICES];

        uint8_t pollInterval;
        bool simple_pairing_supported;
        bool bPollEnable;

        bool checkRemoteName;           // Used to check remote device's name before connecting.
        uint8_t classOfDevice[3];       // Class of device of last device

        /* Variables used by high level HCI task */
        uint8_t  hci_state;             // Current state of Bluetooth HCI connection
        uint16_t hci_counter;           // Counter used for Bluetooth HCI reset loops
        uint16_t hci_num_reset_loops;   // This value indicate how many times it should read before trying to reset
        uint16_t hci_event_flag;        // HCI flags of received Bluetooth events
        uint8_t  inquiry_counter;

        uint8_t hcibuf[BULK_MAXPKTSIZE];        // General purpose buffer for HCI data
        uint8_t l2capinbuf[BULK_MAXPKTSIZE];    // General purpose buffer for L2CAP in data
        uint8_t l2capoutbuf[14];                // General purpose buffer for L2CAP out data

        /* State machines */
        void HCI_event_task();  // Poll the HCI event pipe
        void HCI_task();        // HCI state machine
        void ACL_event_task();  // ACL input pipe
};

/** All Bluetooth services should inherit this class. */
class BluetoothService {
public:
        BluetoothService(BTD *p) : pBtd(p) 
        {
                if(pBtd)
                        pBtd->registerBluetoothService(this); // Register it as a Bluetooth service
        };
        /** Used to pass acldata to the Bluetooth service. */
        virtual void ACLData(uint8_t* ACLData) = 0;
        /** Used to run the different state machines in the Bluetooth service. */
        virtual void Run() = 0;
        /** Used to reset the Bluetooth service. */
        virtual void Reset() = 0;
        /** Used to disconnect both the L2CAP Channel and the HCI Connection for the Bluetooth service. */
        virtual void Disconnect() = 0;
        /** Used to call your own function when the device is successfully initialized. */
        void attachOnInit(void (*funcOnInit)(void)) 
        {
                pFuncOnInit = funcOnInit; // TODO: This really belong in a class of it's own as it is repeated several times
        };

protected:
        /**
         * Called when a device is successfully initialized.
         * Use attachOnInit(void (*funcOnInit)(void)) to call your own function.
         * This is useful for instance if you want to set the LEDs in a specific way.
         */
        virtual void onInit() = 0;

        /** Used to check if the incoming L2CAP data matches the HCI Handle */
        bool checkHciHandle(uint8_t *buf, uint16_t handle) 
        {
                return (buf[0] == (handle & 0xFF)) && (buf[1] == ((handle >> 8) | 0x20));
        }

        /** Pointer to function called in onInit(). */
        void (*pFuncOnInit)(void);

        /** Pointer to BTD instance. */
        BTD *pBtd;

        /** The HCI Handle for the connection. */
        uint16_t hci_handle;

        /** L2CAP flags of received Bluetooth events. */
        uint32_t l2cap_event_flag;

        /** Identifier for L2CAP commands. */
        uint8_t identifier;
};

extern BTD BT;

#endif
