
//
// Included Files
//
#include "driverlib.h"
#include "device.h"
#include "board.h"


#define CPU_RATE    5.00L
#define DELAY (CPU_RATE/1000*6*510)  //Qual period at 6 samples

volatile uint32_t XINT1Count;
volatile uint32_t XINT2Count;
//
// Interrupt Handler
//
interrupt void xint1_isr(void);
interrupt void xint2_isr(void);

//
// Globals
//
uint16_t cpuTimer0IntCount;
uint16_t cpuTimer1IntCount;

//
// Function Prototypes
//
__interrupt void cpuTimer0ISR(void);
__interrupt void cpuTimer1ISR(void);
void initCPUTimers(void);
void configCPUTimer(uint32_t, float, float);

int re=0, fe=0;
unsigned long time=0, pulse_time=0;

//
// Main
//
void main(void)
{
    //
    // Initializes system control, device clock, and peripherals
    //
    Device_init();

    //
    // Initializes PIE and clear PIE registers. Disables CPU interrupts.
    // and clear all CPU interrupt flags.
    //
    Interrupt_initModule();

    //
    // Initialize the PIE vector table with pointers to the shell interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();

    //
    // Board Initialization
    //
    Board_init();

    // Interrupts that are used in this example are re-mapped to
    // ISR functions found within this file.
    //
    Interrupt_register(INT_XINT1, &xint1_isr);
    Interrupt_register(INT_XINT2, &xint2_isr);
    XINT1Count = 0; // Count XINT1 interrupts
    XINT2Count = 0; // Count XINT2 interrupts
    // Enable XINT1 and XINT2 in the PIE: Group 1 interrupt 4 & 5
    // Enable INT1 which is connected to WAKEINT:
    //
    Interrupt_enable(INT_XINT1);
    Interrupt_enable(INT_XINT2);
    EINT;

    //
      // GPIO0 and GPIO1 are inputs
      //
      GPIO_setDirectionMode(11,GPIO_DIR_MODE_IN);          // input
      // XINT1 Synch to SYSCLKOUT only
      GPIO_setQualificationMode(11, GPIO_QUAL_6SAMPLE);
      GPIO_setDirectionMode(10,GPIO_DIR_MODE_IN);          // input
      // XINT2 Qual using 6 samples
      GPIO_setQualificationMode(10, GPIO_QUAL_6SAMPLE);
      // Set qualification period for GPIO0 to GPIO7
      // Each sampling window is 510*SYSCLKOUT
      GPIO_setQualificationPeriod(3,510);

      //
      // GPIO0 is XINT1, GPIO1 is XINT2
      //
      GPIO_setInterruptPin(11,GPIO_INT_XINT1);
      GPIO_setInterruptPin(10,GPIO_INT_XINT2);

      //
      // Configure XINT1
      //
      // Falling edge interrupt
      GPIO_setInterruptType(GPIO_INT_XINT1, GPIO_INT_TYPE_FALLING_EDGE);
      // Rising edge interrupt
      GPIO_setInterruptType(GPIO_INT_XINT2, GPIO_INT_TYPE_RISING_EDGE);

      //
      // Enable XINT1 and XINT2
      //
      GPIO_enableInterrupt(GPIO_INT_XINT1);         // Enable XINT1
      GPIO_enableInterrupt(GPIO_INT_XINT2);         // Enable XINT2


       // ISRs for each CPU Timer interrupt
       //
       Interrupt_register(INT_TIMER0, &cpuTimer0ISR);
       Interrupt_register(INT_TIMER1, &cpuTimer1ISR);

       //
       // Initializes the Device Peripheral. For this example, only initialize the
       // Cpu Timers.
       //
       initCPUTimers();


    //
    // Enables CPU interrupts
    //
    Interrupt_enableMaster();
    //
    // Configure CPU-Timer 0, 1, and 2 to interrupt every second:
    // 1 second Period (in uSeconds)
    //
    configCPUTimer(CPUTIMER0_BASE, DEVICE_SYSCLK_FREQ, 1000);
    configCPUTimer(CPUTIMER1_BASE, DEVICE_SYSCLK_FREQ, 1000);

    //
    // To ensure precise timing, use write-only instructions to write to the
    // entire register. Therefore, if any of the configuration bits are changed
    // in configCPUTimer and initCPUTimers, the below settings must also
    // be updated.
    //
    CPUTimer_enableInterrupt(CPUTIMER0_BASE);
    CPUTimer_enableInterrupt(CPUTIMER1_BASE);
    //
    // Enables CPU int1, int13, and int14 which are connected to CPU-Timer 0,
    // CPU-Timer 1, and CPU-Timer 2 respectively.
    // Enable TINT0 in the PIE: Group 1 interrupt 7
    //
    Interrupt_enable(INT_TIMER0);
    Interrupt_enable(INT_TIMER1);

    //
    // Starts CPU-Timer 0, CPU-Timer 1, and CPU-Timer 2.
    //

    //
    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    //
    EINT;
    ERTM;


    //
    // Loop.
    //
    for(;;)
    {
        if(cpuTimer1IntCount == 1000){
            cpuTimer1IntCount = 0;
            CPUTimer_stopTimer(CPUTIMER1_BASE);
        }
    }
}

//
// xint1_isr - External Interrupt 1 ISR
//
interrupt void xint1_isr(void)
{
    CPUTimer_startTimer(CPUTIMER0_BASE);
    CPUTimer_startTimer(CPUTIMER1_BASE);
    XINT1Count++;


    //
    // Acknowledge this interrupt to get more from group 1
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);

}

//
// xint2_isr - External Interrupt 2 ISR
//
interrupt void xint2_isr(void)
{
    pulse_time=cpuTimer0IntCount;
    CPUTimer_stopTimer(CPUTIMER0_BASE);
    XINT2Count++;
    cpuTimer0IntCount=0;

    //
    // Acknowledge this interrupt to get more from group 1
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

//
// End of file
//

//
// initCPUTimers - This function initializes all three CPU timers
// to a known state.
//
void
initCPUTimers(void)
{
    //
    // Initialize timer period to maximum
    //
    CPUTimer_setPeriod(CPUTIMER0_BASE, 0xFFFFFFFF);
    CPUTimer_setPeriod(CPUTIMER1_BASE, 0xFFFFFFFF);

    //
    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT)
    //
    CPUTimer_setPreScaler(CPUTIMER0_BASE, 0);
    CPUTimer_setPreScaler(CPUTIMER1_BASE, 0);


    //
    // Make sure timer is stopped
    //
    CPUTimer_stopTimer(CPUTIMER0_BASE);
    CPUTimer_stopTimer(CPUTIMER1_BASE);


    //
    // Reload all counter register with period value
    //
    CPUTimer_reloadTimerCounter(CPUTIMER0_BASE);
    CPUTimer_reloadTimerCounter(CPUTIMER1_BASE);


    //
    // Reset interrupt counter
    //
    cpuTimer0IntCount = 0;
    cpuTimer1IntCount = 0;

}

//
// configCPUTimer - This function initializes the selected timer to the
// period specified by the "freq" and "period" parameters. The "freq" is
// entered as Hz and the period in uSeconds. The timer is held in the stopped
// state after configuration.
//
void
configCPUTimer(uint32_t cpuTimer, float freq, float period)
{
    uint32_t temp;

    //
    // Initialize timer period:
    //
    temp = (uint32_t)(freq / 1000000 * period);
    CPUTimer_setPeriod(cpuTimer, temp);

    //
    // Set pre-scale counter to divide by 1 (SYSCLKOUT):
    //
    CPUTimer_setPreScaler(cpuTimer, 0);

    //
    // Initializes timer control register. The timer is stopped, reloaded,
    // free run disabled, and interrupt enabled.
    // Additionally, the free and soft bits are set
    //
    CPUTimer_stopTimer(cpuTimer);
    CPUTimer_reloadTimerCounter(cpuTimer);
    CPUTimer_setEmulationMode(cpuTimer,
                              CPUTIMER_EMULATIONMODE_RUNFREE);
    CPUTimer_enableInterrupt(cpuTimer);

    //
    // Resets interrupt counters for the three cpuTimers
    //
    if (cpuTimer == CPUTIMER0_BASE)
    {

        CPUTimer_setEmulationMode(cpuTimer,
                                  CPUTIMER_EMULATIONMODE_RUNFREE);
        CPUTimer_enableInterrupt(cpuTimer);
        cpuTimer0IntCount = 0;

    }
    else if(cpuTimer == CPUTIMER1_BASE)
    {
        cpuTimer1IntCount = 0;
    }
}

//
// cpuTimer0ISR - Counter for CpuTimer0
//
__interrupt void
cpuTimer0ISR(void)
{
    cpuTimer0IntCount++;

    //
    // Acknowledge this interrupt to receive more interrupts from group 1
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

//
// cpuTimer1ISR - Counter for CpuTimer1
//
__interrupt void
cpuTimer1ISR(void)
{
    //
    // The CPU acknowledges the interrupt.
    //
    cpuTimer1IntCount++;
    //CPUTimer_stopTimer(CPUTIMER1_BASE);

}

//


//
// End of File
//



//
// End of File
//

