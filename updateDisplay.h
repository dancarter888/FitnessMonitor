#ifndef UPDATEDISPLAY_H_
#define UPDATEDISPLAY_H_

/*
 * updateDisplay.h
 *
 *  Created on: 12/05/2020
 *      Author: Daniel
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#define STEP_STATE 0
#define DISTANCE_STATE 1
#define NORMAL_STATE 0
#define DEBUG_STATE 1
#define KILOMETRES 0
#define MILES 1

void upButtonPressed(void);

void downButtonLongPressed(void);

void leftOrRightButtonPressed(void);

void incrementLongPresses(void);

void resetLongPresses(void);

void displayUpdateNEW (int16_t stepCount, int32_t distance);

#endif /* UPDATEDISPLAY_H_ */
