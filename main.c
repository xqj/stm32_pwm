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
  //定义一个GPIO初始化结构体，要在RCC使能之前定义，否则会报错
    GPIO_InitTypeDef GPIO_InitStruct;    
    //初始化RCC并使能
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);    
    //设置要初始化的GPIO的管脚
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;    
    //设置要初始化的GPIO的速度
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    
    //设置要初始化的GPIO的模式
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;    
    //调用初始化函数，传入参数为GPIO初始化结构体的地址
    GPIO_Init(GPIOA, &GPIO_InitStruct);    
    //使能1
	  GPIO_SetBits(GPIOA, GPIO_Pin_0);  
		//使不能1
	  //GPIO_ResetBits(GPIOA, GPIO_Pin_0);
	  //方向控制
		//GPIO_SetBits(GPIOA, GPIO_Pin_2); 	
		//GPIO_ResetBits(GPIOA, GPIO_Pin_2);
		//控制速度
	  unsigned long delayLms=50;
    while(1){        
       //控制速度
        GPIO_SetBits(GPIOA, GPIO_Pin_1);  
				GPIO_ResetBits(GPIOA, GPIO_Pin_2);			  			
			  delay_nms(delayLms);
			  GPIO_ResetBits(GPIOA, GPIO_Pin_1);	
				GPIO_SetBits(GPIOA, GPIO_Pin_2);        		  
			  delay_nms(delayLms);
    }
}
