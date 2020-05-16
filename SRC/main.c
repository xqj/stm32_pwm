#include "stm32f10x.h"
#include "delay.h"
#include "stm32f10x_tim.h"
#include "Tim4ToTim1.h"

//PWM波形输出与测量实例
//TIM4-Ch1(PB6) 输出PWM波形到
//Tim1-CH2（PA9）捕获

void InitPio()
{
	GPIO_InitTypeDef GPIO_InitStructure;
//	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
//	GPIO_Init(GPIOB ,&GPIO_InitStructure) ;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC , ENABLE); 	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOC ,&GPIO_InitStructure) ;

//	GPIO_SetBits(GPIOB ,GPIO_Pin_0|GPIO_Pin_1) ;
}

uint32_t CntsforMs=0;
void InitTimeCounter(void)//计时器
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟
	
  TIM_TimeBaseInitStructure.TIM_Period = 1000; 	//自动重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler=71;  //定时器分频:72Mhz/(71+1)=1Mhz
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//初始化TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
	TIM_Cmd(TIM3,ENABLE); //使能定时器3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//定时器3中断服务函数
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
	{
		CntsforMs++;
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
	}
}

int main()
{
  int cnts=0;	
	DelayInit(72);
	InitPio();
	InitTimeCounter();//只用TIM3来计时
	DelayS(5);	
	while(1)
	{
		cnts++;
		
		if ((cnts%2)==0)
			{			
				GPIO_SetBits(GPIOB ,GPIO_Pin_0);
				GPIO_SetBits(GPIOC ,GPIO_Pin_13);
	//			TIM4->CCR3 = 1000; 
			}
		else
			{
				GPIO_ResetBits(GPIOB ,GPIO_Pin_0);
				GPIO_ResetBits(GPIOC ,GPIO_Pin_13);
	//			TIM4->CCR3 = 9000; 
			}		
	
  }
}





