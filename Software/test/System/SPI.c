#include "stm32f10x.h" // Device header
#include "SPI.h"

#ifdef SOFT_SPI

#define Set_SPI_SCK (GPIOB->BSRR = GPIO_Pin_13)  // PB13
#define Clr_SPI_SCK (GPIOB->BRR = GPIO_Pin_13)   // PB13
#define Set_SPI_MOSI (GPIOB->BSRR = GPIO_Pin_15) // PB15
#define Clr_SPI_MOSI (GPIOB->BRR = GPIO_Pin_15)  // PB15
#define Read_SPI_MISO (GPIOB->IDR & GPIO_Pin_14) // PB14

/**
 * 函    数：Soft_SPI初始化
 * 参    数：无
 * 返 回 值：无
 * 注意事项：此函数需要用户实现内容，实现SS、SCK、MOSI和MISO引脚的初始化
 */
void SPI_INIT(void)
{
    /*开启时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE); // 开启GPIOA和GPIOB的时钟

    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure); // 将PA4、PA5和PA7引脚初始化为推挽输出

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure); // 将PA6引脚初始化为上拉输入

    /*设置默认电平*/
    Clr_SPI_SCK; // SCK默认低电平
}

/**
 * 函    数：Soft_SPI交换传输一个字节，使用Soft_SPI模式0
 * 参    数：ByteSend 要发送的一个字节
 * 返 回 值：接收的一个字节
 */
uint8_t SPI_SwapByte(uint8_t ByteSend)
{
    uint8_t i, ByteReceive = 0x00; // 定义接收的数据，并赋初值0x00，此处必须赋初值0x00，后面会用到

    for (i = 0; i < 8; i++) // 循环8次，依次交换每一位数据
    {
        /*两个!可以对数据进行两次逻辑取反，作用是把非0值统一转换为1，即：!!(0) = 0，!!(非0) = 1*/
        if (!!(ByteSend & (0x80 >> i)))
            Set_SPI_MOSI; // 写入数据到MOSI线
        else
            Clr_SPI_MOSI; // 写入数据到MOSI线
        Set_SPI_SCK;      // 拉高SCK，上升沿移出数据
        if (Read_SPI_MISO)
        {
            ByteReceive |= (0x80 >> i);
        } // 读取MISO数据，并存储到Byte变量
          // 当MISO为1时，置变量指定位为1，当MISO为0时，不做处理，指定位为默认的初值0
        Clr_SPI_SCK; // 拉低SCK，下降沿移入数据
    }

    return ByteReceive; // 返回接收到的一个字节数据
}

#else

/**
 * 函    数：SPI初始化
 * 参    数：无
 * 返 回 值：无
 */
void SPI_INIT(void)
{
    /*开启时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE); // 开启GPIOA、GPIOB和AFIO的时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);                         // 开启SPI2的时钟

    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure); // 将PA6引脚初始化为上拉输入

    /*SPI初始化*/
    SPI_Cmd(SPI2, DISABLE);                                              // 必须先禁能,才能改变MODE
    SPI_InitTypeDef SPI_InitStructure;                                   // 定义结构体变量
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                        // 模式，选择为SPI主模式
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;   // 方向，选择2线全双工
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                    // 数据宽度，选择为8位
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                   // 先行位，选择高位先行
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_128; // 波特率分频，选择128分频
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                           // SPI极性，选择低极性
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                         // SPI相位，选择第一个时钟边沿采样，极性和相位决定选择SPI模式0
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                            // NSS，选择由软件控制
    SPI_InitStructure.SPI_CRCPolynomial = 7;                             // CRC多项式，暂时用不到，给默认值7
    SPI_Init(SPI2, &SPI_InitStructure);                                  // 将结构体变量交给SPI_Init，配置SPI2

    /*SPI使能*/
    SPI_Cmd(SPI2, ENABLE); // 使能SPI2，开始运行
}

/**
 * 函    数：SPI交换传输一个字节，使用SPI模式0
 * 参    数：ByteSend 要发送的一个字节
 * 返 回 值：接收的一个字节
 */
uint8_t SPI_SwapByte(uint8_t ByteSend)
{
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) != SET)
        ; // 等待发送数据寄存器空

    SPI_I2S_SendData(SPI2, ByteSend); // 写入数据到发送数据寄存器，开始产生时序

    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) != SET)
        ; // 等待接收数据寄存器非空

    return SPI_I2S_ReceiveData(SPI2); // 读取接收到的数据并返回
}

#endif
