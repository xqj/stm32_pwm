#ifndef __Tim4ToTim1_H__
#define __Tim4ToTim1_H__
//Tim4-CH1��PB6�����PWM��Tim1-CH2��PA9������

#define MinPwmWidth 1 //��С��PWM��ȣ�us
#define MaxPwmWidth (MinPwmWidth*65535) //�ɲ��������PWM���ڣ�us

void InitTim4ToTim1(void);
void InitTim1Capture(void);//Tim1����
void InitTim4Pwm(unsigned short nUs, unsigned short wUs);//Tim4�������ΪnUs�����ΪwUs����λus����PWM����

extern unsigned short PWM_Width,PWM_Period;
#endif
