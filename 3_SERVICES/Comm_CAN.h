#include "driverlib.h"
#include "device.h"

//
// Defines
//
#define TXCOUNT  100000
#define MSG_DATA_LENGTH    1
#define TX_MSG_OBJ_ID      1

//
// Globals
//


void MSG_CAN(void){

    volatile unsigned long i;
    volatile uint32_t txMsgCount = 0;
    uint16_t txMsgData[1];

    GPIO_setPinConfig(DEVICE_GPIO_CFG_CANRXA);
    GPIO_setPinConfig(GPIO_71_CANTXA);

    //
    // Initialize the CAN controllers
    //
    CAN_initModule(CANA_BASE);

    //
    // Set up the CAN bus bit rate to 500kHz for each module
    // Refer to the Driver Library User Guide for information on how to set
    // tighter timing control. Additionally, consult the device data sheet
    // for more information about the CAN module clocking.
    //
    CAN_setBitRate(CANA_BASE, DEVICE_SYSCLK_FREQ, 500000, 16);

    //
    // Initialize the transmit message object used for sending CAN messages.
    // Message Object Parameters:
    //      CAN Module: A
    //      Message Object ID Number: 1
    //      Message Identifier: 0x95555555
    //      Message Frame: Extended
    //      Message Type: Transmit
    //      Message ID Mask: 0x0
    //      Message Object Flags: None
    //      Message Data Length: 4 Bytes

    //CAN_setupMessageObject(uint32_t base, uint32_t objID, uint32_t msgID,
    //CAN_MsgFrameType frame, CAN_MsgObjType msgType,
    //uint32_t msgIDMask, uint32_t flags, uint16_t msgLen)
    //
    CAN_setupMessageObject(CANA_BASE, TX_MSG_OBJ_ID, 0x1,
                           CAN_MSG_FRAME_STD, CAN_MSG_OBJ_TYPE_TX, 0,
                           CAN_MSG_OBJ_NO_FLAGS, MSG_DATA_LENGTH);

    CAN_startModule(CANA_BASE);
}

void MSG_write(uint16_t byte)
{
    CAN_sendMessage(CANA_BASE, TX_MSG_OBJ_ID, MSG_DATA_LENGTH, byte);

            //
            // Poll TxOk bit in CAN_ES register to check completion of transmission
            //
            while(((HWREGH(CANA_BASE + CAN_O_ES) & CAN_ES_TXOK)) !=  CAN_ES_TXOK)
            {
            }
}


