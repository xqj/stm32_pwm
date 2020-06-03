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
    //����һ��GPIO��ʼ���ṹ�壬Ҫ��RCCʹ��֮ǰ���壬����ᱨ��
    GPIO_InitTypeDef GPIO_InitStruct_A;
    //��ʼ��RCC��ʹ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    //����Ҫ��ʼ����GPIO�Ĺܽ�
    GPIO_InitStruct_A.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
    //����Ҫ��ʼ����GPIO���ٶ�
    GPIO_InitStruct_A.GPIO_Speed = GPIO_Speed_50MHz;
    //����Ҫ��ʼ����GPIO��ģʽ
    GPIO_InitStruct_A.GPIO_Mode = GPIO_Mode_Out_PP;
    //���ó�ʼ���������������ΪGPIO��ʼ���ṹ��ĵ�ַ
    GPIO_Init(GPIOA, &GPIO_InitStruct_A);
		//ʹ�ܿ�
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
