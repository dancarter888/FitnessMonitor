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

/*********************************************************
 * initDisplay: Initialise the Orbit OLED display
 *********************************************************/
void initDisplay (void);

/**
 * This will be called when the up button is pressed.
 * If the systemState is in the normal state the function will check
 * whether the display state is in the distance state, and will toggle
 * the unistate. If the systemState is in the debug state the steps will be
 * incremented.
 */
void upButtonPressed(void);

/**
 * This will be called when the down button is pressed.
 * If it is normal state it will start the process of checking
 * for a long push by incrementing longPresses to be > 0.
 * If in debug then decrement steps.
 */
void downButtonLongPressed(void);

/**
 * Checks whether the displayState is currently step or distance state,
 * and changes it to the inverse of its current state. This function is called
 * when the left or right button has been pressed.
 */
void leftOrRightButtonPressed(void);

/**
 * If there is no change in the button state and the
 * long press down has been initialized then increment
 * increment longPresses. This will be incremented every
 * loop. If it is greater than the threshold count it as a
 * long push and reset the sets.
 */
void incrementLongPresses(void);

/**
 * This will be called if the down button is released
 */
void resetLongPresses(void);

/**
 * Displays the number of steps is the displayState == STEP_STATE,
 * and displays the distance if displaystate == DISTANCE_STATE, 
 * on the orbit OLED display.
 * Displays distance in terms of kms and miles depending on the unitState.
 */
void displayUpdate (int16_t stepCount, int32_t distance);

/**
 * Checks whether the switch_state is in the normal or debug state
 * and sets the system state to normal and debug mode respectively.
 * This function is called when the switch has been pushed (switched up).
 */
void switchSwitched(bool switch_state);

#endif /* UPDATEDISPLAY_H_ */
