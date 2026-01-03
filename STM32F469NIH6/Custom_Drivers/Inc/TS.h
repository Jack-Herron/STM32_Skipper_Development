/*
 * TS.h
 *
 *  Created on: Jan 1, 2026
 *      Author: jackh
 */

#ifndef INC_TS_H_
#define INC_TS_H_

#include <stdint.h>

#define TS___I2C_ADDRESS		0x38
#define TS___I2C_FREQ			100000
#define TS___I2C_RISE_TIME_ns	100
#define TS___I2C_PORT			1

typedef struct
{
  void       (*Init)(uint16_t);
  uint16_t   (*ReadID)(uint16_t);
  void       (*Reset)(uint16_t);
  void       (*Start)(uint16_t);
  uint8_t    (*DetectTouch)(uint16_t);
  void       (*GetXY)(uint16_t, uint16_t*, uint16_t*);
  void       (*EnableIT)(uint16_t);
  void       (*ClearIT)(uint16_t);
  uint8_t    (*GetITStatus)(uint16_t);
  void       (*DisableIT)(uint16_t);
}TS_DrvTypeDef;

void TS___Init(void);
void TS___Set_Event_Callback(void (*callback)(void));
void TS___Get_Point(uint16_t* x, uint16_t* y);
uint8_t TS___Get_Num_Points_Pressed();
#endif /* INC_TS_H_ */
