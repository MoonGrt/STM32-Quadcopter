#ifndef __LED_H
#define __LED_H

#include "stm32f10x.h"

// IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000) + 0x2000000 + ((addr & 0xFFFFF) << 5) + (bitnum << 2))
#define MEM_ADDR(addr) *((volatile unsigned long *)(addr))
#define BIT_ADDR(addr, bitnum) MEM_ADDR(BITBAND(addr, bitnum))

// IO口地址映射
#define GPIOA_ODR_Addr (GPIOA_BASE + 12) // 0x4001080C
#define GPIOB_ODR_Addr (GPIOB_BASE + 12) // 0x40010C0C
#define GPIOC_ODR_Addr (GPIOC_BASE + 12) // 0x4001100C
#define GPIOD_ODR_Addr (GPIOD_BASE + 12) // 0x4001140C
#define GPIOE_ODR_Addr (GPIOE_BASE + 12) // 0x4001180C
#define GPIOF_ODR_Addr (GPIOF_BASE + 12) // 0x40011A0C
#define GPIOG_ODR_Addr (GPIOG_BASE + 12) // 0x40011E0C

#define GPIOA_IDR_Addr (GPIOA_BASE + 8) // 0x40010808
#define GPIOB_IDR_Addr (GPIOB_BASE + 8) // 0x40010C08
#define GPIOC_IDR_Addr (GPIOC_BASE + 8) // 0x40011008
#define GPIOD_IDR_Addr (GPIOD_BASE + 8) // 0x40011408
#define GPIOE_IDR_Addr (GPIOE_BASE + 8) // 0x40011808
#define GPIOF_IDR_Addr (GPIOF_BASE + 8) // 0x40011A08
#define GPIOG_IDR_Addr (GPIOG_BASE + 8) // 0x40011E08

#define PAout(n) BIT_ADDR(GPIOA_ODR_Addr, n)
#define PAin(n) BIT_ADDR(GPIOA_IDR_Addr, n)
#define PBout(n) BIT_ADDR(GPIOB_ODR_Addr, n)
#define PBin(n) BIT_ADDR(GPIOB_IDR_Addr, n)
#define PCout(n) BIT_ADDR(GPIOC_ODR_Addr, n)
#define PCin(n) BIT_ADDR(GPIOC_IDR_Addr, n)
#define PDout(n) BIT_ADDR(GPIOD_ODR_Addr, n)
#define PDin(n) BIT_ADDR(GPIOD_IDR_Addr, n)

#define LED1(x) x ? (GPIOB->BSRR = GPIO_Pin_8) : (GPIOB->BRR = GPIO_Pin_8)
#define LED2(x) x ? (GPIOB->BSRR = GPIO_Pin_5) : (GPIOB->BRR = GPIO_Pin_5)
#define LED3(x) x ? (GPIOB->BSRR = GPIO_Pin_10) : (GPIOB->BRR = GPIO_Pin_10)
#define LED4(x) x ? (GPIOB->BSRR = GPIO_Pin_9) : (GPIOB->BRR = GPIO_Pin_9)
#define OP_LED1 (GPIOB->IDR & GPIO_Pin_8) ? (GPIOB->BRR = GPIO_Pin_8) : (GPIOB->BSRR = GPIO_Pin_8)
#define OP_LED2 (GPIOB->IDR & GPIO_Pin_5) ? (GPIOB->BRR = GPIO_Pin_5) : (GPIOB->BSRR = GPIO_Pin_5)
#define OP_LED3 (GPIOB->IDR & GPIO_Pin_10) ? (GPIOB->BRR = GPIO_Pin_10) : (GPIOB->BSRR = GPIO_Pin_10)
#define OP_LED4 (GPIOB->IDR & GPIO_Pin_9) ? (GPIOB->BRR = GPIO_Pin_9) : (GPIOB->BSRR = GPIO_Pin_9)

// 机身后灯
#define fLED_H() GPIOB->BSRR = GPIO_Pin_1
#define fLED_L() GPIOB->BRR = GPIO_Pin_1
#define fLED_Toggle() GPIOB->ODR ^= GPIO_Pin_1

#define hLED_H() GPIOB->BSRR = GPIO_Pin_2
#define hLED_L() GPIOB->BRR = GPIO_Pin_2
#define hLED_Toggle() GPIOB->ODR ^= GPIO_Pin_2

//-------------------------------------------------
// 机身前灯
#define aLED_H() GPIOB->BSRR = GPIO_Pin_8
#define aLED_L() GPIOB->BRR = GPIO_Pin_8
#define aLED_Toggle() GPIOB->ODR ^= GPIO_Pin_8

#define bLED_H() GPIOB->BSRR = GPIO_Pin_9
#define bLED_L() GPIOB->BRR = GPIO_Pin_9
#define bLED_Toggle() GPIOB->ODR ^= GPIO_Pin_9

typedef struct
{
	uint16_t FlashTime;
	enum
	{
		AlwaysOn,
		AlwaysOff,
		AllFlashLight,
		AlternateFlash,
		WARNING,
		DANGEROURS,
		GET_OFFSET
	} status;
} sLED;

extern sLED LED;
extern void LEDInit(void);
extern void LEDtest(void);
extern void PilotLED(void);

#define LED_TAKE_OFF_ENTER LED.status = WARNING
#define LED_TAKE_OFF_EXIT LED.status = AllFlashLight
#define LED_HEIGHT_LOCK_ENTER \
	LED.FlashTime = 100;      \
	LED.status = AlternateFlash
#define LED_HEIGHT_LOCK_EXIT \
	LED.FlashTime = 200;     \
	LED.status = AllFlashLight
#define LED_3D_ROLL_ENTER LED.status = WARNING
#define LED_3D_ROLL_EXIT LED.status = AllFlashLight
#define LED_SAFTY_TAKE_DOWN_ENTER LED.status = DANGEROURS
#define LED_SAFTY_TAKE_DOWN_EXIT LED.status = AlwaysOn
#define LED_GET_MPU_OFFSET_ENTER LED.status = GET_OFFSET
#define LED_GO_HOME_ENTER LED.status = WARNING
#define LED_GO_HOME_EXIT LED.status = AllFlashLight

#endif
