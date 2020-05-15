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

void upButtonPressed(void) {
    if (displayState == DISTANCE_STATE) {
        if (unitState == KILOMETRES) {
            unitState = MILES;
        } else {
            unitState = KILOMETRES;
        }
    }
}


//void downButtonPressed(void) {}

void downButtonLongPressed(void) {
    if (displayState == DISTANCE_STATE) {
        resetDistance();
    } else {
        resetSteps();
    }
}


void leftOrRightButtonPressed(void) {
    if (displayState == STEP_STATE) {
        displayState = DISTANCE_STATE;
    } else {
        displayState = STEP_STATE;
    }
}

int16_t convertDistance(int16_t distanceCount) {
    if (unitState == MILES) {
        distanceCount = distanceCount * 0.393;
    }

    return distanceCount;
}

void displayUpdateNEW (int16_t stepCount, int16_t distanceCount)
{
    if (displayState == STEP_STATE) {
        char text_buffer[17];           //Display fits 16 characters wide.

        // "Undraw" the previous contents of the line to be updated.
        OLEDStringDraw ("                ", 0, 0);
        // Form a new string for the line.  The maximum width specified for the
        //  number field ensures it is displayed right justified.
        usnprintf(text_buffer, sizeof(text_buffer), "STEPS: %3d", stepCount);
        // Update line on display.
        OLEDStringDraw (text_buffer, 0, 0);
    } else if (displayState == DISTANCE_STATE) {
        int16_t convertedDistanceCount = convertDistance(distanceCount);
        char text_buffer[17];           //Display fits 16 characters wide.

        // "Undraw" the previous contents of the line to be updated.
        OLEDStringDraw ("                ", 0, 0);
        // Form a new string for the line.  The maximum width specified for the
        //  number field ensures it is displayed right justified.
        usnprintf(text_buffer, sizeof(text_buffer), "DISTANCE: %3d", convertedDistanceCount);
        // Update line on display.
        OLEDStringDraw (text_buffer, 0, 0);
    }

}



//void switchSwitched(void) {}
