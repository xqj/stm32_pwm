#ifndef __Tim4ToTim1_H__
#define __Tim4ToTim1_H__
//Tim4-CH1（PB6）输出PWM到Tim1-CH2（PA9）捕获

#define MinPwmWidth 1 //最小的PWM宽度，us
#define MaxPwmWidth (MinPwmWidth*65535) //可测量的最大PWM周期，us

void InitTim4ToTim1(void);
void InitTim1Capture(void);//Tim1捕获
void InitTim4Pwm(unsigned short nUs, unsigned short wUs);//Tim4输出周期为nUs，宽度为wUs（单位us）的PWM波形

extern unsigned short PWM_Width,PWM_Period;
#endif
