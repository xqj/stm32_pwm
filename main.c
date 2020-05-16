#include "stm32f10x.h"
void TIM3_PWM_Init(u16 arr,u16 psc);

//1ms��ʱ����(72MHz)
void delay_nms(u16 time)
{
    u16 i=0;
    while(time--)
    {
        i=12000;
        while(i--);
    }
}

int main(void)
{
    u16 TIMpulse=0;
    u8 dir=1;
    //TIM3_PWM_Init(899,0);//����Ƶ��PWMƵ��=72000/900=80khz
    TIM3_PWM_Init(999,61999);//1HZ,
    delay_nms(100);//�۲��ʼ����pwm
//  TIM_SetCompare2(TIM3,200);//ռ�ձ�200/999+1=20%
    while(1)
    {
        if(dir)TIMpulse++;
        else TIMpulse--;
        if(TIMpulse>999)dir=0;
        if(TIMpulse==0)dir=1;
        TIM_SetCompare2(TIM3,TIMpulse);//�������޸�ռ�ձ�TIMpulse/arr
        ;
    }
}
//arr:�Զ�װ��ֵ   psc:Ԥ��Ƶ��
//TIM3 PWM���ֳ�ʼ��
void TIM3_PWM_Init(u16 arr,u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);//������ӳ��ʱ��

    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);//��ӳ��TIM3_CH2��PB5���

    //GPIO����Ϊ������������TIM3��CH2��PWM���岨��GPIOB5
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);

    //��ʼ��TIM3
    TIM_TimeBaseStructure.TIM_Period=arr;                   //�����Զ���װ������ֵ(����PWM��Ƶ��)
    TIM_TimeBaseStructure.TIM_Prescaler=psc;                //Ԥ��Ƶֵ
    TIM_TimeBaseStructure.TIM_ClockDivision=0;              //����ʱ�ӷָTDTS=Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;//���ϼ���
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);

    //��ʼ��TIM3 Channel2 PWMģʽ
    TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM2;             //ѡ��PWMģʽ2������ֵ���ڱȽ�ֵ��Ч
    TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable; //�Ƚ����ʹ��
    TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;     //������� ��Ϊ��Чֵ
    //����ռ�ձ�  500/arr���ؼ�����=500/999+1�����ͺ���TIM_SetCompare2(TIM3,TIMpulse);����һ��
    TIM_OCInitStructure.TIM_Pulse=500;
    TIM_OC2Init(TIM3,&TIM_OCInitStructure);                     //��ʼ������TIM3 OC2ͨ��
    TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);            //ʹ��Ԥװ������TIM3_CCR2��Ԥװ��ֵ�ڸ����¼�����ʱ���ܱ���������ǰ�Ĵ�����

    TIM_Cmd(TIM3,ENABLE);                                       //ʹ��TIM3
}
