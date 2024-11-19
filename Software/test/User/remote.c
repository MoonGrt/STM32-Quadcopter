#include "ALL_DATA.h"
#include "control.h"
#include <math.h>
#include "myMath.h"
#include "LED.h"
#include "Remote.h"
#include "stdio.h"

#define SUCCESS 0
#undef FAILED
#define FAILED 1
#define BUF_SIZE 9
uint8_t Serial_RxPacket[BUF_SIZE];
uint8_t Serial_RxFlag;

/*****************************************************************************************
 *  ͨ�����ݴ���
 * @param[in]
 * @param[out]
 * @return
 ******************************************************************************************/

void remote_unlock(void);

void USART1_IRQHandler(void)
{
	static uint8_t RxState = 0;
	static uint8_t pRxPacket = 0;
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		uint8_t RxData = USART_ReceiveData(USART1);

		if (RxState == 0 && Serial_RxFlag == 0)
		{
			if (RxData == 0xA5)
			{
				RxState = 1;
				pRxPacket = 0;
			}
		}
		else if (RxState == 1)
		{
			Serial_RxPacket[pRxPacket] = RxData;
			pRxPacket++;
			if (pRxPacket >= 8)
				RxState = 2;
		}
		else if (RxState == 2)
		{
			if (RxData == 0x5A)
			{
				RxState = 0;
				Serial_RxFlag = 1;
			}
		}
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

void RC_Analy(void)
{
	static uint16_t cnt;
	/*             Receive  and check RC data           */
	if (Serial_RxFlag)
	{
		cnt = 0;
		Remote.yaw = ((uint16_t)Serial_RxPacket[5] << 8) | Serial_RxPacket[4];   // ͨ��4
		Remote.roll = ((uint16_t)Serial_RxPacket[7] << 8) | Serial_RxPacket[6];  // ͨ��1
		Remote.pitch = ((uint16_t)Serial_RxPacket[3] << 8) | Serial_RxPacket[2]; // ͨ��2
		Remote.thr = ((uint16_t)Serial_RxPacket[1] << 8) | Serial_RxPacket[0];   // ͨ��3
		printf("t:%d y:%d p:%d r:%d\n",Remote.thr,Remote.yaw,Remote.pitch,Remote.roll);
		
		Remote.roll -= 150;  // ͨ��1   //ǰ��   
		Remote.pitch -= 100; // ͨ��2   //���� -80
		Remote.yaw = 1500;   // ͨ��4	
		
//		if(Remote.thr > 1800)
//			Remote.thr = 1800;   // ͨ��3   ����״̬1730����   ����1700С��  1719
		
		Serial_RxFlag = 0;
		{
			const float roll_pitch_ratio = 0.04f;

			pidPitch.desired = -(Remote.pitch - 1500) * roll_pitch_ratio; // ��ң��ֵ��Ϊ���нǶȵ�����ֵ
			pidPitch.desired = (Remote.pitch - 1500) * roll_pitch_ratio; // ��ң��ֵ��Ϊ���нǶȵ�����ֵ
			pidRoll.desired = -(Remote.roll - 1500) * roll_pitch_ratio;
			if (Remote.yaw > 1820)
				pidYaw.desired += 0.75f;
			else if (Remote.yaw < 1180)
				pidYaw.desired -= 0.75f;
		}
		remote_unlock();
	}
	// ���3��û�յ�ң�����ݣ����ж�ң���źŶ�ʧ���ɿ����κ�ʱ��ֹͣ���У��������ˡ�
	// ���������ʹ���߿ɽ����ر�ң�ص�Դ������������3��������رգ��������ˡ�
	// �����ر�ң�أ�����ڷ����л�ֱ�ӵ��䣬���ܻ��𻵷�������
	else
	{
		cnt++;
		if (cnt > 800)
		{
			cnt = 0;
			ALL_flag.unlock = 0;
		}
	}
}

/*****************************************************************************************
 *  �����ж�
 * @param[in]
 * @param[out]
 * @return
 ******************************************************************************************/
void remote_unlock(void)
{
	//static uint16_t cnt = 0;
	volatile static uint8_t status = WAITING_1;
	
	switch (status)
	{
	case WAITING_1:
		if (Remote.thr < 1050)
		{
			static uint8_t cnt = 0;
			cnt++;
			if (cnt > 25) // ��������豣��С��ʱ�����ϣ���ֹң�ؿ�����ʼ��δ��ɵĴ�������
			{
				cnt = 0;
				status = WAITING_4;
			}
		}
		break;
	case WAITING_4: // ����ǰ׼��
		ALL_flag.unlock = 1;
		status = PROCESS_31;
		LED.status = AlwaysOn;
		break;
	case PROCESS_31: // �������״̬
		if (Remote.thr < 1045)
			if (!ALL_flag.unlock) // Other conditions lock
				status = EXIT_255;
		break;
	case EXIT_255:					// ��������
		LED.status = AllFlashLight; // exit
		//cnt = 0;
		LED.FlashTime = 300; // 300*3ms
		ALL_flag.unlock = 0;
		status = WAITING_1;
		break;
	default:
		status = EXIT_255;
		break;
	}
}
/***********************END OF FILE*************************************/
