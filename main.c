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
    GPIO_InitTypeDef GPIO_InitStruct;    
    //��ʼ��RCC��ʹ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);    
    //����Ҫ��ʼ����GPIO�Ĺܽ�
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;    
    //����Ҫ��ʼ����GPIO���ٶ�
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;    
    //����Ҫ��ʼ����GPIO��ģʽ
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;    
    //���ó�ʼ���������������ΪGPIO��ʼ���ṹ��ĵ�ַ
    GPIO_Init(GPIOA, &GPIO_InitStruct);    
    //��ѭ���и�GPIO_Pin_1������λ��λ��Ҳ���Ǹ��ߵ͵�ƽ����������������Ӧ�ܽŵ�led
    while(1){        
        //GPIO�ܽ���λ��������һ������Ϊ�Ǹ�GPIO�ڣ��ڶ���ΪGPIO�ڵĵڼ�������
        GPIO_SetBits(GPIOA, GPIO_Pin_0);        
        //GPIO�ܽŸ�λ��������һ������Ϊ�Ǹ�GPIO�ڣ��ڶ���ΪGPIO�ڵĵڼ�������
        GPIO_ResetBits(GPIOA, GPIO_Pin_0);
    }
}
