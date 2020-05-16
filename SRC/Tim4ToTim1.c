#include "Tim4ToTim1.h"
#include "stm32f10x.h"
#include "stm32f10x_tim.h"

unsigned short PWM_Width,PWM_Period;

//Tim4-CH1（PB6）输出PWM到Tim1-CH2（PA9）捕获
void InitTim4ToTim1(void)
{
	InitTim4Pwm(20000, 5000);
	InitTim1Capture();
}

void InitTim1Capture(void)//Tim1-CH2（PA9）捕获
{
	NVIC_InitTypeDef NVIC_InitStructure;//配置中断源
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn|TIM1_CC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA , ENABLE); 	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING ; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz ;
	GPIO_Init(GPIOA ,&GPIO_InitStructure) ;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);//APB2外设时钟为高速
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	TIM_DeInit(TIM1);

//	 自动重装载寄存器周期的值(计数值) 
	TIM_TimeBaseStructure.TIM_Period=65535;//超过时溢出
	TIM_TimeBaseStructure.TIM_Prescaler=72*MinPwmWidth-1;//时钟预分频数72Mhz/72=1000kHz
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;//对APB1外设时钟72Mhz的分频,调试发现没起作用
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;  //PA9
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//TIM_ICPolarity_Falling,TIM_ICPolarity_Rising,TIM_ICPolarity_BothEdge
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;  //
  TIM_ICInitStructure.TIM_ICFilter=0x00;
	TIM_ICInit(TIM1, &TIM_ICInitStructure);
	TIM_PWMIConfig(TIM1,&TIM_ICInitStructure);//捕获PWM信号
	TIM_SelectInputTrigger(TIM1, TIM_TS_TI2FP2);//IC2为触发沿  
  TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Reset);//TIM从模式：触发信号的上升沿重新初始化计数器和触发寄存器更新事件
  TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);
	TIM_ARRPreloadConfig(TIM1, ENABLE);///no use TIM1->ARR 
	TIM_ITConfig(TIM1, TIM_IT_CC2|TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM1, ENABLE);
}

void TIM1_UP_IRQHandler()
{
	TIM1->SR &= (uint16_t)~TIM_IT_Update;//清中断
}

void TIM1_CC_IRQHandler(unsigned short nUs, unsigned short wUs)
{
		PWM_Width = TIM1->CCR1;//TIM_GetCapture1(TIM1)
		PWM_Period= TIM1->CCR2;//TIM_GetCapture2(TIM1)
		TIM1->SR &= (uint16_t)~TIM_IT_CC2;
}

void InitTim4Pwm(unsigned short nUs, unsigned short wUs)//Tim4输出周期为nUs，宽度为wUs（单位us）的PWM波形
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //APB2外设时钟为高速
  /*GPIOB Configuration: TIM4 channel 1-4 as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	//设置TIM4CLK PWM
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//APB1外设时钟为低速
	TIM_DeInit(TIM4);
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//定时器基本初始化数据
	TIM_OCInitTypeDef  TIM_OCInitStructure;//定时输出比较控制器结构体

	// 自动重装载寄存器周期的值(计数值) 
	TIM_TimeBaseStructure.TIM_Period=nUs;//1000kHz/20000=50Hz
	TIM_TimeBaseStructure.TIM_Prescaler=72-1;//时钟预分频数72Mhz/72=1MHz
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;//对APB1外设时钟72Mhz的分频,调试发现没起作用
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_Pulse = wUs;
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_Pulse = 2340;//A2212<2.34ms起转
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_Pulse = 1174;
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_Pulse = 1174;//A2212>1.174ms起转
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
}
