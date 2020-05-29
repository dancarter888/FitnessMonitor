#ifndef ACCELERATION_H_
#define ACCELERATION_H_

// *******************************************************
// acceleration.h
//
// Header file for a module for initializing and getting data
// from the accelerometer on the Tiva board and storing it in
// circular buffers
//
//
// Authors: Jakob McKinney, Daniel Siemers, Leo Carolan
// Last modified:  29.05.2020
//
// *******************************************************

#define BUF_SIZE 10

typedef struct{
    int16_t x;
    int16_t y;
    int16_t z;
} vector3_t;

//*****************************************************************************
// The interrupt handler for the for SysTick interrupt.
// Makes a call to getAcclData() to get the acceleration data at the time of the
// sysTick interrupt. It then writes the acceleration data to the circular 
// buffer.
//*****************************************************************************
void SysTickIntHandler(void);

/*********************************************************
 * initialises the acceleration
 *********************************************************/
void initAccl (void);

/********************************************************
 * Function to read acceleration from the accelerometer
 * and return the acceleration
 ********************************************************/
vector3_t getAcclData (void);

/********************************************************
 * Function to convert and return the accelartion from raw to G's
 ********************************************************/
vector3_t convertAcceleration (vector3_t acceleration_raw);

/********************************************************
 * Function to calulate and return the mean acceleration value 
 * meanVec
 ********************************************************/
vector3_t getMeanAccel();

/********************************************************
 * Function to calculate the acceleration - the offset acceleration
 * based of the mean acceleration. The acceleration is then converted
 * to G's and returned.
 ********************************************************/
vector3_t calculateAcceleration(vector3_t offSet);

/********************************************************
 * Initialises the circular buffers with the given buffer
 * size: BUF_SIZE
 ********************************************************/
void initialiseBuffers(void);



#endif /* ACCELERATION_H_ */
