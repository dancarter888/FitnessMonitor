/*
 * stepCounter.c
 *
 *  Created on: 27/05/2020
 *      Author: jakob
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

#define BUF_SIZE 10

typedef struct{
    int16_t x;
    int16_t y;
    int16_t z;
} vector3_t;

vector3_t getAcclData (void);

static circBuf_t g_xBuffer;
static circBuf_t g_yBuffer;
static circBuf_t g_zBuffer;     // Buffer of size BUF_SIZE integers (sample values)

void
SysTickIntHandler(void)
{
    vector3_t currentVec = getAcclData();
    writeCircBuf(&g_xBuffer, currentVec.x);
    writeCircBuf(&g_yBuffer, currentVec.y);
    writeCircBuf(&g_zBuffer, currentVec.z);
}

/********************************************************
 * Function to read accelerometer
 ********************************************************/
vector3_t
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
}

vector3_t calculateAcceleration(vector3_t offSet)
{
    vector3_t acceleration_raw;
    vector3_t acceleration_gs;
    acceleration_raw = getMeanAccel();
    acceleration_raw.x -= offSet.x;
    acceleration_raw.y -= offSet.y;
    acceleration_raw.z -= offSet.z;

    acceleration_gs = convertAcceleration(acceleration_raw);
    return acceleration_gs;
}

void
initialiseBuffers(void)
{
    initCircBuf (&g_xBuffer, BUF_SIZE);
    initCircBuf (&g_yBuffer, BUF_SIZE);
    initCircBuf (&g_zBuffer, BUF_SIZE);
}
