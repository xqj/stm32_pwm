
#include "OLED_I2C.h"
#include "delay.h"
#include "codetab.h"

void OLED_I2C_Configuration(void)
{
	I2C_InitTypeDef  I2C_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure; 

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);

	/*STM32F103C8T6芯片的硬件I2C: PB10 -- SCL; PB11 -- SDA */
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;//I2C必须开漏输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	I2C_DeInit(I2C2);//使用I2C2
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x30;//主机的I2C地址,随便写的
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 400000;//400K

	I2C_Cmd(I2C2, ENABLE);
	I2C_Init(I2C2, &I2C_InitStructure);
}

void I2C_WriteByte(uint8_t addr,uint8_t data)
{
  while(I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY));
	
	I2C_GenerateSTART(I2C2, ENABLE);//开启I2C2
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT));/*EV5,主模式*/

	I2C_Send7bitAddress(I2C2, OLED_ADDRESS, I2C_Direction_Transmitter);//器件地址 -- 默认0x78
	while(!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

	I2C_SendData(I2C2, addr);//寄存器地址
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

	I2C_SendData(I2C2, data);//发送数据
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
	
	I2C_GenerateSTOP(I2C2, ENABLE);//关闭I2C2总线
}

void WriteCmd_I2C(unsigned char I2C_Command)//写命令
{
	I2C_WriteByte(0x00, I2C_Command);
}

void WriteDat_I2C(unsigned char I2C_Data)//写数据
{
	I2C_WriteByte(0x40, I2C_Data);
}

void OLED_I2C_Init(void)
{
	DelayMs(100); //这里的延时很重要
	
	WriteCmd_I2C(0xAE); //display off
	WriteCmd_I2C(0x20);	//Set Memory Addressing Mode	
	WriteCmd_I2C(0x10);	//00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
	WriteCmd_I2C(0xb0);	//Set Page Start Address for Page Addressing Mode,0-7
	WriteCmd_I2C(0xc8);	//Set COM Output Scan Direction
	WriteCmd_I2C(0x00); //---set low column address
	WriteCmd_I2C(0x10); //---set high column address
	WriteCmd_I2C(0x40); //--set start line address
	WriteCmd_I2C(0x81); //--set contrast control register
	WriteCmd_I2C(0xff); //亮度调节 0x00~0xff
	WriteCmd_I2C(0xa1); //--set segment re-map 0 to 127
	WriteCmd_I2C(0xa6); //--set normal display
	WriteCmd_I2C(0xa8); //--set multiplex ratio(1 to 64)
	WriteCmd_I2C(0x3F); //
	WriteCmd_I2C(0xa4); //0xa4,Output follows RAM content;0xa5,Output ignores RAM content
	WriteCmd_I2C(0xd3); //-set display offset
	WriteCmd_I2C(0x00); //-not offset
	WriteCmd_I2C(0xd5); //--set display clock divide ratio/oscillator frequency
	WriteCmd_I2C(0xf0); //--set divide ratio
	WriteCmd_I2C(0xd9); //--set pre-charge period
	WriteCmd_I2C(0x22); //
	WriteCmd_I2C(0xda); //--set com pins hardware configuration
	WriteCmd_I2C(0x12);
	WriteCmd_I2C(0xdb); //--set vcomh
	WriteCmd_I2C(0x20); //0x20,0.77xVcc
	WriteCmd_I2C(0x8d); //--set DC-DC enable
	WriteCmd_I2C(0x14); //
	WriteCmd_I2C(0xaf); //--turn on oled panel
}

void OLED_I2C_SetPos(unsigned char x, unsigned char y) //设置起始点坐标
{ 
	WriteCmd_I2C(0xb0+y);
	WriteCmd_I2C(((x&0xf0)>>4)|0x10);
	WriteCmd_I2C((x&0x0f)|0x01);
}

void OLED_I2C_Fill(unsigned char fill_Data)//全屏填充
{
	unsigned char m,n;
	for(m=0;m<8;m++)
	{
		WriteCmd_I2C(0xb0+m);		//page0-page1
		WriteCmd_I2C(0x00);		//low column start address
		WriteCmd_I2C(0x10);		//high column start address
		for(n=0;n<128;n++)
			{
				WriteDat_I2C(fill_Data);
			}
	}
}

void OLED_I2C_CLS(void)//清屏
{
	OLED_I2C_Fill(0x00);
}

//--------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Calls          : 
// Parameters     : none
// Description    : 将OLED从休眠中唤醒
//--------------------------------------------------------------
void OLED_I2C_ON(void)
{
	WriteCmd_I2C(0X8D);  //设置电荷泵
	WriteCmd_I2C(0X14);  //开启电荷泵
	WriteCmd_I2C(0XAF);  //OLED唤醒
}

//--------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Calls          : 
// Parameters     : none
// Description    : 让OLED休眠 -- 休眠模式下,OLED功耗不到10uA
//--------------------------------------------------------------
void OLED_I2C_OFF(void)
{
	WriteCmd_I2C(0X8D);  //设置电荷泵
	WriteCmd_I2C(0X10);  //关闭电荷泵
	WriteCmd_I2C(0XAE);  //OLED休眠
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowChar(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
// Calls          : 
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); ch[] -- 要显示的字符串; TextSize -- 字符大小(1:6*8 ; 2:8*16)
// Description    : 显示codetab.h中的ASCII字符,有6*8和8*16可选择
//--------------------------------------------------------------
void OLED_I2C_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
{
	unsigned char c = 0,i = 0,j = 0;
	switch(TextSize)
	{
		case 1:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 126)
				{
					x = 0;
					y++;
				}
				OLED_I2C_SetPos(x,y);
				for(i=0;i<6;i++)
					WriteDat_I2C(F6x8_I2C[c][i]);
				x += 6;
				j++;
			}
		}break;
		case 2:
		{
			while(ch[j] != '\0')
			{
				c = ch[j] - 32;
				if(x > 120)
				{
					x = 0;
					y++;
				}
				OLED_I2C_SetPos(x,y);
				for(i=0;i<8;i++)
					WriteDat_I2C(F8X16_I2C[c*16+i]);
				OLED_I2C_SetPos(x,y+1);
				for(i=0;i<8;i++)
					WriteDat_I2C(F8X16_I2C[c*16+i+8]);
				x += 8;
				j++;
			}
		}break;
	}
}

//--------------------------------------------------------------
// Prototype      : void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N)
// Calls          : 
// Parameters     : x,y -- 起始点坐标(x:0~127, y:0~7); N:汉字在codetab.h中的索引
// Description    : 显示codetab.h中的汉字,16*16点阵
//--------------------------------------------------------------
void OLED_I2C_ShowCN(unsigned char x, unsigned char y, unsigned char N)
{
	unsigned char wm=0;
	unsigned int  adder=32*N;
	OLED_I2C_SetPos(x , y);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat_I2C(F16x16[adder]);
		adder += 1;
	}
	OLED_I2C_SetPos(x,y + 1);
	for(wm = 0;wm < 16;wm++)
	{
		WriteDat_I2C(F16x16[adder]);
		adder += 1;
	}
}

//--------------------------------------------------------------
// Prototype      : void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[]);
// Calls          : 
// Parameters     : x0,y0 -- 起始点坐标(x0:0~127, y0:0~7); x1,y1 -- 起点对角线(结束点)的坐标(x1:1~128,y1:1~8)
// Description    : 显示BMP位图
//--------------------------------------------------------------
void OLED_I2C_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0)
		y = y1/8;
  else
		y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		OLED_I2C_SetPos(x0,y);
    for(x=x0;x<x1;x++)
		{
			WriteDat_I2C(BMP[j++]);
		}
	}
}
