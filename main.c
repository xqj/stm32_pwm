#include "stm32f10x.h"
#include "stdio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x.h"
#include "stm32f10x_iwdg.h"

u8 USART1_RX_BUF[1];
u8 USART1_RX_CNT=0;

void Usart1_Init(u32 bound)
{
    //GPIO�˿�����
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC, ENABLE);//ʹ��USART1,GPIOA,Cʱ��

    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //�����������
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9

    //USART1_RX      GPIOA.10��ʼ��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
    GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10

    //Usart1 NVIC ����
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�   0-3;

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);    //����ָ���Ĳ�����ʼ��VIC�Ĵ���

   //USART ��ʼ������

    USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    //�շ�ģʽ

    USART_Init(USART1, &USART_InitStructure); //��ʼ������1
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
    USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1
}
/**
* USART1����len���ֽ�.
* buf:�������׵�ַ
* len:���͵��ֽ���(Ϊ�˺ͱ�����Ľ���ƥ��,���ｨ�鲻Ҫ����64���ֽ�)
**/
void USART1_Send_Data(u8 *buf,u16 len)
{
    u16 t;
    GPIO_SetBits(GPIOA,GPIO_Pin_9);
//  RS485_TX_EN=1;            //����Ϊ����ģʽ
    for(t=0;t<len;t++)        //ѭ����������
    {
        while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������
        USART_SendData(USART1,buf[t]);
    }
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    GPIO_ResetBits(GPIOA,GPIO_Pin_9);
//    RS485_TX_EN=0;                //����Ϊ����ģʽ
}

/**
* ����ָ�����ȵ��ַ���
* ������չ̶���СΪ21���ֽڵ��ַ���
**/
void USART1_IRQHandler(void)                    //����1�жϷ������
{
    u8 Res;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
        {
            Res =USART_ReceiveData(USART1);    //��ȡ���յ�������
            if(USART1_RX_CNT<1)//���ڽ���ָ�����ȵ��ַ���
            {
                USART1_RX_BUF[USART1_RX_CNT]=Res;        //��¼���յ���ֵ
                USART1_RX_CNT++;                                        //������������1
            }
     }
         //���-������������Ҫ�ȶ�SR,�ٶ�DR�Ĵ����������������жϵ�����
    if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET)
    {
        USART_ReceiveData(USART1);
        USART_ClearFlag(USART1,USART_FLAG_ORE);
    }
     USART_ClearFlag(USART1,USART_IT_RXNE); //һ��Ҫ��������ж�
}
int main(void)
{
    Usart1_Init(9600);//����1����������Ϊ9600  
    while(1)
    {       
        if(USART1_RX_CNT==1)//���ݽ������
        {
            USART1_RX_CNT=0;//ָ�븴λ
            //�����յ������ݷ��ͳ�ȥ
						USART1_RX_BUF[0]='3';
            USART1_Send_Data(USART1_RX_BUF,1);//ͨ������1�����յ��Ĺ̶������ַ����ͳ�ȥ
        }
    }

}
