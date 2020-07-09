#include "stm32f10x.h"

#define DATA_LEN 1
#define BOUND 9600

char Usart_Data_Buffer;
int count=0;

void delay_ms(u16 time)
{
    u16 i=0;
    while(time--)
    {
        i=12000;
        while(i--);
    }
}
//USART1���ж�
void USART1_IRQHandler(void)
{
   
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        Usart_Data_Buffer =USART_ReceiveData(USART1);//��ȡ�յ�����
    }
		//���-���������Ҫ�ȶ�SR���ٶ�DR�Ĵ����������������жϵ�����
    if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET)
    {
        USART_ReceiveData(USART1);
        USART_ClearFlag(USART1,USART_FLAG_ORE);
    }
    USART_ClearFlag(USART1,USART_IT_RXNE);//��������ж�
		count++;
}
/*
����ͨ��
pwm����
����ٶȿ���
*/
int main()
{

    /*--------------------------------------GPIO-----------------------------------------------*/
    GPIO_InitTypeDef GPIO_InitStrue;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB,ENABLE);//GPIOA�˿�ʹ��,��ӦAPB2
    //TX��Ӧ����RX
    GPIO_InitStrue.GPIO_Mode=GPIO_Mode_AF_PP;
    GPIO_InitStrue.GPIO_Pin=GPIO_Pin_9;
    GPIO_InitStrue.GPIO_Speed=GPIO_Speed_10MHz;
    GPIO_Init(GPIOA,&GPIO_InitStrue);
    //RX��Ӧ����TX
    GPIO_InitStrue.GPIO_Mode=GPIO_Mode_IN_FLOATING;
    GPIO_InitStrue.GPIO_Pin=GPIO_Pin_10;
    GPIO_InitStrue.GPIO_Speed=GPIO_Speed_10MHz;
    GPIO_Init(GPIOA,&GPIO_InitStrue);

    GPIO_InitStrue.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStrue.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStrue.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStrue);
    /*--------------------------------------USART-----------------------------------------------*/
    //Ƕ�������жϿ�������ѡ��
    NVIC_InitTypeDef NVIC_InitStrue;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    NVIC_InitStrue.NVIC_IRQChannel=USART2_IRQn;
    //ʹ���ж�
    NVIC_InitStrue.NVIC_IRQChannelCmd=ENABLE;
    //��ռ���ȼ���1
    NVIC_InitStrue.NVIC_IRQChannelPreemptionPriority=1;
    //�����ȼ���1
    NVIC_InitStrue.NVIC_IRQChannelSubPriority=1;
    //��ʼ������NVIC
    NVIC_Init(&NVIC_InitStrue);
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    //����
    USART_InitTypeDef USART_InitStrue;
    USART_InitStrue.USART_BaudRate=BOUND;//���ڲ�����
    USART_InitStrue.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStrue.USART_Mode=USART_Mode_Tx|USART_Mode_Rx;//�շ�ģʽ
    USART_InitStrue.USART_Parity=USART_Parity_No;//����żУ��λ
    USART_InitStrue.USART_StopBits=USART_StopBits_1;//һ��ֹͣλ
    USART_InitStrue.USART_WordLength=USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ

    USART_Init(USART1,&USART_InitStrue);//��ʼ������1
    USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//���������ж�
    USART_Cmd(USART1,ENABLE);//ʹ�ܴ���1

    while(1)
    {
			if(Usart_Data_Buffer=='1'){
				 GPIO_SetBits(GPIOB,GPIO_Pin_0);
			}
			if(Usart_Data_Buffer=='2'){
				 GPIO_ResetBits(GPIOB,GPIO_Pin_0);
			}
    }
}

