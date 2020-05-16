#include "stm32f10x.h"

void delay_nus(unsigned long n)
{
	unsigned long j;
	while(n--)
	{
		j=8;
		while(j--);
	}
}
void delay_nms(unsigned long n)
{
	while(n--)
		delay_nus(1100);
}
int main(void)
{ 
 
    while(1)
    {
       GPIO_SetBits(GPIOB,GPIO_Pin_1);
			 delay_nms(500);
			 GPIO_ResetBits(GPIOB,GPIO_Pin_1);
			 delay_nms(500);
    }
}
