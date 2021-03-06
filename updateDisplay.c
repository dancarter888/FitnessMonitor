// *******************************************************
//
// updateDisplay.c
//
// A module that handles all the logic for the states that
// a fitness monitor can be in, and also updating the
// display according to that state.
//
// Authors: Jakob McKinney, Daniel Siemers, Leo Carolan
// Last modified:  29.05.2020
//
// *******************************************************
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_i2c.h"
#include "driverlib/pin_map.h"
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

#define STEP_STATE 0
#define DISTANCE_STATE 1
#define NORMAL_STATE 0
#define DEBUG_STATE 1
#define KILOMETRES 0
#define MILES 1
#define LONG_PRESS_THRESHOLD 30

static int displayState = STEP_STATE;
int unitState = KILOMETRES;
int systemState = NORMAL_STATE;
int longPresses = 0;


/*********************************************************
 * initDisplay: Initialise the Orbit OLED display
 *********************************************************/
void
initDisplay (void)
{
    OLEDInitialise ();
}

/**
 * This will be called when the up button is pressed.
 * If the systemState is in the normal state the function will check
 * whether the display state is in the distance state, and will toggle
 * the unistate. If the systemState is in the debug state the steps will be
 * incremented.
 */
void upButtonPressed(void) {
    if (systemState == NORMAL_STATE) {
        if (displayState == DISTANCE_STATE) {
            if (unitState == KILOMETRES) {
                unitState = MILES;
            } else {
                unitState = KILOMETRES;
            }
        }
    } else if (systemState == DEBUG_STATE) {
        incrementSteps();
    }
}


/**
 * This will be called when the down button is pressed.
 * If it is normal state it will start the process of checking
 * for a long push by incrementing longPresses to be > 0.
 * If in debug then decrement steps.
 */
void downButtonLongPressed(void) {
    if (systemState == NORMAL_STATE) {
        longPresses++;
    } else if (systemState == DEBUG_STATE) {
        decrementSteps();
    }
}


/**
 * If there is no change in the button state and the
 * long press down has been initialized then increment
 * increment longPresses. This will be incremented every
 * loop. If it is greater than the threshold count it as a
 * long push and reset the sets.
 */
void incrementLongPresses(void) {
    if (systemState == NORMAL_STATE) {
        if (longPresses > 0) {
            longPresses++;
            if (longPresses > LONG_PRESS_THRESHOLD) {
                resetSteps();
            }
        }
    }
}


/**
 * This will be called if the down button is released
 */
void resetLongPresses(void) {
    if (systemState == NORMAL_STATE) {
        longPresses = 0;
    }
}

/**
 * Checks whether the displayState is currently step or distance state,
 * and changes it to the inverse of its current state. This function is called
 * when the left or right button has been pressed.
 */
void leftOrRightButtonPressed(void) {
    if (displayState == STEP_STATE) {
        displayState = DISTANCE_STATE;
    } else {
        displayState = STEP_STATE;
    }
}

/**
 * Converts the distance from kms to miles
 */
uint32_t convertDistance(uint32_t distance) {
    return distance * 0.621;
}


/**
 * Displays the number of steps is the displayState == STEP_STATE,
 * and displays the distance if displaystate == DISTANCE_STATE, 
 * on the orbit OLED display.
 * Displays distance in terms of kms and miles depending on the unitState.
 */
void displayUpdate (int16_t stepCount, uint32_t distance)
{
    if (displayState == STEP_STATE) {
        char text_buffer[17];           //Display fits 16 characters wide.

        // "Undraw" the previous contents of the line to be updated.
        OLEDStringDraw ("                ", 0, 0);
        // Form a new string for the line.  The maximum width specified for the
        //  number field ensures it is displayed right justified.
        usnprintf(text_buffer, sizeof(text_buffer), "STEPS:");
        // Update line on display.
        OLEDStringDraw (text_buffer, 0, 0);

        // "Undraw" the previous contents of the line to be updated.
       OLEDStringDraw ("                ", 0, 2);
       // Form a new string for the line.  The maximum width specified for the
       //  number field ensures it is displayed right justified.
       usnprintf(text_buffer, sizeof(text_buffer), "%d", stepCount);
       // Update line on display.
       OLEDStringDraw (text_buffer, 0, 2);

    } else if (displayState == DISTANCE_STATE) {
        if (unitState == MILES) {
            distance = convertDistance(distance);
        }


        // Calculate each digit to repr the number of kms
        uint32_t distMetres = distance / 100;
        uint16_t thousands = distMetres / 1000;
        uint16_t y = distMetres % 1000;
        uint16_t hundreds = y / 100;
        uint16_t tens = (y % 100) / 10;
        uint16_t ones = y % 10;

        char text_buffer[17];           //Display fits 16 characters wide.
        // "Undraw" the previous contents of the line to be updated.
        OLEDStringDraw ("                ", 0, 0);
        // Form a new string for the line.  The maximum width specified for the
        //  number field ensures it is displayed right justified.
        usnprintf(text_buffer, sizeof(text_buffer), "DISTANCE:");
        // Update line on display.
        OLEDStringDraw (text_buffer, 0, 0);

        // "Undraw" the previous contents of the line to be updated.
        OLEDStringDraw ("                ", 0, 2);
        // Form a new string for the line.  The maximum width specified for the
        //  number field ensures it is displayed right justified.
        char* unitName = "kilometres";

        if (unitState == MILES) {
            unitName = "miles";
        }

        usnprintf(text_buffer, sizeof(text_buffer), "%d.%d%d%d %s", thousands, hundreds, tens, ones, unitName);
        // Update line on display.
        OLEDStringDraw (text_buffer, 0, 2);
    }

}


/**
 * Checks whether the switch_state is in the normal or debug state
 * and sets the system state to normal and debug mode respectively.
 * This function is called when the switch has been pushed (switched up).
 */
void switchSwitched(bool switch_state) {
    if (switch_state == false) {
        systemState = NORMAL_STATE;
    } else {
        systemState = DEBUG_STATE;
    }
}
