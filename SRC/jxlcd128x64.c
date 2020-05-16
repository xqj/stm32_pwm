#include "delay.h"
#include "jxlcd128x64.h"
//#include "codetab.h"

void JLX12864G_086_GPIOInit(void)  
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	//�������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	//�ٶ�50MHZ
  GPIO_Init(GPIOB,&GPIO_InitStructure);		
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8;	//ROM_OUT
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;	//��������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	//�ٶ�50MHZ
  GPIO_Init(GPIOA,&GPIO_InitStructure);	
}

void transfer_command_lcd(int data1) //дָ�LCD
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

void transfer_data_lcd(int data1) //д���ݵ�LCD
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

void initial_lcd() //LCD��ʼ��
{
	lcd_reset_l; 
	DelayMs(10);//��λ���ֵ͵�ƽ��ʱ��>3us
	lcd_reset_h; 
	DelayMs(20);//��λ���ڲ�״̬��ʱ>6ms
	transfer_command_lcd(0xe2); //�����λ��Reset��ָ��
	DelayMs(10);
	//0x2C,0x2E,0x2F ����ָ�˳�������д��
	//��ʾ���δ��ڲ���ѹ����ѹ������·����ѹ��������
	//Ҳ���Ե���д0x2F��һ���Դ������ֵ�·��
	transfer_command_lcd(0x2c); 
	DelayMs(20);
	transfer_command_lcd(0x2e); 
	DelayMs(20);
	transfer_command_lcd(0x2f); 
	DelayMs(20);
	transfer_command_lcd(0x23); //�ֵ��Աȶ�ֵ�������÷�ΧΪ��0x20��0x27����ֵԽ��Աȶ�Խǿ	
	transfer_command_lcd(0x81); //΢���Աȶ�ֵ��������ָ���������ʹ��
	transfer_command_lcd(0x28); //���÷�ΧΪ��0x00��0x3F,��ֵԽ��Աȶ�Խǿ	
	transfer_command_lcd(0xa2); //����ƫѹ�ȣ�0XA2��BIAS=1/9 (����)��0XA3��BIAS=1/7
	transfer_command_lcd(0xc8); //��ɨ��˳��ѡ��0XC0=��ͨɨ��˳�򣬴��ϵ��£�0XC8=��תɨ��˳�򣬴��µ���
	transfer_command_lcd(0xa0); //��ʾ�е�ַ������0xA0=���棬�е�ַ�����ң�0xA1=��ת���е�ַ���ҵ���
	transfer_command_lcd(0x40); //������ʾ�洢������ʾ��ʼ��,������ֵΪ0X40~0X7F,�ֱ�����0��63��
	transfer_command_lcd(0xaf); //��ʾ��/�أ�0XAE=�أ�0XAF=����
	lcd_cs1_h;
}

//page��1-8��column��1-128
void lcd_address(uint page,uint column)//ÿ8 ��Ϊһ��ҳ��64 �з�Ϊ8��ҳ
{
	lcd_cs1_l;
	column=column-0x01;
	transfer_command_lcd(0xb0+page-1);
	transfer_command_lcd(0x10+((column>>4)&0x0f));//��4 λ���4 λ��ͬ����е�ַ
	transfer_command_lcd(column&0x0f);
}

void clear_screen() //ȫ������
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

void display_128x64(uchar *dp) //��ʾ128*64�ĵ���ͼ��
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

//page��1-8��column��1-128
void display_graphic_16x16(uchar page,uchar column,uchar *dp) //��ʾ16*16�ĵ���ͼ�񡢺��֡���Ƨ�ֻ�16*16���������ͼ��
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

//page��1-8��column��1-128
void display_graphic_8x16(uchar page,uchar column,uchar *dp) //��ʾ8*16����ͼ��ASCII����8*16����������ַ�������ͼ��
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
//page��1-8��column��1-128
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

void send_command_to_ROM( uchar datu ) //��ָ��ֿ�
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

uchar get_data_from_ROM( ) //���ֿ���ȡ���ֻ��ַ�
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

