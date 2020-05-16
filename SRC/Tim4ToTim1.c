#include "Tim4ToTim1.h"
#include "stm32f10x.h"
#include "stm32f10x_tim.h"

unsigned short PWM_Width,PWM_Period;

//Tim4-CH1��PB6�����PWM��Tim1-CH2��PA9������
void InitTim4ToTim1(void)
{
	InitTim4Pwm(20000, 5000);
	InitTim1Capture();
}

void InitTim1Capture(void)//Tim1-CH2��PA9������
{
	NVIC_InitTypeDef NVIC_InitStructure;//�����ж�Դ
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
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 , ENABLE);//APB2����ʱ��Ϊ����
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_ICInitTypeDef  TIM_ICInitStructure;
	TIM_DeInit(TIM1);

//	 �Զ���װ�ؼĴ������ڵ�ֵ(����ֵ) 
	TIM_TimeBaseStructure.TIM_Period=65535;//����ʱ���
	TIM_TimeBaseStructure.TIM_Prescaler=72*MinPwmWidth-1;//ʱ��Ԥ��Ƶ��72Mhz/72=1000kHz
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;//��APB1����ʱ��72Mhz�ķ�Ƶ,���Է���û������
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);

  TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;  //PA9
  TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//TIM_ICPolarity_Falling,TIM_ICPolarity_Rising,TIM_ICPolarity_BothEdge
  TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
  TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;  //
  TIM_ICInitStructure.TIM_ICFilter=0x00;
	TIM_ICInit(TIM1, &TIM_ICInitStructure);
	TIM_PWMIConfig(TIM1,&TIM_ICInitStructure);//����PWM�ź�
	TIM_SelectInputTrigger(TIM1, TIM_TS_TI2FP2);//IC2Ϊ������  
  TIM_SelectSlaveMode(TIM1, TIM_SlaveMode_Reset);//TIM��ģʽ�������źŵ����������³�ʼ���������ʹ����Ĵ��������¼�
  TIM_SelectMasterSlaveMode(TIM1, TIM_MasterSlaveMode_Enable);
	TIM_ARRPreloadConfig(TIM1, ENABLE);///no use TIM1->ARR 
	TIM_ITConfig(TIM1, TIM_IT_CC2|TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM1, ENABLE);
}

void TIM1_UP_IRQHandler()
{
	TIM1->SR &= (uint16_t)~TIM_IT_Update;//���ж�
}

void TIM1_CC_IRQHandler(unsigned short nUs, unsigned short wUs)
{
		PWM_Width = TIM1->CCR1;//TIM_GetCapture1(TIM1)
		PWM_Period= TIM1->CCR2;//TIM_GetCapture2(TIM1)
		TIM1->SR &= (uint16_t)~TIM_IT_CC2;
}

void InitTim4Pwm(unsigned short nUs, unsigned short wUs)//Tim4�������ΪnUs�����ΪwUs����λus����PWM����
{
  GPIO_InitTypeDef GPIO_InitStructure;
  /* GPIOB clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //APB2����ʱ��Ϊ����
  /*GPIOB Configuration: TIM4 channel 1-4 as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

	//����TIM4CLK PWM
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);//APB1����ʱ��Ϊ����
	TIM_DeInit(TIM4);
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;//��ʱ��������ʼ������
	TIM_OCInitTypeDef  TIM_OCInitStructure;//��ʱ����ȽϿ������ṹ��

	// �Զ���װ�ؼĴ������ڵ�ֵ(����ֵ) 
	TIM_TimeBaseStructure.TIM_Period=nUs;//1000kHz/20000=50Hz
	TIM_TimeBaseStructure.TIM_Prescaler=72-1;//ʱ��Ԥ��Ƶ��72Mhz/72=1MHz
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;//��APB1����ʱ��72Mhz�ķ�Ƶ,���Է���û������
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;//
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_Pulse = wUs;
	TIM_OC1Init(TIM4, &TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_Pulse = 2340;//A2212<2.34ms��ת
	TIM_OC2Init(TIM4, &TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_Pulse = 1174;
	TIM_OC3Init(TIM4, &TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_Pulse = 1174;//A2212>1.174ms��ת
	TIM_OC4Init(TIM4, &TIM_OCInitStructure);
	
	TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(TIM4, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
}
