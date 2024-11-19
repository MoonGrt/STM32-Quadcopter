#include "USART.h"
#include "stdio.h"

/*
 * ��������fputc
 * ����  ���ض���c�⺯��printf��USART1
 * ����  ����
 * ���  ����
 * ����  ����printf����
 */
int fputc(int ch, FILE *f)
{
	/* ��Printf���ݷ������� */
	USART_SendData(USART3, (unsigned char)ch);
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) != SET)
		;
	return (ch);
}

/*
 * ��������USART1_Config
 * ����  ��USART1 GPIO ����,����ģʽ���á�
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
void USART1_Config(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_Initstructure;
	/* ���ô���1 ��USART1�� ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA,ENABLE);
	
	/* Configure the NVIC Preemption Priority Bits */
	
	/* ʹ�ܴ���1�ж� */
	NVIC_Initstructure.NVIC_IRQChannel = USART1_IRQn;			// USART1  ����1ȫ���ж�
	NVIC_Initstructure.NVIC_IRQChannelPreemptionPriority = 0;	// ��ռ���ȼ�1
	NVIC_Initstructure.NVIC_IRQChannelSubPriority = 0;			// ���ȼ�1
	/*IRQͨ��ʹ��*/
	NVIC_Initstructure.NVIC_IRQChannelCmd = ENABLE;
	/*����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART1*/
	NVIC_Init(&NVIC_Initstructure);
	
	/*����GPIO�˿�����*/
	/* ���ô���1 USART1 Tx (PA.9)*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	/* ���ô���1 USART1 Rx (PA.10)*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;	
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	// USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1,&USART_InitStructure);	
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	USART_Cmd(USART1,ENABLE);
}
/*
 * ��������USART3_Config
 * ����  ��USART3 GPIO ����,����ģʽ���á�
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
void USART3_Config(u32 bound)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	/* ���ô���3 ��USART3�� ʱ��*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3 | RCC_APB2Periph_GPIOB, ENABLE);

	/* Configure the NVIC Preemption Priority Bits */

	/* ʹ�ܴ���3�ж� */
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;		  // USART3  ����3ȫ���ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // ��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		  // �����ȼ�1
	/*IRQͨ��ʹ��*/
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	/*����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���USART3*/
	NVIC_Init(&NVIC_InitStructure);

	/*����GPIO�˿�����*/
	/* ���ô���3 USART3 Tx (PB.10)*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	/* ���ô���3 USART3 Rx (PB.11)*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;										// ���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;						// �ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;							// һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;								// ����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;					// �շ�ģʽ
	USART_Init(USART3, &USART_InitStructure);

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // �����ж�
	USART_Cmd(USART3, ENABLE);					   // ʹ�ܴ���
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
