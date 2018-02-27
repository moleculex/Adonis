#include "app.h"
#include "rn2483.h"
#include "platform_config.h"
#include "stm32f30x_iwdg.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

void SetSysClockTo8(void);

int main(void)
{
	SetSysClockTo8();
	SysTick_Config(8000);
	//set_voltage();

	/*IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	IWDG_SetPrescaler(IWDG_Prescaler_64);
	IWDG_SetReload(5*40000/64);
	IWDG_ReloadCounter();
	IWDG_Enable();*/

	xTaskCreate((pdTASK_CODE)app,
				(const signed char * const)"app",
				128,
				NULL,
				2,
				NULL);


	/*xTaskCreate((pdTASK_CODE)acc_task,
				(const signed char * const)"acc_task",
				384,
				NULL,
				2,
				NULL);*/

	vTaskStartScheduler();

	while(1);
}

ErrorStatus HSEStartUpStatus;

void SetSysClockTo8(void)
{
  /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration -----------------------*/
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if (HSEStartUpStatus == SUCCESS)
  {
    /* Flash 0 wait state */
    FLASH_SetLatency(FLASH_Latency_0);
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);

    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1);

    /* PCLK1 = HCLK */
    RCC_PCLK1Config(RCC_HCLK_Div1);

    /* PLLCLK = (8MHz/2) * 6 = 24 MHz */
    RCC_PREDIV1Config(RCC_PREDIV1_Div2);
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_2);

    /* Enable PLL */
    RCC_PLLCmd(ENABLE);

    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
    {
    }

    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* Wait till PLL is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x08)
    {
    }
  }
  else
  {
    /* If HSE fails to start-up, the application will have wrong clock configuration.
       User can add here some code to deal with this error */

    /* Go to infinite loop */
    while (1)
    {
    }
  }
}

