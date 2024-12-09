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
_st_Mpu MPU6050;                 // MPU6050ԭʼ����
_st_Mag AK8975;
_st_AngE Angle;        // ��ǰ�Ƕ���ֵ̬
_st_Remote Remote;     // ң��ͨ��ֵ
_st_ALL_flag ALL_flag; // ϵͳ��־λ������������־λ��

PidObject pidRateX; // �ڻ�PID����
PidObject pidRateY;
PidObject pidRateZ;

PidObject pidPitch; // �⻷PID����
PidObject pidRoll;
PidObject pidYaw;

PidObject pidHeightRate;
PidObject pidHeightHigh;

void pid_param_Init(void) // PID���Ʋ�����ʼ������дPID�����ᱣ�����ݣ��������ɺ�ֱ���ڳ�������� ����¼���ɿ�
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

    pidPitch.kp = 7.0f;
    pidRoll.kp = 7.0f;
    pidYaw.kp = 7.0f;
}

void TIM3_IRQHandler(void) // TIM3�ж� 3ms
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) // ���ָ����TIM�жϷ������:TIM �ж�Դ
    {
        SysTick_count++;

        MpuGetData();                         // ��ȡMPU6050����
        GetAngle(&MPU6050, &Angle, 0.00626f); // ��ȡ��̬�Ƕ�
        RC_Analy();                           // ����ң��������
        FlightPidControl(0.003f);             // ���п���PID����
        MotorControl();                       // �������

        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // ���TIMx���жϴ�����λ:TIM �ж�Դ
    }
}

void Init(void)
{
    SystemInit();                                   // System init.
    delay_init();                                   // delay init.
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 2��bit����ռ���ȼ���2��bit�������ȼ�

    // ϵͳ��ʼ��
    IIC_Init();       // I2C��ʼ��
    pid_param_Init(); // PID������ʼ��

    USART1_Config(115200);
    USART3_Config(115200);

    // LEDInit();		  // LED���Ƴ�ʼ��
    MpuInit(); // MPU6050��ʼ

    // TIM2_PWM_Config(); // 4·PWM��ʼ��
    // TIM3_Config(); // ϵͳ�������ڳ�ʼ��
}

int main(void)
{
    // ϵͳ��ʼ��
    Init();

    while (1)
    {
        // ANTO_polling(); // ������λ��
        // PilotLED(); // LEDˢ��

        MpuGetData();                         // ��ȡMPU6050����
        GetAngle(&MPU6050, &Angle, 0.00626f); // ��ȡ��̬�Ƕ�

        // float dt = 0.003f; // ϵͳ���� 3ms
        // pidRateX.measured = MPU6050.gyroX * Gyro_G; // �ڻ�����ֵ �Ƕ�/��
        // pidRateY.measured = MPU6050.gyroY * Gyro_G;
        // pidRateZ.measured = MPU6050.gyroZ * Gyro_G;
        // pidPitch.measured = Angle.pitch; // �⻷����ֵ ��λ���Ƕ�
        // pidRoll.measured = Angle.roll;
        // pidYaw.measured = Angle.yaw;
        // pidUpdate(&pidRoll, dt);        // ����PID�������������⻷	�����PID
        // pidRateX.desired = pidRoll.out; // ���⻷��PID�����Ϊ�ڻ�PID������ֵ��Ϊ����PID
        // pidUpdate(&pidRateX, dt);       // �ٵ����ڻ�
        // pidUpdate(&pidPitch, dt); // ����PID�������������⻷	������PID
        // pidRateY.desired = pidPitch.out;
        // pidUpdate(&pidRateY, dt); // �ٵ����ڻ�
        // pidUpdate(&pidYaw, dt);
        // pidRateZ.desired = pidYaw.out;
        // pidUpdate(&pidRateZ, dt);
        // printf("%f %f %f\n",pidYaw.desired,pidYaw.measured,pidRateZ.out);

        delay_ms(500);
    }
}
