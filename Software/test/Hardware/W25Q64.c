#include "stm32f10x.h" // Device header
#include "W25Q64.h"

// #define Soft_SPI // 软件SPI

#ifdef Soft_SPI
#include "Soft_SPI.h"

/**
 * 函    数：Soft_SPI写SS引脚电平
 * 参    数：BitValue 协议层传入的当前需要写入SS的电平，范围0~1
 * 返 回 值：无
 * 注意事项：此函数需要用户实现内容，当BitValue为0时，需要置SS为低电平，当BitValue为1时，需要置SS为高电平
 */
void Soft_SPI_W_SS(uint8_t BitValue)
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_8, (BitAction)BitValue); // 根据BitValue，设置SS引脚的电平
}

/**
 * 函    数：Soft_SPI起始
 * 参    数：无
 * 返 回 值：无
 */
void Soft_SPI_Start(void)
{
    Soft_SPI_W_SS(0); // 拉低SS，开始时序
}

/**
 * 函    数：Soft_SPI终止
 * 参    数：无
 * 返 回 值：无
 */
void Soft_SPI_Stop(void)
{
    Soft_SPI_W_SS(1); // 拉高SS，终止时序
}

/**
 * 函    数：W25Q64初始化
 * 参    数：无
 * 返 回 值：无
 */
void W25Q64_Init(void)
{
    Soft_SPI_Init(); // 先初始化底层的Soft_SPI

    /*设置默认电平*/
    Soft_SPI_W_SS(1);  // SS默认高电平
}

/**
 * 函    数：MPU6050读取ID号
 * 参    数：MID 工厂ID，使用输出参数的形式返回
 * 参    数：DID 设备ID，使用输出参数的形式返回
 * 返 回 值：无
 */
void W25Q64_ReadID(uint8_t *MID, uint16_t *DID)
{
    Soft_SPI_Start();                             // Soft_SPI起始
    Soft_SPI_SwapByte(W25Q64_JEDEC_ID);           // 交换发送读取ID的指令
    *MID = Soft_SPI_SwapByte(W25Q64_DUMMY_BYTE);  // 交换接收MID，通过输出参数返回
    *DID = Soft_SPI_SwapByte(W25Q64_DUMMY_BYTE);  // 交换接收DID高8位
    *DID <<= 8;                              // 高8位移到高位
    *DID |= Soft_SPI_SwapByte(W25Q64_DUMMY_BYTE); // 或上交换接收DID的低8位，通过输出参数返回
    Soft_SPI_Stop();                              // Soft_SPI终止
}

/**
 * 函    数：W25Q64写使能
 * 参    数：无
 * 返 回 值：无
 */
void W25Q64_WriteEnable(void)
{
    Soft_SPI_Start();                       // Soft_SPI起始
    Soft_SPI_SwapByte(W25Q64_WRITE_ENABLE); // 交换发送写使能的指令
    Soft_SPI_Stop();                        // Soft_SPI终止
}

/**
 * 函    数：W25Q64等待忙
 * 参    数：无
 * 返 回 值：无
 */
void W25Q64_WaitBusy(void)
{
    uint32_t Timeout;
    Soft_SPI_Start();                                             // Soft_SPI起始
    Soft_SPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1);             // 交换发送读状态寄存器1的指令
    Timeout = 100000;                                        // 给定超时计数时间
    while ((Soft_SPI_SwapByte(W25Q64_DUMMY_BYTE) & 0x01) == 0x01) // 循环等待忙标志位
    {
        Timeout--;        // 等待时，计数值自减
        if (Timeout == 0) // 自减到0后，等待超时
        {
            /*超时的错误处理代码，可以添加到此处*/
            break; // 跳出等待，不等了
        }
    }
    Soft_SPI_Stop(); // Soft_SPI终止
}

/**
 * 函    数：W25Q64页编程
 * 参    数：Address 页编程的起始地址，范围：0x000000~0x7FFFFF
 * 参    数：DataArray	用于写入数据的数组
 * 参    数：Count 要写入数据的数量，范围：0~256
 * 返 回 值：无
 * 注意事项：写入的地址范围不能跨页
 */
void W25Q64_PageProgram(uint32_t Address, uint8_t *DataArray, uint16_t Count)
{
    uint16_t i;

    W25Q64_WriteEnable(); // 写使能

    Soft_SPI_Start();                       // Soft_SPI起始
    Soft_SPI_SwapByte(W25Q64_PAGE_PROGRAM); // 交换发送页编程的指令
    Soft_SPI_SwapByte(Address >> 16);       // 交换发送地址23~16位
    Soft_SPI_SwapByte(Address >> 8);        // 交换发送地址15~8位
    Soft_SPI_SwapByte(Address);             // 交换发送地址7~0位
    for (i = 0; i < Count; i++)        // 循环Count次
    {
        Soft_SPI_SwapByte(DataArray[i]); // 依次在起始地址后写入数据
    }
    Soft_SPI_Stop(); // Soft_SPI终止

    W25Q64_WaitBusy(); // 等待忙
}

/**
 * 函    数：W25Q64扇区擦除（4KB）
 * 参    数：Address 指定扇区的地址，范围：0x000000~0x7FFFFF
 * 返 回 值：无
 */
void W25Q64_SectorErase(uint32_t Address)
{
    W25Q64_WriteEnable(); // 写使能

    Soft_SPI_Start();                           // Soft_SPI起始
    Soft_SPI_SwapByte(W25Q64_SECTOR_ERASE_4KB); // 交换发送扇区擦除的指令
    Soft_SPI_SwapByte(Address >> 16);           // 交换发送地址23~16位
    Soft_SPI_SwapByte(Address >> 8);            // 交换发送地址15~8位
    Soft_SPI_SwapByte(Address);                 // 交换发送地址7~0位
    Soft_SPI_Stop();                            // Soft_SPI终止

    W25Q64_WaitBusy(); // 等待忙
}

/**
 * 函    数：W25Q64读取数据
 * 参    数：Address 读取数据的起始地址，范围：0x000000~0x7FFFFF
 * 参    数：DataArray 用于接收读取数据的数组，通过输出参数返回
 * 参    数：Count 要读取数据的数量，范围：0~0x800000
 * 返 回 值：无
 */
void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count)
{
    uint32_t i;
    Soft_SPI_Start();                    // Soft_SPI起始
    Soft_SPI_SwapByte(W25Q64_READ_DATA); // 交换发送读取数据的指令
    Soft_SPI_SwapByte(Address >> 16);    // 交换发送地址23~16位
    Soft_SPI_SwapByte(Address >> 8);     // 交换发送地址15~8位
    Soft_SPI_SwapByte(Address);          // 交换发送地址7~0位
    for (i = 0; i < Count; i++)     // 循环Count次
    {
        DataArray[i] = Soft_SPI_SwapByte(W25Q64_DUMMY_BYTE); // 依次在起始地址后读取数据
    }
    Soft_SPI_Stop(); // Soft_SPI终止
}

#else // Hard_SPI
#include "Hard_SPI.h" // 硬件SPI

/**
 * 函    数：SPI写SS引脚电平，SS仍由软件模拟
 * 参    数：BitValue 协议层传入的当前需要写入SS的电平，范围0~1
 * 返 回 值：无
 * 注意事项：此函数需要用户实现内容，当BitValue为0时，需要置SS为低电平，当BitValue为1时，需要置SS为高电平
 */
void Hard_SPI_W_SS(uint8_t BitValue)
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_8, (BitAction)BitValue); // 根据BitValue，设置SS引脚的电平
}

/**
 * 函    数：SPI起始
 * 参    数：无
 * 返 回 值：无
 */
void Hard_SPI_Start(void)
{
    Hard_SPI_W_SS(0); // 拉低SS，开始时序
}

/**
 * 函    数：SPI终止
 * 参    数：无
 * 返 回 值：无
 */
void Hard_SPI_Stop(void)
{
    Hard_SPI_W_SS(1); // 拉高SS，终止时序
}

/**
 * 函    数：W25Q64初始化
 * 参    数：无
 * 返 回 值：无
 */
void W25Q64_Init(void)
{
    Hard_SPI_Init(); // 先初始化底层的SPI

    /*设置默认电平*/
    Hard_SPI_W_SS(1); // SS默认高电平
}

/**
 * 函    数：MPU6050读取ID号
 * 参    数：MID 工厂ID，使用输出参数的形式返回
 * 参    数：DID 设备ID，使用输出参数的形式返回
 * 返 回 值：无
 */
void W25Q64_ReadID(uint8_t *MID, uint16_t *DID)
{
    Hard_SPI_Start();                             // SPI起始
    Hard_SPI_SwapByte(W25Q64_JEDEC_ID);           // 交换发送读取ID的指令
    *MID = Hard_SPI_SwapByte(W25Q64_DUMMY_BYTE);  // 交换接收MID，通过输出参数返回
    *DID = Hard_SPI_SwapByte(W25Q64_DUMMY_BYTE);  // 交换接收DID高8位
    *DID <<= 8;                                // 高8位移到高位
    *DID |= Hard_SPI_SwapByte(W25Q64_DUMMY_BYTE); // 或上交换接收DID的低8位，通过输出参数返回
    Hard_SPI_Stop();                              // SPI终止
}

/**
 * 函    数：W25Q64写使能
 * 参    数：无
 * 返 回 值：无
 */
void W25Q64_WriteEnable(void)
{
    Hard_SPI_Start();                       // SPI起始
    Hard_SPI_SwapByte(W25Q64_WRITE_ENABLE); // 交换发送写使能的指令
    Hard_SPI_Stop();                        // SPI终止
}

/**
 * 函    数：W25Q64等待忙
 * 参    数：无
 * 返 回 值：无
 */
void W25Q64_WaitBusy(void)
{
    uint32_t Timeout;
    Hard_SPI_Start();                                             // SPI起始
    Hard_SPI_SwapByte(W25Q64_READ_STATUS_REGISTER_1);             // 交换发送读状态寄存器1的指令
    Timeout = 100000;                                          // 给定超时计数时间
    while ((Hard_SPI_SwapByte(W25Q64_DUMMY_BYTE) & 0x01) == 0x01) // 循环等待忙标志位
    {
        Timeout--;        // 等待时，计数值自减
        if (Timeout == 0) // 自减到0后，等待超时
        {
            /*超时的错误处理代码，可以添加到此处*/
            break; // 跳出等待，不等了
        }
    }
    Hard_SPI_Stop(); // SPI终止
}

/**
 * 函    数：W25Q64页编程
 * 参    数：Address 页编程的起始地址，范围：0x000000~0x7FFFFF
 * 参    数：DataArray	用于写入数据的数组
 * 参    数：Count 要写入数据的数量，范围：0~256
 * 返 回 值：无
 * 注意事项：写入的地址范围不能跨页
 */
void W25Q64_PageProgram(uint32_t Address, uint8_t *DataArray, uint16_t Count)
{
    uint16_t i;

    W25Q64_WriteEnable(); // 写使能

    Hard_SPI_Start();                       // SPI起始
    Hard_SPI_SwapByte(W25Q64_PAGE_PROGRAM); // 交换发送页编程的指令
    Hard_SPI_SwapByte(Address >> 16);       // 交换发送地址23~16位
    Hard_SPI_SwapByte(Address >> 8);        // 交换发送地址15~8位
    Hard_SPI_SwapByte(Address);             // 交换发送地址7~0位
    for (i = 0; i < Count; i++)          // 循环Count次
    {
        Hard_SPI_SwapByte(DataArray[i]); // 依次在起始地址后写入数据
    }
    Hard_SPI_Stop(); // SPI终止

    W25Q64_WaitBusy(); // 等待忙
}

/**
 * 函    数：W25Q64扇区擦除（4KB）
 * 参    数：Address 指定扇区的地址，范围：0x000000~0x7FFFFF
 * 返 回 值：无
 */
void W25Q64_SectorErase(uint32_t Address)
{
    W25Q64_WriteEnable(); // 写使能

    Hard_SPI_Start();                           // SPI起始
    Hard_SPI_SwapByte(W25Q64_SECTOR_ERASE_4KB); // 交换发送扇区擦除的指令
    Hard_SPI_SwapByte(Address >> 16);           // 交换发送地址23~16位
    Hard_SPI_SwapByte(Address >> 8);            // 交换发送地址15~8位
    Hard_SPI_SwapByte(Address);                 // 交换发送地址7~0位
    Hard_SPI_Stop();                            // SPI终止

    W25Q64_WaitBusy(); // 等待忙
}

/**
 * 函    数：W25Q64读取数据
 * 参    数：Address 读取数据的起始地址，范围：0x000000~0x7FFFFF
 * 参    数：DataArray 用于接收读取数据的数组，通过输出参数返回
 * 参    数：Count 要读取数据的数量，范围：0~0x800000
 * 返 回 值：无
 */
void W25Q64_ReadData(uint32_t Address, uint8_t *DataArray, uint32_t Count)
{
    uint32_t i;
    Hard_SPI_Start();                    // SPI起始
    Hard_SPI_SwapByte(W25Q64_READ_DATA); // 交换发送读取数据的指令
    Hard_SPI_SwapByte(Address >> 16);    // 交换发送地址23~16位
    Hard_SPI_SwapByte(Address >> 8);     // 交换发送地址15~8位
    Hard_SPI_SwapByte(Address);          // 交换发送地址7~0位
    for (i = 0; i < Count; i++)       // 循环Count次
    {
        DataArray[i] = Hard_SPI_SwapByte(W25Q64_DUMMY_BYTE); // 依次在起始地址后读取数据
    }
    Hard_SPI_Stop(); // SPI终止
}

#endif // Soft_SPI