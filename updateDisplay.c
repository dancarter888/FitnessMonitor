/*
 * updateDisplay.c
 *
 *  Created on: 12/05/2020
 *      Author: Daniel
 */
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

#define STEP_STATE 0
#define DISTANCE_STATE 1
#define NORMAL_STATE 0
#define DEBUG_STATE 1
#define KILOMETRES 0
#define MILES 1

static int displayState = STEP_STATE;
int unitState = KILOMETRES;
int systemState = NORMAL_STATE;


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


//void downButtonPressed(void) {}

void downButtonLongPressed(void) {
    if (systemState == NORMAL_STATE) {
        resetSteps();
    } else if (systemState == DEBUG_STATE) {
        decrementSteps();
    }
}


void leftOrRightButtonPressed(void) {
    if (displayState == STEP_STATE) {
        displayState = DISTANCE_STATE;
    } else {
        displayState = STEP_STATE;
    }
}

uint16_t convertDistance(uint16_t distance) {
    if (unitState == MILES) {
        distance = distance * 0.621;
    }

    return distance;
}

void displayUpdateNEW (int16_t stepCount, uint16_t distance)
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
        uint16_t convertedDistance = convertDistance(distance);
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
        usnprintf(text_buffer, sizeof(text_buffer), "%d", convertedDistance);
        // Update line on display.
        OLEDStringDraw (text_buffer, 0, 2);
    }

}



//void switchSwitched(void) {}
