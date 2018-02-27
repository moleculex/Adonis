#ifndef __PLATFORM_CONFIG_H
#define __PLATFORM_CONFIG_H

#include "stm32f30x.h"
#include "stm32f30x_gpio.h"
#include "stm32f30x_rcc.h"
#include "stdbool.h"

//RN2483
#define RN2483_RCC_AHB				RCC_AHBPeriph_GPIOA
#define RN2483_RCC_APB				RCC_APB2Periph_USART1
#define RN2483_PORT 				GPIOA
#define RN2483_TX_PIN				GPIO_Pin_9
#define RN2483_RX_PIN				GPIO_Pin_10
#define RN2483_TX_PINSOURCE			GPIO_PinSource9
#define RN2483_RX_PINSOURCE			GPIO_PinSource10
#define RN2483_USART				USART1
#define RN2483_USART_IRQN			USART1_IRQn
#define rn2483_read 				USART1_IRQHandler

#define RN2483_RST_RCC_AHB			RCC_AHBPeriph_GPIOB
#define RN2483_RST_PORT				GPIOB
#define RN2483_RST_PIN				GPIO_Pin_5

//GPS
#define GPS_RCC_AHB			RCC_AHBPeriph_GPIOA
#define GPS_RCC_APB			RCC_APB1Periph_USART2
#define GPS_PORT			GPIOA
#define GPS_TX_PIN			GPIO_Pin_3
#define GPS_RX_PIN			GPIO_Pin_2
#define GPS_TX_PINSOURCE	GPIO_PinSource3
#define GPS_RX_PINSOURCE	GPIO_PinSource2
#define GPS_USART			USART2
#define GPS_USART_IRQN		USART2_IRQn
#define gps_read 			USART2_IRQHandler

//voltage
#define VSEL12_PORT			GPIOA
#define VSEL1_PIN			GPIO_Pin_1
#define VSEL2_PIN			GPIO_Pin_4
#define VSEL12_RCC_AHB		RCC_AHBPeriph_GPIOA

#define VSEL34_PORT			GPIOB
#define VSEL3_PIN			GPIO_Pin_6
#define VSEL4_PIN			GPIO_Pin_7
#define VSEL34_RCC_AHB		RCC_AHBPeriph_GPIOB

bool active, tilt, seismic;

#define TILT
#define SEISMIC
//#define TEST

#endif
