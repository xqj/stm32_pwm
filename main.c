#include "stm32f10x.h"
#include "stdio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x.h"
#include "stm32f10x_iwdg.h"

u8 USART1_RX_BUF[1];
u8 USART1_RX_CNT=0;

void Usart1_Init(u32 bound)
{
    //GPIO???������??
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);//��1?��USART1,GPIOA,C����?��

    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //?�䨮?��?������?3?
    GPIO_Init(GPIOA, &GPIO_InitStructure);//3?��??��GPIOA.9

    //USART1_RX      GPIOA.103?��??��
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//????��?��?
    GPIO_Init(GPIOA, &GPIO_InitStructure);//3?��??��GPIOA.10
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //?�䨮?��?������?3?
    GPIO_Init(GPIOB, &GPIO_InitStructure);//3?��??��GPIOA.9

    //Usart1 NVIC ????
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //����??NVIC?D??��?����2:2???��??��??��??��?2???����|��??��??   0-3;

    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//?��??��??��??3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //������??��??3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQ�����̨���1?��
    NVIC_Init(&NVIC_InitStructure);    //?��?Y???����?2?��y3?��??��VIC??��??��

   //USART 3?��??������??

    USART_InitStructure.USART_BaudRate = bound;//��??��2����??��
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//��?3��?a8??��y?Y??��?
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//��???����?1??
    USART_InitStructure.USART_Parity = USART_Parity_No;//?T????D��?��??
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//?T��2?t��y?Y����????
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    //��?����?�꨺?

    USART_Init(USART1, &USART_InitStructure); //3?��??����??��1
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//?a??��??��?������?D??
    USART_Cmd(USART1, ENABLE);                    //��1?����??��1
}
/**
* USART1����?��len??��??��.
* buf:����?��??������??��
* len:����?����?��??����y(?a��?o����?�䨲??��??����??��??,?a��??������2?��a3?1y64??��??��)
**/
void USART1_Send_Data(u8 *buf,u16 len)
{
    u16 t;
   
//  RS485_TX_EN=1;            //����???a����?��?�꨺?
    for(t=0;t<len;t++)        //?-?������?����y?Y
    {
        while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //?-?������?��,?����?����?��������?
        USART_SendData(USART1,buf[t]);
    }
    while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);  
//    RS485_TX_EN=0;                //����???a?����??�꨺?
}

/**
* ?����????��3��?����?��?��?��?
* ������??����?1��?���䨮D??a21??��??����?��?��?��?
**/
void USART1_IRQHandler(void)                    //��??��1?D??��t??3��D��
{
	
    u8 Res;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
        {
            Res =USART_ReceiveData(USART1);    //?����??����?��?��?��y?Y
            if(USART1_RX_CNT<1)//??����?����????��3��?����?��?��?��?
            {
                USART1_RX_BUF[USART1_RX_CNT]=Res;        //?????����?��?��??��
                USART1_RX_CNT++;                                        //?����?��y?Y???��1
            }
     }
         //��?3?-��?1?���騦����?3?D����a?��?��SR,?��?��DR??��??��?��?��??3y2???��??D??��??����a
    if(USART_GetFlagStatus(USART1,USART_FLAG_ORE) == SET)
    {
        USART_ReceiveData(USART1);
        USART_ClearFlag(USART1,USART_FLAG_ORE);
    }
     USART_ClearFlag(USART1,USART_IT_RXNE); //��??����a??3y?����??D??
}
int main(void)
{
    Usart1_Init(9600);//��??��12����??������???a9600  
    while(1)
    {       
        if(USART1_RX_CNT==1)//��y?Y?����?����3��
        {
					 GPIO_ResetBits(GPIOB,GPIO_Pin_0);
            USART1_RX_CNT=0;//?????��??
            //???����?��?��?��y?Y����?��3?����
						USART1_RX_BUF[0]='6';
            USART1_Send_Data(USART1_RX_BUF,1);//����1y��??��1???����?��?��?1��?��3��?����?��?����?��3?����
        }
    }

}
