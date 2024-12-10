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

volatile uint32_t SysTick_count; // 系统时间计数
_st_Mpu MPU6050;                 // MPU6050原始数据
_st_Mag AK8975;
_st_AngE Angle;        // 当前角度姿态值
_st_Remote Remote;     // 遥控通道值
_st_ALL_flag ALL_flag; // 系统标志位，包含解锁标志位等

PidObject pidRateX; // 内环PID数据
PidObject pidRateY;
PidObject pidRateZ;

PidObject pidPitch; // 外环PID数据
PidObject pidRoll;
PidObject pidYaw;

PidObject pidHeightRate;
PidObject pidHeightHigh;

void pid_param_Init(void) // PID控制参数初始化，改写PID并不会保存数据，请调试完成后直接在程序里更改 再烧录到飞控
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

void TIM3_IRQHandler(void) // TIM3中断 3ms
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) // 检查指定的TIM中断发生与否:TIM 中断源
    {
        SysTick_count++;

        MpuGetData();                         // 获取MPU6050数据
        GetAngle(&MPU6050, &Angle, 0.00626f); // 获取姿态角度
        RC_Analy();                           // 解析遥控器数据
        FlightPidControl(0.003f);             // 飞行控制PID计算
        MotorControl();                       // 电机控制

        TIM_ClearITPendingBit(TIM3, TIM_IT_Update); // 清除TIMx的中断待处理位:TIM 中断源
    }
}

void Init(void)
{
    SystemInit();                                   // System init.
    delay_init();                                   // delay init.
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); // 2个bit的抢占优先级，2个bit的子优先级

    // 系统初始化
    pid_param_Init(); // PID参数初始化
    LEDInit();        // LED闪灯初始化

    // USART 初始化
    USART1_Config(115200);
    USART3_Config(115200);

    // I2C 初始化
    IIC_Init();   // I2C初始化
    MpuInit();    // MPU6050初始
    spl06_init(); // SPL06初始化

    // SPI 初始化
    SPI_INIT();      // SPI初始化
    NRF24L01_init(); // 2.4G遥控通信初始化
    W25Q64_Init();   // W25Q64初始化

    // TIM 初始化
    // TIM2_PWM_Config(); // 4路PWM初始化
    // TIM3_Config(); // 系统工作周期初始化
}

void MPU6050_Test(void);
void SPL06_Test(void);
void NRF24L01_Test(void);
void W25Q64_Test(void);
// void I2C_Test(void);

int main(void)
{
    // 系统初始化
    Init();

    MPU6050_Test();  // MPU6050测试
    SPL06_Test();    // SPL06测试
    NRF24L01_Test(); // 2.4G遥控通信测试
    W25Q64_Test();   // W25Q64测试
    I2C_Test();      // I2C测试

    while (1)
    {
        // ANTO_polling(); // 匿名上位机
        // PilotLED(); // LED刷新
    }
}

void MPU6050_Test(void)
{
    MpuGetData();                         // 获取MPU6050数据
    GetAngle(&MPU6050, &Angle, 0.00626f); // 获取姿态角度

    pidRateX.measured = MPU6050.gyroX * Gyro_G; // 内环测量值 角度/秒
    pidRateY.measured = MPU6050.gyroY * Gyro_G;
    pidRateZ.measured = MPU6050.gyroZ * Gyro_G;
    pidPitch.measured = Angle.pitch; // 外环测量值 单位：角度
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

uint8_t MID;                                     // 定义用于存放MID号的变量
uint16_t DID;                                    // 定义用于存放DID号的变量
uint8_t ArrayWrite[] = {0x01, 0x02, 0x03, 0x04}; // 定义要写入数据的测试数组
uint8_t ArrayRead[4];                            // 定义要读取数据的测试数组
void W25Q64_Test(void)
{
    /*打印ID号*/
    W25Q64_ReadID(&MID, &DID); // 获取W25Q64的ID号
    printf("MID: %02X\n", MID);
    printf("DID: %04X\n", DID);

    /*W25Q64功能函数测试*/
    W25Q64_SectorErase(0x000000);                // 扇区擦除
    W25Q64_PageProgram(0x000000, ArrayWrite, 4); // 将写入数据的测试数组写入到W25Q64中
    W25Q64_ReadData(0x000000, ArrayRead, 4);     // 读取刚写入的测试数据到读取数据的测试数组中

    /*打印数据*/
    printf("W: %02X %02X %02X %02X\n", ArrayWrite[0], ArrayWrite[1], ArrayWrite[2], ArrayWrite[3]);
    printf("R: %02X %02X %02X %02X\n", ArrayRead[0], ArrayRead[1], ArrayRead[2], ArrayRead[3]);
}
