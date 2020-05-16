#ifndef __OLED_I2C_H
#define	__OLED_I2C_H

#include "stm32f10x.h"

#define OLED_ADDRESS	0x78 //ͨ������0R����,������0x78��0x7A������ַ -- Ĭ��0x78

void OLED_I2C_Configuration(void);
void I2C_WriteByte(uint8_t addr,uint8_t data);
void WriteCmd_I2C(unsigned char I2C_Command);
void WriteDat_I2C(unsigned char I2C_Data);
void OLED_I2C_Init(void);
void OLED_I2C_SetPos(unsigned char x, unsigned char y);
void OLED_I2C_Fill(unsigned char fill_Data);
void OLED_I2C_CLS(void);
void OLED_I2C_ON(void);
void OLED_I2C_OFF(void);
void OLED_I2C_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize);
void OLED_I2C_ShowCN(unsigned char x, unsigned char y, unsigned char N);
void OLED_I2C_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);

#endif
