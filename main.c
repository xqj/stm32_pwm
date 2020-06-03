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
    GPIO_InitTypeDef GPIO_InitStruct_A;
    //初始化RCC并使能
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    //设置要初始化的GPIO的管脚
    GPIO_InitStruct_A.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
    //设置要初始化的GPIO的速度
    GPIO_InitStruct_A.GPIO_Speed = GPIO_Speed_50MHz;
    //设置要初始化的GPIO的模式
    GPIO_InitStruct_A.GPIO_Mode = GPIO_Mode_Out_PP;
    //调用初始化函数，传入参数为GPIO初始化结构体的地址
    GPIO_Init(GPIOA, &GPIO_InitStruct_A);
		//使能口
    GPIO_SetBits(GPIOA, GPIO_Pin_0);
    GPIO_ResetBits(GPIOA, GPIO_Pin_1);
    GPIO_SetBits(GPIOA, GPIO_Pin_3);
    GPIO_ResetBits(GPIOA, GPIO_Pin_2);
    // IN1=0,IN2=1
    GPIO_SetBits(GPIOA, GPIO_Pin_4);
    GPIO_ResetBits(GPIOA, GPIO_Pin_5);
    // IN3=0,IN4=1
    GPIO_SetBits(GPIOA, GPIO_Pin_6);
    GPIO_ResetBits(GPIOA, GPIO_Pin_7);
    while(1) {

    }
}
