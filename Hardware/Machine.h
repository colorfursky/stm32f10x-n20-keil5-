#ifndef __MACHINE_H
#define __MACHINE_H

#define INIT_SPEED 80

#include <stdint.h>

void Machine_Init(void);

uint8_t Machine_CheckCmd(void);
uint8_t Machine_CheckRotation(void);

void Machine_SetSpeed(uint8_t Percentage);
void Machine_ON(void);
void Machine_OFF(void);
void Machine_TURN(void);
void Machine_ForwardRotation(void);
void Machine_ReverseRotation(void);
void Machine_RotationTurn(void);
void Machine_config_init(void);

#endif
