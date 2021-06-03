
//
// Included Files
//
#include <Comm_CAN.h>
#include "Time_mng.h"



#define CPU_RATE    5.00L
#define DELAY (CPU_RATE/1000*6*510)  //Qual period at 6 samples

uint16_t risingEdgeCount = 0;
uint16_t fallingEdgeCount = 0;
uint16_t pulseCount = 0;
//
// Interrupt Handler
//
interrupt void mStateRisingEdge(void);
interrupt void mStateFallingEdge(void);
__interrupt void cpuTimer0ISR(void);
__interrupt void cpuTimer1ISR(void);



//
// Globals
uint16_t baseTime;
uint16_t pulseTime;
bool pulseError=0;

//
// Function Prototypes
//
void masterGeneralSetup(void);
void mStateSendCommand(void);




//
// Main
//
void main(void)
{
    masterGeneralSetup();

    for(;;)
    {

        baseTime = getTime(baseTimer);

        if(baseTime == 1000){
            timeReset(baseTimer);
            timeStop(baseTimer);
            if(pulseCount > 0 || pulseError == 1){

                mStateSendCommand();
            }


        }
    }
}

void masterGeneralSetup (void){
    Device_init(); // Initializes system control, device clock, and peripherals

    Interrupt_initModule();     // Initializes PIE and clear PIE registers. Disables CPU interrupts.
                                // and clear all CPU interrupt flags.

    Interrupt_initVectorTable();     // Initialize the PIE vector table with pointers to the shell interrupt
                                    // Service Routines (ISR).

    Board_init();     // Board Initialization


    Interrupt_register(INT_XINT1, &mStateRisingEdge);// Interrupts that are used in this example are re-mapped to
    Interrupt_register(INT_XINT2, &mStateFallingEdge); // ISR functions found within this file.


    Interrupt_enable(INT_XINT1);    // Enable XINT1 and XINT2 in the PIE: Group 1 interrupt 4 & 5
    Interrupt_enable(INT_XINT2);// Enable INT1 which is connected to WAKEINT:


      GPIO_setDirectionMode(11,GPIO_DIR_MODE_IN);          // input
      // XINT1 Qual using 6 samples
      GPIO_setQualificationMode(11, GPIO_QUAL_6SAMPLE);
      GPIO_setDirectionMode(10,GPIO_DIR_MODE_IN);          // input
      // XINT2 Qual using 6 samples
      GPIO_setQualificationMode(10, GPIO_QUAL_6SAMPLE);
      // Set qualification period for GPIO0 to GPIO7
      // Each sampling window is 510*SYSCLKOUT
      GPIO_setQualificationPeriod(3,510);

      GPIO_setInterruptPin(11,GPIO_INT_XINT1);// GPIO0 is XINT1, GPIO1 is XINT2
      GPIO_setInterruptPin(10,GPIO_INT_XINT2);

      GPIO_setInterruptType(GPIO_INT_XINT1, GPIO_INT_TYPE_FALLING_EDGE);  // Falling edge interrupt
      GPIO_setInterruptType(GPIO_INT_XINT2, GPIO_INT_TYPE_RISING_EDGE); // Rising edge interrupt

      GPIO_enableInterrupt(GPIO_INT_XINT1);         // Enable XINT1
      GPIO_enableInterrupt(GPIO_INT_XINT2);         // Enable XINT2



      Interrupt_register(INT_TIMER0, &cpuTimer0ISR); // ISRs for each CPU Timer interrupt
      Interrupt_register(INT_TIMER1, &cpuTimer1ISR);

      initCPUTimers();
      Device_initGPIO();
      MSG_CAN();

}



//
// mStateRisingEdge - External Interrupt 1 ISR
//
interrupt void mStateRisingEdge(void)
{
    timeStart(pulseTimer);
    timeStart(baseTimer);
    risingEdgeCount++;
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);

}

//
// mStateFallingEdge - External Interrupt 2 ISR
//
interrupt void mStateFallingEdge(void)
{

    timeStop(pulseTimer);
    fallingEdgeCount++;
    pulseTime = getTime(pulseTimer);
    timeReset(pulseTimer);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);

    if(pulseTime>20 && pulseTime<1000)
        pulseCount++;
    else if (pulseTime<1000)
        pulseError = 1;


}


void mStateSendCommand(void)
{
    char command;
    if (pulseCount == 1)
        command = 'A';
    else if (pulseCount == 2)
        command = 'B';
    else if (pulseCount == 3)
        command = 'C';
    else
        command = 'E';

    if(pulseError == 1)
        command = 'E';

    MSG_write(command);
    command=0;

}


//
// End of file
//
