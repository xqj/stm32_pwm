#include "stm32f10x.h"
#include "delay.h"
#include "stm32f10x_tim.h"
#include "Tim4ToTim1.h"

//PWM������������ʵ��
//TIM4-Ch1(PB6) ���PWM���ε�
//Tim1-CH2��PA9������

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
void InitTimeCounter(void)//��ʱ��
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///ʹ��TIM3ʱ��
	
  TIM_TimeBaseInitStructure.TIM_Period = 1000; 	//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler=71;  //��ʱ����Ƶ:72Mhz/(71+1)=1Mhz
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//��ʼ��TIM3
	
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
	TIM_Cmd(TIM3,ENABLE); //ʹ�ܶ�ʱ��3
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //��ʱ��3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

//��ʱ��3�жϷ�����
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
	{
		CntsforMs++;
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
	}
}

int main()
{
  int cnts=0;	
	DelayInit(72);
	InitPio();
	InitTimeCounter();//ֻ��TIM3����ʱ
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





