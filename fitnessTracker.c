/**********************************************************
 *
 * Milestone1.c
 **********************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_i2c.h"
#include "driverlib/pin_map.h" //Needed for pin configure
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/i2c.h"
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "utils/ustdlib.h"
#include "acc.h"
#include "i2c_driver.h"
#include "buttons4.h"
#include "driverlib/interrupt.h"
#include "circBufT.h"
#include "updateDisplay.h"
#include "acceleration.h"

/**********************************************************
 * Constants
 **********************************************************/
// Systick configuration
#define SYSTICK_RATE_HZ 10
#define SAMPLE_RATE_HZ 10

//*****************************************************************************
// Global variables
//*****************************************************************************
static int16_t stepCount = -1;
static int32_t distance = 0;

/*******************************************
 *      Local prototypes
 *******************************************/
void initClock (void);
void initDisplay (void);
void initAccl (void);

/***********************************************************
 * Initialisation functions: clock, SysTick, PWM
 ***********************************************************
 * Clock
 ***********************************************************/
void
initClock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                   SYSCTL_XTAL_16MHZ);

    //
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);
    //
    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);
    //
    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}

// *******************************************************
// countSteps: calculcates the norm of vector x, y, z and checks
// whether a step has occured.
// A step occurs when norm changes from norm > 150 (sets stepFlag = 1) to norm < 150 (sets stepFlag = 0)
uint16_t countSteps(uint16_t stepFlag, vector3_t acceleration_gs) {
    uint16_t norm = sqrt(pow(acceleration_gs.x, 2) + pow(acceleration_gs.y, 2) + pow(acceleration_gs.z, 2));
    if (stepFlag == 0) {
        //If it is above 1.5 then set to 1 and add 1 step
        if (norm >= 150) {
            stepCount += 1;
            stepFlag = 1;
        }
    } else {
        //If it is now below 1.5 then set flag to 0
        if (norm < 150) {
            stepFlag = 0;
        }
    }
    distance = stepCount * 90;

    return stepFlag;
}

// *******************************************************
// resetSteps: resets StepCount
void resetSteps(void){
    stepCount = 0;
}

// *******************************************************
// incrementSteps: increments stepCount by 100
void incrementSteps(void) {
    stepCount += 100;
}

// *******************************************************
// decrementSteps: decrements stepCount by 100
void decrementSteps(void) {
    stepCount = ((stepCount - 500) < 0) ? 0 : (stepCount - 500);
}

// *******************************************************
// checkButtons: Checks whether each button has been pressed or released,
// if the left or right buttton has been pressed, a call to leftOrRightButtonPressed()
// is made. Similarly for the up and down buttons,
// a function call to upButtonPressed() and downButtonLongPressed() is made respectively.
// The function also checks whether the switch has been pushed, making a call to
// switchSwitched(true) setting the switchState to true 
// and switchSwitched(false) setting the switchState to false when the switched is released.
// If the down button is released a call to resetLongPresses() is made.
void checkButtons(void)
{
    uint8_t butState;
    uint8_t switchState;
    butState = checkButton (LEFT);
    switch (butState)
    {
    case PUSHED:
        leftOrRightButtonPressed();
        break;
    case RELEASED:
        break;
    // Do nothing if state is NO_CHANGE
    }

    butState = checkButton (RIGHT);
    switch (butState)
    {
    case PUSHED:
        leftOrRightButtonPressed();
        break;
    case RELEASED:
        break;
    // Do nothing if state is NO_CHANGE
    }

    butState = checkButton (UP);
    switch (butState)
    {
    case PUSHED:
        upButtonPressed();
        break;
    case RELEASED:
        break;
    // Do nothing if state is NO_CHANGE
    }

    butState = checkButton (DOWN);
    switch (butState)
    {
    case PUSHED:
        downButtonLongPressed();
        break;
    case RELEASED:
        resetLongPresses();
        break;
    default:
        incrementLongPresses();
    }

    switchState = checkSwitch();
    switch (switchState)
    {
    case PUSHED:
        switchSwitched(true);
        break;
    case RELEASED:
        switchSwitched(false);
        break;
    }
}

/********************************************************
 * Main function which initilises the the microcontroller
 * and runs the round robin task schedular (while loop)
 ********************************************************/
int
main (void)
{
    initClock ();
    initButtons ();

    // Enable interrupts to the processor.
    IntMasterEnable();

    vector3_t acceleration_gs;
    int16_t stepFlag = 0;
    vector3_t offSet = getAcclData();
    while (1)
    {
        SysCtlDelay (SysCtlClockGet () / 100);  // set delay
        acceleration_gs = calculateAcceleration(offSet);
        stepFlag = countSteps(stepFlag, acceleration_gs);
        // check state of each button and display if a change is detected
        updateButtons ();

        checkButtons();

        displayUpdate(stepCount, distance);  // 
    }
}
