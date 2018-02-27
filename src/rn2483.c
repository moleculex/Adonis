#include "rn2483.h"
#include "platform_config.h"
#include "stm32f30x_usart.h"
#include "stm32f30x_misc.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

void rn2483_init(void)
{
	_rn2483.rxBuffer_ptr = &_rn2483.rxBuffer[0];


	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RN2483_RCC_AHB | RN2483_RST_RCC_AHB, ENABLE);
	RCC_APB2PeriphClockCmd(RN2483_RCC_APB, ENABLE);

	//RST
	GPIO_InitStructure.GPIO_Pin = RN2483_RST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //was OD
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RN2483_RST_PORT, &GPIO_InitStructure);
	GPIO_SetBits(RN2483_RST_PORT, RN2483_RST_PIN);

	/* Connect USART pins to AF */
	GPIO_PinAFConfig(RN2483_PORT, RN2483_TX_PINSOURCE, GPIO_AF_7);
	GPIO_PinAFConfig(RN2483_PORT, RN2483_RX_PINSOURCE, GPIO_AF_7);

	GPIO_InitStructure.GPIO_Pin = RN2483_TX_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(RN2483_PORT, &GPIO_InitStructure);

	/* Configure USART Rx as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = RN2483_RX_PIN;
	GPIO_Init(RN2483_PORT, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 57600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(RN2483_USART,&USART_InitStructure);

	USART_ITConfig(RN2483_USART, USART_IT_RXNE, ENABLE);

	NVIC_InitStructure.NVIC_IRQChannel = RN2483_USART_IRQN;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_Cmd(RN2483_USART, ENABLE);
}

void rn2483_join(void)
{
	GPIO_ResetBits(RN2483_RST_PORT, RN2483_RST_PIN);
	vTaskDelay(1000);
	GPIO_SetBits(RN2483_RST_PORT, RN2483_RST_PIN);
	vTaskDelay(1000);
	GPIO_ResetBits(RN2483_RST_PORT, RN2483_RST_PIN);
	vTaskDelay(1000);

	_rn2483.rxBuffer_ptr = &_rn2483.rxBuffer[0];
	rn2483_write("sys get hweui\r\n", 15);
	vTaskDelay(200);
	memcpy(&_rn2483.deveui, &_rn2483.rxBuffer, 16);

	strcpy(_rn2483.txBuffer, "mac set deveui ");
	strncat(_rn2483.txBuffer, _rn2483.deveui, 16);
	strcat(_rn2483.txBuffer, "\r\n");
	rn2483_write(_rn2483.txBuffer, 33);
	vTaskDelay(200);

	rn2483_write("mac set appeui 70B3D576AE000007\r\n",33);
	vTaskDelay(200);

	rn2483_write("mac set appkey 13341334133413341334133413341334\r\n", 49);
	vTaskDelay(200);

	rn2483_write("mac set dr 0\r\n", 14);
	vTaskDelay(200);

	rn2483_write("mac set adr off\r\n", 17);
	vTaskDelay(200);

	rn2483_write("mac join otaa\r\n", 15);
	IWDG_ReloadCounter();
	vTaskDelay(10000);
}

void rn2483_sleep(void)
{
	rn2483_write("sys sleep 4294967295\r\n", 22);
	vTaskDelay(1000);
}

void rn2483_write(char *buffer, char length)
{
	for(uint8_t i = 0; i < length; i++)
	{
		USART_SendData(RN2483_USART, buffer[i]);
		while (USART_GetFlagStatus(RN2483_USART, USART_FLAG_TXE) == RESET);
	}
}

void rn2483_read(void)
{
	if(USART_GetITStatus(RN2483_USART, USART_IT_RXNE) != RESET)
	{
		char inchar = (char)USART_ReceiveData(RN2483_USART);
		*_rn2483.rxBuffer_ptr++ = inchar;
	}
}

