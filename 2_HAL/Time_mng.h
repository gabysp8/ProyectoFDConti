
//
// Included Files
//
#include "driverlib.h"
#include "device.h"
#include "board.h"

//
// Globals
//
uint16_t cpuTimer0IntCount;
uint16_t cpuTimer1IntCount;

#define pulseTimer CPUTIMER0_BASE
#define baseTimer CPUTIMER1_BASE


//
// Function Prototypes
//
void initCPUTimers(void);
void configCPUTimer(uint32_t, float, float);


// initCPUTimers - This function initializes all three CPU timers
// to a known state.
//
void initCPUTimers(void)
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

    Interrupt_enableMaster();

    //COnfigure Timer 0 & 1 to count each milisecond
    configCPUTimer(CPUTIMER0_BASE, DEVICE_SYSCLK_FREQ, 1000);
    configCPUTimer(CPUTIMER1_BASE, DEVICE_SYSCLK_FREQ, 1000);

    CPUTimer_enableInterrupt(CPUTIMER0_BASE);
    CPUTimer_enableInterrupt(CPUTIMER1_BASE);
    //
    // Enables CPU int1, int13, and int14 which are connected to CPU-Timer 0,
    // CPU-Timer 1, and CPU-Timer 2 respectively.
    // Enable TINT0 in the PIE: Group 1 interrupt 7
    //
    Interrupt_enable(INT_TIMER0);
    Interrupt_enable(INT_TIMER1);

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
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

//
// cpuTimer1ISR - Counter for CpuTimer1
//
__interrupt void
cpuTimer1ISR(void)
{

    cpuTimer1IntCount++;
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}


void timeStart(uint32_t cpuTimer)
{
    CPUTimer_startTimer(cpuTimer);
}

void timeStop(uint16_t cpuTimer)
{
    CPUTimer_stopTimer(cpuTimer);

}


void timeReset(uint32_t cpuTimer)
{
    if (cpuTimer == CPUTIMER0_BASE)
        {

            cpuTimer0IntCount=0;

        }
        else if(cpuTimer == CPUTIMER1_BASE)
        {
            cpuTimer1IntCount=0;
        }
}


uint16_t getTime(uint32_t cpuTimer)
{
    ASSERT(CPUTimer_isBaseValid(cpuTimer));
    if (cpuTimer == CPUTIMER0_BASE)
        {

            return(cpuTimer0IntCount);

        }
        else if(cpuTimer == CPUTIMER1_BASE)
        {
            return(cpuTimer1IntCount);
        }
        else
            return (0);
}




