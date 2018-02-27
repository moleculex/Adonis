#include "platform_config.h"
#include "stm32f30x_i2c.h"
#include "kx122.h"
#include "rn2483.h"
#include "flash.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

char ins2;

void kx122_init(void)
{
	  GPIO_InitTypeDef GPIO_InitStructure;
	  I2C_InitTypeDef  I2C_InitStructure;

	  RCC_I2CCLKConfig(RCC_I2C1CLK_SYSCLK);

	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_4);
	  GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_4);

	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	  GPIO_Init(GPIOB, &GPIO_InitStructure);

	  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_7;
	  GPIO_Init(GPIOB, &GPIO_InitStructure);

	  I2C_DeInit(I2C1);

	  I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	  I2C_InitStructure.I2C_AnalogFilter = I2C_AnalogFilter_Enable;
	  I2C_InitStructure.I2C_DigitalFilter = 0x00;
	  I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	  I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	  I2C_InitStructure.I2C_Timing = 0xC062121F;

	  I2C_Init(I2C1, &I2C_InitStructure);
	  I2C_Cmd(I2C1, ENABLE);

	  //exti
	  EXTI_InitTypeDef   EXTI_InitStructure;
	  NVIC_InitTypeDef   NVIC_InitStructure;

	  /* Enable GPIOA clock */
	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	  /* Configure PA1 pin as input floating */
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);

	  /* Enable SYSCFG clock */
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	  /* Connect EXTI1 Line to PA1 pin */
	  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

	  /* Configure EXTI1 line */
	  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
	  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	  EXTI_Init(&EXTI_InitStructure);

	  /* Enable and set EXTI0 Interrupt to the lowest priority */
	  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0f;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	  NVIC_Init(&NVIC_InitStructure);

	  //leds
	  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //was OD
	  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	  GPIO_Init(GPIOB, &GPIO_InitStructure);

	  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	  GPIO_Init(GPIOA, &GPIO_InitStructure);
}


uint8_t kx122_read(uint8_t addr)
{
	uint8_t d;
	//int t;

	//t = 1000;
	I2C_TransferHandling(I2C1, 0x3e, 1, I2C_SoftEnd_Mode, I2C_Generate_Start_Write);
	while(I2C_GetFlagStatus(I2C1, I2C_ISR_TXIS) == RESET/* && t > 0*/);
		//t--;


	//t = 1000;
	I2C_SendData(I2C1, addr);
	while(I2C_GetFlagStatus(I2C1, I2C_ISR_TC) == RESET /*&& t > 0*/);
		//t--;


	//t = 1000;
	I2C_TransferHandling(I2C1, 0x3f, 1, I2C_AutoEnd_Mode, I2C_Generate_Start_Read);
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET/* && t > 0*/);
		//t--;

	d = I2C_ReceiveData(I2C1);

    /*while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));
    I2C_ClearFlag(I2C1, I2C_ICR_STOPCF);*/

	return d;
}

void kx122_write(uint8_t addr, uint8_t data)
{
	//int t;

	//t = 1000;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)!=RESET /*&& t > 0*/);
		//t--;

	//t = 1000;
	I2C_TransferHandling(I2C1, 0x3e, 2, I2C_AutoEnd_Mode, I2C_Generate_Start_Write);
	while(I2C_GetFlagStatus(I2C1, I2C_ISR_TXIS) == RESET /*&& t > 0*/);
		//t--;

	//t = 1000;
	I2C_SendData(I2C1, addr);
	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_TXE) == RESET /*&& t > 0*/);
		//t--;

	//t = 1000;
	I2C_SendData(I2C1, data);
	while(I2C_GetFlagStatus(I2C1,I2C_FLAG_TXE) == RESET /*&& t > 0*/);
		//t--;

    //while(I2C_GetFlagStatus(I2C1, I2C_FLAG_STOPF));
    //I2C_ClearFlag(I2C2, I2C_ICR_STOPCF);

}

void EXTI0_IRQHandler(void)
{
  if (EXTI_GetITStatus(EXTI_Line0) == SET)
  {
	  //flash_erase(0x0800FA00);
	  active = true;
	  //_flash.buffer[0] = 0x01;

	  ins2 = kx122_read(0x13);

	  if((ins2 & 0x01) == 0x01)
		  tilt = true;//_flash.buffer[1] = 0x01;

	  if((ins2 & 0x04) == 0x04)
		  seismic = true;//_flash.buffer[2] = 0x01;

	  //flash_write(0x0800FA00, _flash.buffer, 3);

	  kx122_read(0x17);

	  EXTI_ClearITPendingBit(EXTI_Line0);
  }
}
