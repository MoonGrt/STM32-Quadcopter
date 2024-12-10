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
#include "W25Q64.h"
#include "SPI.h"
#include "NRF24L01.h"
#include "SPL06.h"
#include "I2C.h"

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
    pid_param_Init(); // PID������ʼ��
    LEDInit();        // LED���Ƴ�ʼ��

    // USART ��ʼ��
    USART1_Config(115200);
    USART3_Config(115200);

    // I2C ��ʼ��
    IIC_Init();   // I2C��ʼ��
    MpuInit();    // MPU6050��ʼ
    spl06_init(); // SPL06��ʼ��

    // SPI ��ʼ��
    SPI_INIT();      // SPI��ʼ��
    NRF24L01_init(); // 2.4Gң��ͨ�ų�ʼ��
    W25Q64_Init();   // W25Q64��ʼ��

    // TIM ��ʼ��
    // TIM2_PWM_Config(); // 4·PWM��ʼ��
    // TIM3_Config(); // ϵͳ�������ڳ�ʼ��
}

void MPU6050_Test(void);
void SPL06_Test(void);
void NRF24L01_Test(void);
void W25Q64_Test(void);
// void I2C_Test(void);

int main(void)
{
    // ϵͳ��ʼ��
    Init();

    MPU6050_Test();  // MPU6050����
    SPL06_Test();    // SPL06����
    NRF24L01_Test(); // 2.4Gң��ͨ�Ų���
    W25Q64_Test();   // W25Q64����
    I2C_Test();      // I2C����

    while (1)
    {
        // ANTO_polling(); // ������λ��
        // PilotLED(); // LEDˢ��
    }
}

void MPU6050_Test(void)
{
    MpuGetData();                         // ��ȡMPU6050����
    GetAngle(&MPU6050, &Angle, 0.00626f); // ��ȡ��̬�Ƕ�

    pidRateX.measured = MPU6050.gyroX * Gyro_G; // �ڻ�����ֵ �Ƕ�/��
    pidRateY.measured = MPU6050.gyroY * Gyro_G;
    pidRateZ.measured = MPU6050.gyroZ * Gyro_G;
    pidPitch.measured = Angle.pitch; // �⻷����ֵ ��λ���Ƕ�
    pidRoll.measured = Angle.roll;
    pidYaw.measured = Angle.yaw;
    printf("Pitch: %.2f, Roll: %.2f, Yaw: %.2f\n", pidYaw.desired, pidYaw.measured, pidRateZ.out);
}

void SPL06_Test(void)
{
    spl06_result_t spl06_result;
    spl06_get_result(&spl06_result);
    // printf("Praw: %d, Traw: %d\n", spl06_result.Praw, spl06_result.Traw);
    printf("Pcomp: %.2f, Tcomp: %.2f\n", spl06_result.Pcomp, spl06_result.Tcomp);
}

void NRF24L01_Test(void)
{
}

uint8_t MID;                                     // �������ڴ��MID�ŵı���
uint16_t DID;                                    // �������ڴ��DID�ŵı���
uint8_t ArrayWrite[] = {0x01, 0x02, 0x03, 0x04}; // ����Ҫд�����ݵĲ�������
uint8_t ArrayRead[4];                            // ����Ҫ��ȡ���ݵĲ�������
void W25Q64_Test(void)
{
    /*��ӡID��*/
    W25Q64_ReadID(&MID, &DID); // ��ȡW25Q64��ID��
    printf("MID: %02X\n", MID);
    printf("DID: %04X\n", DID);

    /*W25Q64���ܺ�������*/
    W25Q64_SectorErase(0x000000);                // ��������
    W25Q64_PageProgram(0x000000, ArrayWrite, 4); // ��д�����ݵĲ�������д�뵽W25Q64��
    W25Q64_ReadData(0x000000, ArrayRead, 4);     // ��ȡ��д��Ĳ������ݵ���ȡ���ݵĲ���������

    /*��ӡ����*/
    printf("W: %02X %02X %02X %02X\n", ArrayWrite[0], ArrayWrite[1], ArrayWrite[2], ArrayWrite[3]);
    printf("R: %02X %02X %02X %02X\n", ArrayRead[0], ArrayRead[1], ArrayRead[2], ArrayRead[3]);
}
