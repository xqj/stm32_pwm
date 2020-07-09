#include "stm32f10x.h"



int main()
{
    GPIO_InitTypeDef GPIO_LED_InitStructure;
    GPIO_InitTypeDef GPIO_USART_TX_InitStructure;
    GPIO_InitTypeDef GPIO_USART_RX_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    unsigned short int temp;

    // ÅäÖÃÊ±ÖÓ
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

    // ÅäÖÃUSART_TX
    GPIO_USART_TX_InitStructure.GPIO_Pin =  GPIO_Pin_9;
    GPIO_USART_TX_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_USART_TX_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOA, &GPIO_USART_TX_InitStructure);

    // ÅäÖÃUSART_RX
    GPIO_USART_RX_InitStructure.GPIO_Pin =  GPIO_Pin_10;
    GPIO_USART_RX_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;

    GPIO_Init(GPIOA, &GPIO_USART_RX_InitStructure);

    // ÅäÖÃ´®¿Ú
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    USART_Init(USART1, &USART_InitStructure);

    USART_Cmd(USART1, ENABLE);

    //ÅäÖÃµÆ
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_LED_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_LED_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_LED_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

    GPIO_Init(GPIOB, &GPIO_LED_InitStructure);

    //¹Ø±ÕµÆ
    GPIO_SetBits(GPIOB, GPIO_Pin_0);

    while (1)
    {
        if(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) != RESET)
        {
            temp = USART_ReceiveData(USART1);
           // USART_SendData(USART1,temp);
            GPIOB->ODR ^=GPIO_Pin_0;
        }

    }
}
