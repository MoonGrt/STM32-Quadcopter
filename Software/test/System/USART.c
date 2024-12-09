#include "USART.h"
#include "stdio.h"

/*
 * 函数名：fputc
 * 描述  ：重定向c库函数printf到USART1
 * 输入  ：无
 * 输出  ：无
 * 调用  ：由printf调用
 */
int fputc(int ch, FILE *f)
{
    USART_ClearFlag(USART3, USART_FLAG_TC); // 解决第一个字节丢失问题
    /* 将Printf内容发往串口 */
    USART_SendData(USART3, (unsigned char)ch);
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) != SET)
        ;
    return (ch);
}

/*
 * 函数名：USART1_Config
 * 描述  ：USART1 GPIO 配置,工作模式配置。
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void USART1_Config(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_Initstructure;
    /* 配置串口1 （USART1） 时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    /* Configure the NVIC Preemption Priority Bits */

    /* 使能串口1中断 */
    NVIC_Initstructure.NVIC_IRQChannel = USART1_IRQn;         // USART1  串口1全局中断
    NVIC_Initstructure.NVIC_IRQChannelPreemptionPriority = 0; // 抢占优先级1
    NVIC_Initstructure.NVIC_IRQChannelSubPriority = 0;        // 优先级1
    /*IRQ通道使能*/
    NVIC_Initstructure.NVIC_IRQChannelCmd = ENABLE;
    /*根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART1*/
    NVIC_Init(&NVIC_Initstructure);

    /*串口GPIO端口配置*/
    /* 配置串口1 USART1 Tx (PA.9)*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* 配置串口1 USART1 Rx (PA.10)*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // USART 初始化设置
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART1, ENABLE);
}
/*
 * 函数名：USART3_Config
 * 描述  ：USART3 GPIO 配置,工作模式配置。
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
void USART3_Config(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    /* 配置串口3 （USART3） 时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 | RCC_APB2Periph_GPIOB, ENABLE);

    /* Configure the NVIC Preemption Priority Bits */

    /* 使能串口3中断 */
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;         // USART3  串口3全局中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级1
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // 子优先级1
    /*IRQ通道使能*/
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    /*根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器USART3*/
    NVIC_Init(&NVIC_InitStructure);

    /*串口GPIO端口配置*/
    /* 配置串口3 USART3 Tx (PB.10)*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* 配置串口3 USART3 Rx (PB.11)*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // USART 初始化设置
    USART_InitStructure.USART_BaudRate = bound;                                     // 串口波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // 字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // 一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;                             // 无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // 收发模式
    USART_Init(USART3, &USART_InitStructure);

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // 开启中断
    USART_Cmd(USART3, ENABLE);                     // 使能串口
}

void USART1_SendByte(const int8_t *Data, uint8_t len)
{
    uint8_t i;
    for (i = 0; i < len; i++)
    {
        while (!(USART1->SR & USART_FLAG_TXE))
            ; // while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        USART_SendData(USART1, *(Data + i));
    }
}

void USART3_SendByte(const int8_t *Data, uint8_t len)
{
    uint8_t i;
    for (i = 0; i < len; i++)
    {
        while (!(USART3->SR & USART_FLAG_TXE))
            ; // while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
        USART_SendData(USART3, *(Data + i));
    }
}
