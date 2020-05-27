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
#include "stepCounter.h"

/*typedef struct{
    int16_t x;
    int16_t y;
    int16_t z;
} vector3_t;*/

/**********************************************************
 * Constants
 **********************************************************/
// Systick configuration
#define SYSTICK_RATE_HZ 10
//#define BUF_SIZE 10
#define SAMPLE_RATE_HZ 10

// Unit conversion values
#define ACCELERATION_RAW 0
#define ACCELERATION_GS 1
#define ACCELERATION_MS2 2

//*****************************************************************************
// Global variables
//*****************************************************************************
static circBuf_t g_xBuffer;
static circBuf_t g_yBuffer;
static circBuf_t g_zBuffer;     // Buffer of size BUF_SIZE integers (sample values)
static int16_t stepCount = -1;
static int32_t distance = 0;

/*******************************************
 *      Local prototypes
 *******************************************/
void initClock (void);
void initDisplay (void);
void initAccl (void);
//vector3_t getAcclData (void);

//*****************************************************************************
// The interrupt handler for the for SysTick interrupt.
//*****************************************************************************
/*void
SysTickIntHandler(void)
{
    vector3_t currentVec = getAcclData();
    writeCircBuf(&g_xBuffer, currentVec.x);
    writeCircBuf(&g_yBuffer, currentVec.y);
    writeCircBuf(&g_zBuffer, currentVec.z);
}*/


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

/*********************************************************
 * initDisplay
 *********************************************************/
void
initDisplay (void)
{
    // Initialise the Orbit OLED display
    OLEDInitialise ();
}

/*********************************************************
 * initAccl
 *********************************************************/
void
initAccl (void)
{
    char    toAccl[] = {0, 0};  // parameter, value

    /*
     * Enable I2C Peripheral
     */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_I2C0);

    /*
     * Set I2C GPIO pins
     */
    GPIOPinTypeI2C(I2CSDAPort, I2CSDA_PIN);
    GPIOPinTypeI2CSCL(I2CSCLPort, I2CSCL_PIN);
    GPIOPinConfigure(I2CSCL);
    GPIOPinConfigure(I2CSDA);

    /*
     * Setup I2C
     */
    I2CMasterInitExpClk(I2C0_BASE, SysCtlClockGet(), true);

    GPIOPinTypeGPIOInput(ACCL_INT2Port, ACCL_INT2);

    //Initialize ADXL345 Acceleromter

    // set +-16g, 13 bit resolution, active low interrupts
    toAccl[0] = ACCL_DATA_FORMAT;
    toAccl[1] = (ACCL_RANGE_16G | ACCL_FULL_RES);
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_PWR_CTL;
    toAccl[1] = ACCL_MEASURE;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);


    toAccl[0] = ACCL_BW_RATE;
    toAccl[1] = ACCL_RATE_100HZ;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_INT;
    toAccl[1] = 0x00;       // Disable interrupts from accelerometer.
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_OFFSET_X;
    toAccl[1] = 0x00;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_OFFSET_Y;
    toAccl[1] = 0x00;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);

    toAccl[0] = ACCL_OFFSET_Z;
    toAccl[1] = 0x00;
    I2CGenTransmit(toAccl, 1, WRITE, ACCL_ADDR);
}

/********************************************************
 * Function to read accelerometer
 ********************************************************/
/*vector3_t
getAcclData (void)
{
    char    fromAccl[] = {0, 0, 0, 0, 0, 0, 0}; // starting address, placeholders for data to be read.
    vector3_t acceleration;
    uint8_t bytesToRead = 6;

    fromAccl[0] = ACCL_DATA_X0;
    I2CGenTransmit(fromAccl, bytesToRead, READ, ACCL_ADDR);

    acceleration.x = (fromAccl[2] << 8) | fromAccl[1]; // Return 16-bit acceleration readings.
    acceleration.y = (fromAccl[4] << 8) | fromAccl[3];
    acceleration.z = (fromAccl[6] << 8) | fromAccl[5];

    return acceleration;
}

vector3_t
convertAcceleration (vector3_t acceleration_raw)
{
    OLEDStringDraw ("GS ", 0, 0);
    acceleration_raw.x = (acceleration_raw.x * 1000) / 256;
    acceleration_raw.y = (acceleration_raw.y * 1000) / 256;
    acceleration_raw.z = (acceleration_raw.z * 1000) / 256;

    return acceleration_raw;
}

vector3_t getMeanAccel() {
    uint16_t i;
    int32_t x_sum;
    int32_t y_sum;
    int32_t z_sum;
    vector3_t meanVec;

    x_sum = 0;
    y_sum = 0;
    z_sum = 0;
    for (i = 0; i < BUF_SIZE; i++) {
        x_sum = x_sum + readCircBuf(&g_xBuffer);
        y_sum = y_sum + readCircBuf(&g_yBuffer);
        z_sum = z_sum + readCircBuf(&g_zBuffer);
    }

    meanVec.x = (2 * x_sum + BUF_SIZE) / 2 / BUF_SIZE;
    meanVec.y = (2 * y_sum + BUF_SIZE) / 2 / BUF_SIZE;
    meanVec.z = (2 * z_sum + BUF_SIZE) / 2 / BUF_SIZE;

    return meanVec;
}*/

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

void resetSteps(void){
    stepCount = 0;
}

void incrementSteps(void) {
    stepCount += 100;
}

void decrementSteps(void) {
    stepCount = ((stepCount - 500) < 0) ? 0 : (stepCount - 500);
}

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

/*vector3_t calculateAcceleration(vector3_t offSet)
{
    vector3_t acceleration_raw;
    vector3_t acceleration_gs;
    acceleration_raw = getMeanAccel();
    acceleration_raw.x -= offSet.x;
    acceleration_raw.y -= offSet.y;
    acceleration_raw.z -= offSet.z;

    acceleration_gs = convertAcceleration(acceleration_raw);
    return acceleration_gs;
}*/

/********************************************************
 * main
 ********************************************************/
int
main (void)
{
    initClock ();
    initAccl ();
    initDisplay ();
    initButtons ();
    initCircBuf (&g_xBuffer, BUF_SIZE);
    initCircBuf (&g_yBuffer, BUF_SIZE);
    initCircBuf (&g_zBuffer, BUF_SIZE);

    // Enable interrupts to the processor.
    IntMasterEnable();

    vector3_t acceleration_gs;
    int16_t stepFlag = 0;
    vector3_t offSet = getAcclData();
    while (1)
    {
        SysCtlDelay (SysCtlClockGet () / 100);
        acceleration_gs = calculateAcceleration(offSet);
        stepFlag = countSteps(stepFlag, acceleration_gs);
        // check state of each button and display if a change is detected
        updateButtons ();

        checkButtons();

        displayUpdate(stepCount, distance);
    }
}
