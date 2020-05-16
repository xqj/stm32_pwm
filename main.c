#include "stm32f10x.h"
void TIM3_PWM_Init(u16 arr,u16 psc);

//1ms延时函数(72MHz)
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
    //TIM3_PWM_Init(899,0);//不分频，PWM频率=72000/900=80khz
    TIM3_PWM_Init(999,61999);//1HZ,
    delay_nms(100);//观察初始化的pwm
//  TIM_SetCompare2(TIM3,200);//占空比200/999+1=20%
    while(1)
    {
        if(dir)TIMpulse++;
        else TIMpulse--;
        if(TIMpulse>999)dir=0;
        if(TIMpulse==0)dir=1;
        TIM_SetCompare2(TIM3,TIMpulse);//程序中修改占空比TIMpulse/arr
        ;
    }
}
//arr:自动装载值   psc:预分频数
//TIM3 PWM部分初始化
void TIM3_PWM_Init(u16 arr,u16 psc)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO,ENABLE);//开启重映射时钟

    GPIO_PinRemapConfig(GPIO_PartialRemap_TIM3,ENABLE);//重映射TIM3_CH2到PB5输出

    //GPIO设置为复用输出，输出TIM3的CH2的PWM脉冲波形GPIOB5
    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);

    //初始化TIM3
    TIM_TimeBaseStructure.TIM_Period=arr;                   //设置自动重装载周期值(决定PWM的频率)
    TIM_TimeBaseStructure.TIM_Prescaler=psc;                //预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision=0;              //设置时钟分割：TDTS=Tck_tim
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up;//向上计数
    TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);

    //初始化TIM3 Channel2 PWM模式
    TIM_OCInitStructure.TIM_OCMode=TIM_OCMode_PWM2;             //选择PWM模式2，计数值大于比较值有效
    TIM_OCInitStructure.TIM_OutputState=TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity=TIM_OCPolarity_High;     //输出极性 高为有效值
    //配置占空比  500/arr重载计数器=500/999+1，，和函数TIM_SetCompare2(TIM3,TIMpulse);功能一样
    TIM_OCInitStructure.TIM_Pulse=500;
    TIM_OC2Init(TIM3,&TIM_OCInitStructure);                     //初始化外设TIM3 OC2通道
    TIM_OC2PreloadConfig(TIM3,TIM_OCPreload_Enable);            //使能预装载器即TIM3_CCR2的预装载值在更新事件到来时才能被传送至当前寄存器中

    TIM_Cmd(TIM3,ENABLE);                                       //使能TIM3
}
