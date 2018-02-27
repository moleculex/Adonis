#include "stm32f30x_pwr.h"
#include "stm32f30x_rtc.h"
#include "stm32f30x_rcc.h"
#include "power.h"

void power_init(void)
{
	  RTC_InitTypeDef   RTC_InitStructure;
	  RTC_TimeTypeDef   RTC_TimeStructure;

	  /* Enable the PWR clock */
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	  /* Allow access to Backup Domain */
	  PWR_BackupAccessCmd(ENABLE);

	  /* Disable wake-up source(ALARM) to guarantee free access to WUT level-OR input */
	  RTC_ITConfig(RTC_IT_ALRA, DISABLE);

	  /* Clear Wakeup flag */
	  PWR_ClearFlag(PWR_FLAG_WU);

	  /* Enable wake-up source(ALARM) to guarantee free access to WUT level-OR input */
	  RTC_ITConfig(RTC_IT_ALRA, ENABLE);

	  /* Enable the LSI OSC */
	  RCC_LSICmd(ENABLE);

	  /* Wait till LSI is ready */
	  while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
	  {}

	  /* Check if the StandBy flag is set */
	  if (PWR_GetFlagStatus(PWR_FLAG_SB) != RESET)
	  {
			/* Clear StandBy flag */
			PWR_ClearFlag(PWR_FLAG_SB);

			/* Check if the StandBy flag is cleared */
			if (PWR_GetFlagStatus(PWR_FLAG_SB) != RESET)
			{
			  while(1);
			}

			RTC_WaitForSynchro();

			/* No need to configure the RTC as the RTC config(clock source, enable,
			prescaler,...) are kept after wake-up from STANDBY */
	  }
	  else
	  {
			/* RTC Configuration ******************************************************/
			/* Reset Backup Domain */
			RCC_BackupResetCmd(ENABLE);
			RCC_BackupResetCmd(DISABLE);

			/* Select the RTC Clock Source */
			RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

			/* Enable the RTC Clock */
			RCC_RTCCLKCmd(ENABLE);

			/* Wait for RTC APB registers synchronisation */
			RTC_WaitForSynchro();

			RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
			RTC_InitStructure.RTC_AsynchPrediv = 99;
			RTC_InitStructure.RTC_SynchPrediv =  (LSI_VALUE/100) - 1;

			RTC_Init(&RTC_InitStructure);

			/* Set the time to 01h 00mn 00s AM */
			RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
			RTC_TimeStructure.RTC_Hours   = 0x01;
			RTC_TimeStructure.RTC_Minutes = 0x00;
			RTC_TimeStructure.RTC_Seconds = 0x00;

			RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);
	  }
}

void power_standby(void)
{
	  RTC_AlarmTypeDef  RTC_AlarmStructure;
	  RTC_TimeTypeDef   RTC_TimeStructure;

	  /* Disable the Alarm A */
	  RTC_AlarmCmd(RTC_Alarm_A, DISABLE);

	  /* Get the current time */
	  RTC_GetTime(RTC_Format_BCD, &RTC_TimeStructure);

	  /* Set the alarm to current time + 3s */
	  RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_TimeStructure.RTC_H12;
	  RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = RTC_TimeStructure.RTC_Hours;
	  RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = (RTC_TimeStructure.RTC_Minutes + 0x1) % 60;
	  RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = RTC_TimeStructure.RTC_Seconds;
	  RTC_AlarmStructure.RTC_AlarmDateWeekDay = 31;
	  RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
	  RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay | RTC_AlarmMask_Hours | RTC_AlarmMask_Minutes;
	  RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);

	  /* Enable RTC Alarm A Interrupt: this Interrupt will wake-up the system from
	  STANDBY mode (RTC Alarm IT not enabled in NVIC) */
	  RTC_ITConfig(RTC_IT_ALRA, ENABLE);

	  /* Enable the Alarm A */
	  RTC_AlarmCmd(RTC_Alarm_A, ENABLE);

	  /* Clear RTC Alarm Flag */
	  RTC_ClearFlag(RTC_FLAG_ALRAF);

	  /* Request to enter STANDBY mode (Wake Up flag is cleared in PWR_EnterSTANDBYMode function) */
	  PWR_EnterSTANDBYMode();
}

/*void power_standby(void)
{
	RTC_InitTypeDef  RTC_InitStructure;
	RTC_AlarmTypeDef RTC_AlarmStructure;
	RTC_TimeTypeDef  RTC_TimeStructure;

	// RTC Configuration
	// Enable the PWR clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	// Allow access to Backup Domain
	PWR_BackupAccessCmd(ENABLE);

	// Enable the LSI OSC
	RCC_LSICmd(ENABLE);

	// Wait till LSI is ready
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET){}

	// Reset Backup Domain
	RCC_BackupResetCmd(ENABLE);
	RCC_BackupResetCmd(DISABLE);

	// Select the RTC Clock Source
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);

	// Enable the RTC Clock
	RCC_RTCCLKCmd(ENABLE);

	// Wait for RTC APB registers synchronisation
	RTC_WaitForSynchro();

	RTC_InitStructure.RTC_HourFormat = RTC_HourFormat_24;
	RTC_InitStructure.RTC_AsynchPrediv = 0x7F;
	RTC_InitStructure.RTC_SynchPrediv = 0x0138;

	RTC_Init(&RTC_InitStructure);

	// Set the alarm X+60s
	RTC_AlarmStructure.RTC_AlarmTime.RTC_H12     = RTC_H12_AM;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Hours   = 0x01;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Minutes = 0x01;
	RTC_AlarmStructure.RTC_AlarmTime.RTC_Seconds = 0x00;
	RTC_AlarmStructure.RTC_AlarmDateWeekDay = 0x31;
	RTC_AlarmStructure.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date;
	RTC_AlarmStructure.RTC_AlarmMask = RTC_AlarmMask_DateWeekDay;
	RTC_SetAlarm(RTC_Format_BCD, RTC_Alarm_A, &RTC_AlarmStructure);

	// Enable RTC Alarm A Interrupt
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);

	// Enable the alarm
	RTC_AlarmCmd(RTC_Alarm_A, ENABLE);

	// Set the time to 01h 00mn 00s AM
	RTC_TimeStructure.RTC_H12     = RTC_H12_AM;
	RTC_TimeStructure.RTC_Hours   = 0x01;
	RTC_TimeStructure.RTC_Minutes = 0x00;
	RTC_TimeStructure.RTC_Seconds = 0x00;

	RTC_SetTime(RTC_Format_BCD, &RTC_TimeStructure);

	// Clear Wakeup flag
	PWR_ClearFlag(PWR_FLAG_WU);

	RTC_ClearFlag(RTC_FLAG_ALRAF);

	// Request to enter STANDBY mode (Wake Up flag is cleared in PWR_EnterSTANDBYMode function)
	PWR_EnterSTANDBYMode();
}*/
