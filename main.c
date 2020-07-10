#include "stm32f10x.h"
#include "stdio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x.h"
#include "stm32f10x_iwdg.h"

char USART1_RX_BUF[8]={'S','1','F','0','6','.','0','T'};
u8 USART1_RX_CNT=0;

void Usart1_Init(u32 bound)
{
    //GPIO???úéè??
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//使能GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟

    //USART1_TX   GPIOA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    //?′ó?í?íìê?3?
    GPIO_Init(GPIOA, &GPIO_InitStructure);//3?ê??ˉGPIOA.9

    //USART1_RX      GPIOA.103?ê??ˉ
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA10
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//????ê?è?
    GPIO_Init(GPIOA, &GPIO_InitStructure);//3?ê??ˉGPIOA.10
	
	 RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,ENABLE);//复位串口2
   RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART2,DISABLE);//停止复位

    //Usart1 NVIC ????
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);    //éè??NVIC?D??·?×é2:2???à??ó??è??￡?2???ìó|ó??è??   0-3;

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//?à??ó??è??3
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //×óó??è??3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;            //IRQí¨μàê1?ü
    NVIC_Init(&NVIC_InitStructure);    //?ù?Y???¨μ?2?êy3?ê??ˉVIC??′??÷

   //USART 3?ê??ˉéè??

    USART_InitStructure.USART_BaudRate = bound;//′??ú2¨ì??ê
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//×?3¤?a8??êy?Y??ê?
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//ò???í￡?1??
    USART_InitStructure.USART_Parity = USART_Parity_No;//?T????D￡?é??
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//?Tó2?têy?Yá÷????
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    //ê?·￠?￡ê?

    USART_Init(USART2, &USART_InitStructure); //3?ê??ˉ′??ú1
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//?a??′??ú?óêü?D??
    USART_Cmd(USART2, ENABLE);                    //ê1?ü′??ú1
}
/**
* USART1·￠?ílen??×??ú.
* buf:·￠?í??ê×μ??·
* len:·￠?íμ?×??úêy(?aá?oí±?′ú??μ??óê??￥??,?aà??¨òé2?òa3?1y64??×??ú)
**/
void USART2_Send_Data(char *buf,u16 len)
{
    u16 t;
   
//  RS485_TX_EN=1;            //éè???a·￠?í?￡ê?
    for(t=0;t<len;t++)        //?-?··￠?íêy?Y
    {
        while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET); //?-?··￠?í,?±μ?·￠?ííê±?
        USART_SendData(USART2,buf[t]);
    }
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);  
//    RS485_TX_EN=0;                //éè???a?óê??￡ê?
}

/**
* ?óê????¨3¤?èμ?×?·?′?
* ±èè??óê?1ì?¨′óD??a21??×??úμ?×?·?′?
**/
void USART2_IRQHandler(void)                    //′??ú1?D??·t??3ìDò
{
	
    u8 Res;
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
        {
            Res =USART_ReceiveData(USART2); 
     }
         //ò?3?-è?1?·￠éúò?3?Dèòa?è?áSR,?ù?áDR??′??÷?ò?é??3y2???è??D??μ??êìa
    if(USART_GetFlagStatus(USART2,USART_FLAG_ORE) == SET) 
    {
        USART_ReceiveData(USART2);
        USART_ClearFlag(USART2,USART_FLAG_ORE);
    }
     USART_ClearFlag(USART2,USART_IT_RXNE); //ò??¨òa??3y?óê??D??
}
int main(void)
{
    Usart2_Init(9600);//′??ú12¨ì??êéè???a9600  
	 
    while(1)
    {       
       
    }

}
