#include "stm32f10x.h"
#include "USART.h"
#include "TIM.h"
#include "LED.h"
#include "mpu6050.h"
#include "imu.h"
#include "ANO_DT.h"
#include "Remote.h"
#include "control.h"
#include "delay.h"

#undef SUCCESS
#define SUCCESS 0
#undef FAILED
#define FAILED 1

volatile uint32_t SysTick_count; // ϵͳʱ�����
_st_Mpu MPU6050;				 // MPU6050ԭʼ����
_st_Mag AK8975;
_st_AngE Angle;		   // ��ǰ�Ƕ���ֵ̬
_st_Remote Remote;	   // ң��ͨ��ֵ
_st_ALL_flag ALL_flag; // ϵͳ��־λ������������־λ��

PidObject pidRateX; // �ڻ�PID����
PidObject pidRateY;
PidObject pidRateZ;

PidObject pidPitch; // �⻷PID����
PidObject pidRoll;
PidObject pidYaw;

PidObject pidHeightRate;
PidObject pidHeightHigh;

void pid_param_Init(void); // PID���Ʋ�����ʼ������дPID�����ᱣ�����ݣ��������ɺ�ֱ���ڳ�������� ����¼���ɿ�

void TIM3_IRQHandler(void) // TIM3�ж� 3ms
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) // ���ָ����TIM�жϷ������:TIM �ж�Դ
	{
		SysTick_count++;
		
		MpuGetData();// ��ȡMPU6050����
		GetAngle(&MPU6050, &Angle, 0.00626f);// ��ȡ��̬�Ƕ�
		RC_Analy();// ����ң��������
		FlightPidControl(0.003f);// ���п���PID����
		MotorControl();// �������
		
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // ���TIMx���жϴ�����λ:TIM �ж�Դ
	}
}

int main(void)
{
	SystemInit();									// System init.
	delay_init();									// delay init.
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 2��bit����ռ���ȼ���2��bit�������ȼ�

	// ϵͳ��ʼ��
	IIC_Init();		  // I2C��ʼ��
	pid_param_Init(); // PID������ʼ��
	LEDInit();		  // LED���Ƴ�ʼ��
	MpuInit();		  // MPU6050��ʼ��
	USART1_Config(115200);
	USART3_Config(115200);
	TIM2_PWM_Config(); // 4·PWM��ʼ��
	TIM3_Config();	   // ϵͳ�������ڳ�ʼ��

	while (1)
	{
		// ANTO_polling(); // ������λ��
		PilotLED(); // LEDˢ��
	}
}

void pid_param_Init(void)
{
	pidRateX.kp = 2.0f;
	pidRateY.kp = 2.0f;
	pidRateZ.kp = 4.0f;

	pidRateX.ki = 0.0f;
	pidRateY.ki = 0.0f;
	pidRateZ.ki = 0.0f;

	pidRateX.kd = 0.28f;
	pidRateY.kd = 0.28f;
	pidRateZ.kd = 0.4f;
//	pidRateX.kd = 0.08f;
//	pidRateY.kd = 0.08f;
//	pidRateZ.kd = 0.5f;

	pidPitch.kp = 7.0f;
	pidRoll.kp = 7.0f;
	pidYaw.kp = 7.0f;
}
