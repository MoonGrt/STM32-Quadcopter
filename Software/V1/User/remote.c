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
 *  通道数据处理
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
		Remote.yaw = ((uint16_t)Serial_RxPacket[5] << 8) | Serial_RxPacket[4];   // 通道4
		Remote.roll = ((uint16_t)Serial_RxPacket[7] << 8) | Serial_RxPacket[6];  // 通道1
		Remote.pitch = ((uint16_t)Serial_RxPacket[3] << 8) | Serial_RxPacket[2]; // 通道2
		Remote.thr = ((uint16_t)Serial_RxPacket[1] << 8) | Serial_RxPacket[0];   // 通道3
		printf("t:%d y:%d p:%d r:%d\n",Remote.thr,Remote.yaw,Remote.pitch,Remote.roll);
		
		Remote.roll -= 150;  // 通道1   //前后   
		Remote.pitch -= 100; // 通道2   //左右 -80
		Remote.yaw = 1500;   // 通道4	
		
//		if(Remote.thr > 1800)
//			Remote.thr = 1800;   // 通道3   满电状态1730大了   满电1700小了  1719
		
		Serial_RxFlag = 0;
		{
			const float roll_pitch_ratio = 0.04f;

			pidPitch.desired = -(Remote.pitch - 1500) * roll_pitch_ratio; // 将遥杆值作为飞行角度的期望值
			pidPitch.desired = (Remote.pitch - 1500) * roll_pitch_ratio; // 将遥杆值作为飞行角度的期望值
			pidRoll.desired = -(Remote.roll - 1500) * roll_pitch_ratio;
			if (Remote.yaw > 1820)
				pidYaw.desired += 0.75f;
			else if (Remote.yaw < 1180)
				pidYaw.desired -= 0.75f;
		}
		remote_unlock();
	}
	// 如果3秒没收到遥控数据，则判断遥控信号丢失，飞控在任何时候停止飞行，避免伤人。
	// 意外情况，使用者可紧急关闭遥控电源，飞行器会在3秒后立即关闭，避免伤人。
	// 立即关闭遥控，如果在飞行中会直接掉落，可能会损坏飞行器。
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
 *  解锁判断
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
			if (cnt > 25) // 最低油门需保持小段时间以上，防止遥控开机初始化未完成的错误数据
			{
				cnt = 0;
				status = WAITING_4;
			}
		}
		break;
	case WAITING_4: // 解锁前准备
		ALL_flag.unlock = 1;
		status = PROCESS_31;
		LED.status = AlwaysOn;
		break;
	case PROCESS_31: // 进入解锁状态
		if (Remote.thr < 1045)
			if (!ALL_flag.unlock) // Other conditions lock
				status = EXIT_255;
		break;
	case EXIT_255:					// 进入锁定
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
