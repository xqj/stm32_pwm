#ifndef __DELAY_H
#define __DELAY_H 

#include "stm32f10x.h"

void DelayInit(u8 SYSCLK);
void DelayMs(unsigned int nms);
void DelayUs(unsigned long nus);
void DelayS(unsigned int ns);


#endif
