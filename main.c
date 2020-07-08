#include "stm32f10x.h"
#include "stdio.h"

#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

void RCC_Configuration(void);
void GPIO_Configuration(void);
void USART_Configuration(void);
void delay_ms(u16 time);
void UART_PutChar(USART_TypeDef* USARTx, uint8_t Data);
void UART_PutStr (USART_TypeDef* USARTx, uint8_t *str);
int Putchar(int c);

void RCC_Configuration(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO|RCC_APB2Periph_GPIOB,ENABLE);
}

void GPIO_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    //TX--À¶ÑÀRXT
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    //RX--À¶ÑÀTX
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

}
void USART_Configuration(void)
{

    USART_InitTypeDef USART_InitStructure;

    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1,&USART_InitStructure);
    USART_Cmd(USART1,ENABLE);
}
void delay_ms(u16 time)
{
    u16 i=0;
    while(time--)
    {
        i=12000;
        while(i--);
    }
}
char  Usart1_Data_Buffer;


void USART1_IRQHandler(void)
{
    if(USART_GetITStatus(USART1,USART_IT_RXNE) != RESET)
    {
        Usart1_Data_Buffer = USART_ReceiveData(USART1);
    }
    USART_ClearITPendingBit(USART1,USART_IT_RXNE);
}

int main()
{
    SystemInit();
    RCC_Configuration();
    GPIO_Configuration();
    USART_Configuration();
    GPIO_ResetBits(GPIOB,GPIO_Pin_5);
    while(1)
    {
        if(Usart1_Data_Buffer==1) {
            GPIO_SetBits(GPIOB,GPIO_Pin_5);
        }
        if(Usart1_Data_Buffer==2) {
            GPIO_ResetBits(GPIOB,GPIO_Pin_5);
        }
    }

}

