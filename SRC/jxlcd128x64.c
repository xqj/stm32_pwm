#include "delay.h"
#include "jxlcd128x64.h"
//#include "codetab.h"

void JLX12864G_086_GPIOInit(void)  
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	//推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	//速度50MHZ
  GPIO_Init(GPIOB,&GPIO_InitStructure);		
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;	//ROM_OUT
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;	//上拉输入
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	//速度50MHZ
  GPIO_Init(GPIOA,&GPIO_InitStructure);	
}

void transfer_command_lcd(int data1) //写指令到LCD
{
	char i;
	lcd_cs1_l;
	lcd_rs_l;
	for(i=0;i<8;i++)
	{
		lcd_sck_l;
		if(data1&0x80)
			lcd_sda_h;
		else
			lcd_sda_l; 
		data1=(data1<<=1);
		lcd_sck_h;
	}
	lcd_cs1_h;
}

void transfer_data_lcd(int data1) //写数据到LCD
{
	char i;
	lcd_cs1_l;
	lcd_rs_h;
	for(i=0;i<8;i++)
	{
		lcd_sck_l;
		if(data1&0x80)
			lcd_sda_h;
		else
			lcd_sda_l;
		data1=(data1<<=1);
		lcd_sck_h;
	}
	lcd_cs1_h;
}

void initial_lcd() //LCD初始化
{
	lcd_reset_l; 
	DelayMs(10);//复位保持低电平的时间>3us
	lcd_reset_h; 
	DelayMs(20);//复位到内部状态延时>6ms
	transfer_command_lcd(0xe2); //软件复位（Reset）指令
	DelayMs(10);
	//0x2C,0x2E,0x2F 三条指令按顺序紧接着写，
	//表示依次打开内部升压、电压调整电路、电压跟随器。
	//也可以单单写0x2F，一次性打开三部分电路。
	transfer_command_lcd(0x2c); 
	DelayMs(20);
	transfer_command_lcd(0x2e); 
	DelayMs(20);
	transfer_command_lcd(0x2f); 
	DelayMs(20);
	transfer_command_lcd(0x23); //粗调对比度值。可设置范围为：0x20～0x27，数值越大对比度越强	
	transfer_command_lcd(0x81); //微调对比度值，与下面指令需紧接着使用
	transfer_command_lcd(0x28); //设置范围为：0x00～0x3F,数值越大对比度越强	
	transfer_command_lcd(0xa2); //设置偏压比：0XA2：BIAS=1/9 (常用)；0XA3：BIAS=1/7
	transfer_command_lcd(0xc8); //行扫描顺序选择：0XC0=普通扫描顺序，从上到下；0XC8=反转扫描顺序，从下到上
	transfer_command_lcd(0xa0); //显示列地址增减：0xA0=常规，列地址从左到右；0xA1=反转，列地址从右到左
	transfer_command_lcd(0x40); //设置显示存储器的显示初始行,可设置值为0X40~0X7F,分别代表第0～63行
	transfer_command_lcd(0xaf); //显示开/关：0XAE=关；0XAF=开。
	lcd_cs1_h;
}

//page：1-8；column：1-128
void lcd_address(uint page,uint column)//每8 行为一个页，64 行分为8个页
{
	lcd_cs1_l;
	column=column-0x01;
	transfer_command_lcd(0xb0+page-1);
	transfer_command_lcd(0x10+((column>>4)&0x0f));//高4 位与低4 位共同组成列地址
	transfer_command_lcd(column&0x0f);
}

void clear_screen() //全屏清屏
{
	lcd_cs1_l;
	unsigned char i,j;
	for(i=0;i<9;i++)
	{
		transfer_command_lcd(0xb0+i);
		transfer_command_lcd(0x10);
		transfer_command_lcd(0x00);
		for(j=0;j<132;j++)
		{
			transfer_data_lcd(0x00);
		}
	}
	lcd_cs1_h;
}

void display_128x64(uchar *dp) //显示128*64的点阵图像
{
	uint i,j;
	for(j=0;j<8;j++)
	{
		lcd_address(j+1,1);
		for (i=0;i<128;i++)
		{
			transfer_data_lcd(*dp);
			dp++;
		}
	}
}

//page：1-8；column：1-128
void display_graphic_16x16(uchar page,uchar column,uchar *dp) //显示16*16的点阵图像、汉字、生僻字或16*16点阵的其它图标
{
	uint i,j;
	for(j=0;j<2;j++)
	{
		lcd_address(page+j,column);
		for (i=0;i<16;i++)
		{
			transfer_data_lcd(*dp);
			dp++;
		}
	}
}

//page：1-8；column：1-128
void display_graphic_8x16(uchar page,uchar column,uchar *dp) //显示8*16点阵图像、ASCII、或8*16点阵的自造字符、其它图标
{
	uint i,j;
	for(j=0;j<16;j++)
	{
//		lcd_address(page+j,column);
		for (i=0;i<256;i++)
		{
//			uchar c = (*dp) - 32;			
//			transfer_data_lcd(F8X16_I2C[c*16+j*8+i]); 
//			dp++;
		lcd_address(j,i);
			transfer_data_lcd(0x55); 
		}
	}
}
//page：1-8；column：1-128
void display_graphic_5x8(uchar page,uchar column,uchar *dp) //5*8
{
	uint i;
	lcd_address(page,column);
	for (i=0;i<6;i++)
	{
		transfer_data_lcd(*dp);
		dp++;
	}
}

void send_command_to_ROM( uchar datu ) //送指令到字库
{
	uchar i;
	for(i=0;i<8;i++ )
	{
		Rom_SCK_l;
		if(datu&0x80)
			Rom_IN_h;
		else
			Rom_IN_l;
		datu = datu<<1;
		Rom_SCK_h;
	}
}

uchar get_data_from_ROM( ) //从字库中取汉字或字符
{
	uchar i;
	uchar ret_data=0;
	for(i=0;i<8;i++)
	{
		Rom_OUT_h;
		Rom_SCK_l;
	//	DelayUs(1);//
		if(Rom_OUT)  //
			ret_data=ret_data+1;
		else
			ret_data=ret_data+0;
		ret_data=ret_data<<1;
		Rom_SCK_h;
//		DelayUs(1);//
	}
	return(ret_data);
}

void get_and_write_16x16(ulong fontaddr,uchar page,uchar column)
{
	uchar i,j,disp_data;
	Rom_CS_l;
	send_command_to_ROM(0x03);
	send_command_to_ROM((fontaddr&0xff0000)>>16);
	send_command_to_ROM((fontaddr&0xff00)>>8);
	send_command_to_ROM(fontaddr&0xff);
	for(j=0;j<2;j++)
	{
		lcd_address(page+j,column);
		for(i=0; i<16; i++ )
		{
			disp_data=get_data_from_ROM();
			transfer_data_lcd(disp_data);
		}
	}
	Rom_CS_h;
}

void get_and_write_8x16(ulong fontaddr,uchar page,uchar column)
{
	uchar i,j,disp_data;
	Rom_CS_l;
	send_command_to_ROM(0x03);
	send_command_to_ROM((fontaddr&0xff0000)>>16);
	send_command_to_ROM((fontaddr&0xff00)>>8); 
	send_command_to_ROM(fontaddr&0xff); 
	for(j=0;j<2;j++)
	{
		lcd_address(page+j,column);
		for(i=0; i<8; i++ )
		{
			disp_data=get_data_from_ROM();
			transfer_data_lcd(disp_data);
		}
	}
	Rom_CS_h;
}

void get_and_write_5x8(ulong fontaddr,uchar page,uchar column)
{
	uchar i,disp_data;
	Rom_CS_l;
	send_command_to_ROM(0x03);
	send_command_to_ROM((fontaddr&0xff0000)>>16);
	send_command_to_ROM((fontaddr&0xff00)>>8);
	send_command_to_ROM(fontaddr&0xff);
	lcd_address(page,column);
	for(i=0; i<5; i++ )
	{
		disp_data=get_data_from_ROM();
		transfer_data_lcd(disp_data);
	}
	Rom_CS_h;
}

ulong fontaddr=0;
void display_GB2312_string(uchar page,uchar column,uchar *text)
{
	uchar i= 0;
	while((text[i]>0x00))
	{
		if(((text[i]>=0xb0) &&(text[i]<=0xf7))&&(text[i+1]>=0xa1))
		{
			fontaddr = (text[i]- 0xb0)*94;
			fontaddr += (text[i+1]-0xa1)+846;
			fontaddr = (ulong)(fontaddr*32);
			get_and_write_16x16(fontaddr,page,column);
			i+=2;
			column+=16;
		}
		else if(((text[i]>=0xa1) &&(text[i]<=0xa3))&&(text[i+1]>=0xa1))
		{
			fontaddr = (text[i]- 0xa1)*94;
			fontaddr += (text[i+1]-0xa1);
			fontaddr = (ulong)(fontaddr*32);
			get_and_write_16x16(fontaddr,page,column);
			i+=2;
			column+=16;
		}
		else if((text[i]>=0x20) &&(text[i]<=0x7e))
		{
			fontaddr = (text[i]- 0x20);
			fontaddr = (unsigned long)(fontaddr*16);
			fontaddr = (unsigned long)(fontaddr+0x3cf80);
			get_and_write_8x16(fontaddr,page,column);
			i+=1;
			column+=8;
		}
		else
		i++;
	}
}

void display_string_5x8(uchar page,uchar column,uchar *text)
{
	unsigned char i= 0;
	while((text[i]>0x00))
	{
		if((text[i]>=0x20) &&(text[i]<=0x7e))
		{
			fontaddr = (text[i]- 0x20);
			fontaddr = (unsigned long)(fontaddr*8);
			fontaddr = (unsigned long)(fontaddr+0x3bfc0);
			get_and_write_5x8(fontaddr,page,column);
			i+=1;
			column+=6;
		}
		else
		i++;
	}
}

