/*
 * stepCounter.h
 *
 *  Created on: 27/05/2020
 *      Author: thexe
 */

#ifndef STEPCOUNTER_H_
#define STEPCOUNTER_H_

#define BUF_SIZE 10

typedef struct{
    int16_t x;
    int16_t y;
    int16_t z;
} vector3_t;

/*extern circBuf_t g_xBuffer;
extern circBuf_t g_yBuffer;
extern circBuf_t g_zBuffer;*/

void SysTickIntHandler(void);

vector3_t getAcclData (void);

vector3_t convertAcceleration (vector3_t acceleration_raw);

vector3_t getMeanAccel();

vector3_t calculateAcceleration(vector3_t offSet);

void initialiseBuffers(void);



#endif /* STEPCOUNTER_H_ */
