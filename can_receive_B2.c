//#############################################################################
//
// FILE:   can_receive.c
//
// TITLE:   CAN RECEIVE BOARD 2
//
/* ========================== include files =========================== */
#include "driverlib.h"
#include "device.h"

/* ============================ constants ============================= */
#define LOOP_COUNT 10
#define MSG_DATA_LENGTH 0 // "Don't care" for a Receive mailbox
#define RX_MSG_OBJ_ID 1   // Use mailbox 1

/* ======================== global variables ========================== */
uint16_t rxMsgData[8];
uint16_t Data;
volatile uint32_t rxMsgCount = 0;
uint16_t Dictionary[4];
uint16_t cValue;
uint16_t x;

/* ==================== function prototypes =========================== */
void slaveGeneralSetup(void);
void can(void);
int CheckDictionary(int Dictionary[], int Data);
int setPWM(int Data);
void defPWM(void);

//
// Main
//
void main(void)
{
    slaveGeneralSetup();

    Dictionary[0] = 0xA;
    Dictionary[1] = 0xB;
    Dictionary[2] = 0xC;
    Dictionary[3] = 0xE;

    //
    // Start reception - Just wait for data from another node
    //
    while (1)
    {
        //
        // Poll RxOk bit in CAN_ES register to check completion of Reception
        //
        if (((HWREGH(CANA_BASE + CAN_O_ES) & CAN_ES_RXOK)) == CAN_ES_RXOK) //CAN_O_ES (4) // Error and Status Register //CAN_ES_RXOK (16) // Reception status
        {
            //
            // Get the received message
            //
            //CAN_IF1DATA Register    0x00000000

            CAN_readMessage(CANA_BASE, RX_MSG_OBJ_ID, rxMsgData);
            rxMsgCount++;
        }

        Data = rxMsgData[0];
        cValue = CheckDictionary(Dictionary, Data);

        if (rxMsgData[0 != ])
        {
            /* code */
        }

        if (cValue != -1)
        {
            setPWM(cValue);
        }
    }
}

/* ============================ Functions ============================= */
void slaveGeneralSetup()
{

    // Initialize device clock and peripherals
    Device_init();

    //
    // Initialize GPIO
    //
    Device_initGPIO();
    GPIO_setPadConfig(2, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(2, GPIO_DIR_MODE_OUT);
    //
    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    //
    Interrupt_initModule();

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();

    //
    // Configure GPIO pins for CANTX/CANRX
    //
    GPIO_setPinConfig(GPIO_70_CANRXA);
    GPIO_setPinConfig(GPIO_71_CANTXA);

    //
    // Configure GPIO pin which is toggled upon message reception
    //
    GPIO_setPadConfig(70, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(71, GPIO_DIR_MODE_OUT);
}

void can()
{
    //
    // Initialize the CAN controller
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
    // Initialize the receive message object used for receiving CAN messages.
    // Message Object Parameters:
    //      CAN Module: A
    //      Message Object ID Number: 1
    //      Message Identifier: 0x1
    //      Message Frame: Standard
    //      Message Type: Receive
    //      Message ID Mask: 0x0
    //      Message Object Flags: None
    //      Message Data Length: "Don't care" for a Receive mailbox
    //
    CAN_setupMessageObject(CANA_BASE, RX_MSG_OBJ_ID, 0x1,
                           CAN_MSG_FRAME_STD, CAN_MSG_OBJ_TYPE_RX, 0,
                           CAN_MSG_OBJ_NO_FLAGS, MSG_DATA_LENGTH);

    //
    // Start CAN module A operations
    //
    CAN_startModule(CANA_BASE);
}

int CheckDictionary(int dictionary[], int Data)
{
    for (x = 0; x < 4; x++)
    {
        if (dictionary[x] == Data)
            return Data;
    }
    return -1;
}

void defPWM()
{
    for (;;)
    {
        //
        // Turn on LED
        //
        GPIO_writePin(2, 0);

        //
        // Delay for a bit.
        //
        DEVICE_DELAY_US(250000);

        //
        // Turn off LED
        //
        GPIO_writePin(2, 1);

        //
        // Delay for a bit.
        //
        DEVICE_DELAY_US(250000);
    }
}

int setPWM(int Data)
{
    switch (Data)
    {
        // 0x0A received, toggle led
    case 10:
        GPIO_writePin(2, 0);
        break;

        // 0x0B received, PWM 1HZ
    case 11:
        for (;;)
        {
            //
            // Turn on LED
            //
            GPIO_writePin(2, 0);

            //
            // Delay for a bit.
            //
            DEVICE_DELAY_US(500000);

            //
            // Turn off LED
            //
            GPIO_writePin(2, 1);

            //
            // Delay for a bit.
            //
            DEVICE_DELAY_US(500000);
        }
        break;

        // 0x0C received, PWM 1HZ
    case 12:
        for (;;)
        {
            //
            // Turn on LED
            //
            GPIO_writePin(2, 0);

            //
            // Delay for a bit.
            //
            DEVICE_DELAY_US(1000000);

            //
            // Turn off LED
            //
            GPIO_writePin(2, 1);

            //
            // Delay for a bit.
            //
            DEVICE_DELAY_US(1000000);
        }
        break;
    }

case 14:
    GPIO_writePin(3, 1);
    for (;;)
    {
        //
        // Turn on LED
        //
        GPIO_writePin(2, 0);

        //
        // Delay for a bit.
        //
        DEVICE_DELAY_US(250000);

        //
        // Turn off LED
        //
        GPIO_writePin(2, 1);

        //
        // Delay for a bit.
        //
        DEVICE_DELAY_US(250000);
    }
    break;
}
}
