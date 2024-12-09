#include "stm32f10x.h" // Device header

/**
 * 函    数：Soft_SPI写SCK引脚电平
 * 参    数：BitValue 协议层传入的当前需要写入SCK的电平，范围0~1
 * 返 回 值：无
 * 注意事项：此函数需要用户实现内容，当BitValue为0时，需要置SCK为低电平，当BitValue为1时，需要置SCK为高电平
 */
void Soft_SPI_W_SCK(uint8_t BitValue)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_13, (BitAction)BitValue); // 根据BitValue，设置SCK引脚的电平
}

/**
 * 函    数：Soft_SPI写MOSI引脚电平
 * 参    数：BitValue 协议层传入的当前需要写入MOSI的电平，范围0~1
 * 返 回 值：无
 * 注意事项：此函数需要用户实现内容，当BitValue为0时，需要置MOSI为低电平，当BitValue为1时，需要置MOSI为高电平
 */
void Soft_SPI_W_MOSI(uint8_t BitValue)
{
    GPIO_WriteBit(GPIOB, GPIO_Pin_15, (BitAction)BitValue); // 根据BitValue，设置MOSI引脚的电平，BitValue要实现非0即1的特性
}

/**
 * 函    数：I2C读MISO引脚电平
 * 参    数：无
 * 返 回 值：协议层需要得到的当前MISO的电平，范围0~1
 * 注意事项：此函数需要用户实现内容，当前MISO为低电平时，返回0，当前MISO为高电平时，返回1
 */
uint8_t Soft_SPI_R_MISO(void)
{
    return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14); // 读取MISO电平并返回
}

/**
 * 函    数：Soft_SPI初始化
 * 参    数：无
 * 返 回 值：无
 * 注意事项：此函数需要用户实现内容，实现SS、SCK、MOSI和MISO引脚的初始化
 */
void Soft_SPI_Init(void)
{
    /*开启时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE); // 开启GPIOA和GPIOB的时钟

    /*GPIO初始化*/
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); // 将PA4、PA5和PA7引脚初始化为推挽输出

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure); // 将PA4、PA5和PA7引脚初始化为推挽输出

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure); // 将PA6引脚初始化为上拉输入

    /*设置默认电平*/
    Soft_SPI_W_SCK(0); // SCK默认低电平
}

/**
 * 函    数：Soft_SPI交换传输一个字节，使用Soft_SPI模式0
 * 参    数：ByteSend 要发送的一个字节
 * 返 回 值：接收的一个字节
 */
uint8_t Soft_SPI_SwapByte(uint8_t ByteSend)
{
    uint8_t i, ByteReceive = 0x00; // 定义接收的数据，并赋初值0x00，此处必须赋初值0x00，后面会用到

    for (i = 0; i < 8; i++) // 循环8次，依次交换每一位数据
    {
        /*两个!可以对数据进行两次逻辑取反，作用是把非0值统一转换为1，即：!!(0) = 0，!!(非0) = 1*/
        Soft_SPI_W_MOSI(!!(ByteSend & (0x80 >> i))); // 使用掩码的方式取出ByteSend的指定一位数据并写入到MOSI线
        Soft_SPI_W_SCK(1);                           // 拉高SCK，上升沿移出数据
        if (Soft_SPI_R_MISO())
        {
            ByteReceive |= (0x80 >> i);
        } // 读取MISO数据，并存储到Byte变量
          // 当MISO为1时，置变量指定位为1，当MISO为0时，不做处理，指定位为默认的初值0
        Soft_SPI_W_SCK(0); // 拉低SCK，下降沿移入数据
    }

    return ByteReceive; // 返回接收到的一个字节数据
}