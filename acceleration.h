/*
 * acceleration.h
 *
 *  Created on: 27/05/2020
 *      Author: Jakob, Daniel, Leo
 */

#ifndef ACCELERATION_H_
#define ACCELERATION_H_

#define BUF_SIZE 10

typedef struct{
    int16_t x;
    int16_t y;
    int16_t z;
} vector3_t;

void SysTickIntHandler(void);

void initAccl (void);

vector3_t getAcclData (void);

vector3_t convertAcceleration (vector3_t acceleration_raw);

vector3_t getMeanAccel();

vector3_t calculateAcceleration(vector3_t offSet);

void initialiseBuffers(void);



#endif /* ACCELERATION_H_ */
