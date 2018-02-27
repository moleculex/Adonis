#include "app.h"
#include "kx122.h"
#include "rn2483.h"
#include "platform_config.h"
#include "power.h"
#include "flash.h"

#include "stm32f30x_iwdg.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

char x,y,z;
char d,e,f;
char ins2;

void app(void)
{
#ifdef TILT
	tilt = false;
#endif

#ifdef SEISMIC
	seismic = false;
#endif

	rn2483_init();
	//rn2483_join();
	kx122_init();
	//power_init();
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

	GPIO_SetBits(GPIOB, GPIO_Pin_0);
	GPIO_SetBits(GPIOA, GPIO_Pin_7);
	//IWDG_ReloadCounter();
	vTaskDelay(500);

	//IWDG_ReloadCounter();
	//vTaskDelay(2000);

	//IWDG_ReloadCounter();

	ins2 = kx122_read(0x13);

	GPIO_ResetBits(GPIOB, GPIO_Pin_0);
	GPIO_ResetBits(GPIOA, GPIO_Pin_7);

#ifdef TILT
	if((ins2 & 0x01) == 0x01)
	{
		vTaskDelay(500);
		x = kx122_read(0x07);
		y = kx122_read(0x09);
		if(x > 25 || x < -25 || y > 25 || y < -25)
		{
			//tilt = true;

			while(x > 25 || x < -25 || y > 25 || y < -25)
			{
				GPIO_SetBits(GPIOB, GPIO_Pin_0);
				rn2483_join();
				rn2483_write("mac tx uncnf 10 3f3f3f02f2a40101\r\n", 34);
				GPIO_ResetBits(GPIOB, GPIO_Pin_0);
				vTaskDelay(10000);
				x = kx122_read(0x07);
				y = kx122_read(0x09);
			}
		}
	}
#endif

#ifdef SEISMIC
	if((ins2 & 0x04) == 0x04)
		seismic = true;
#endif

	//d = kx122_read(0x0f); //WHO_AM_I
    //e = kx122_read(0x17); //INT_REL

#if defined(TILT) && defined(SEISMIC)
    kx122_write(0x18, 0x05); //CNTL1 tilt and tap
#else
#ifdef TILT
    kx122_write(0x18, 0x41);
#endif

#ifdef SEISMIC
    kx122_write(0x18, 0x04);
#endif
#endif
	kx122_write(0x19, 0x3C); //CNTL2 tilt  x-/x+ & y-/y+
#ifdef SEISMIC
	kx122_write(0x1A, 0x80); //CNTL3
#endif
	kx122_write(0x22, 0x01);
#ifdef SEISMIC
	kx122_write(0x26, 0xFF);//TTH high thresh
	kx122_write(0x27, 0x5A); //TTL low thresh
	kx122_write(0x28, 0xF1);//FTD (1/50HZ)*15 - (1/50HZ)*1
	kx122_write(0x2A, 0x01);//TLT 1/50 * 1 = 20ms
	kx122_write(0x2B, 0x0A);//TWS 1/50 * 5 = 100ms
#endif
	kx122_write(0x1C, 0x30); //INC1
#ifdef SEISMIC
	kx122_write(0x1E, 0x03); //INC3
#endif

#if defined(TILT) && defined(SEISMIC)
	kx122_write(0x1F, 0x05); //INC4 tilt and tap
#else
#ifdef TILT
    kx122_write(0x1F, 0x01);//INC4 Tilt int
#endif

#ifdef SEISMIC
    kx122_write(0x1F, 0x04);
#endif
#endif

#ifdef TILT
	kx122_write(0x32, 0x0C); //0x18 = tilt angle 45 deg
#endif

#if defined(TILT) && defined(SEISMIC)
	kx122_write(0x18, 0x85); //CNTL1 run tilt and tap
#else
#ifdef TILT
	kx122_write(0x18, 0x81);
#endif

#ifdef SEISMIC
	kx122_write(0x18, 0x84);
#endif
#endif

    d = kx122_read(0x17);

	for(;;)
	{
		//IWDG_ReloadCounter();
		//PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
		//flash_read(0x0800FA00, _flash.buffer, 3);
		//if(_flash.buffer[0] == 0x01/*active == true*/)
		//{
			//active = false;
			//flash_erase(0x0800FA00);
		//}

#ifdef TILT
#ifdef TEST
		ins2 = kx122_read(0x13);
		if((ins2 & 0x01) == 0x01)
		{
			vTaskDelay(500);
			z[1] = kx122_read(0x0B);
			if(z[1] < 63)
			{
				GPIO_SetBits(GPIOB, GPIO_Pin_0);
				/*do
				{
					z[0] = kx122_read(0x0A);
					z[1] = kx122_read(0x0B);
					vTaskDelay(500);
				}
				while(z[1] < 63);*/
				vTaskDelay(500);
				GPIO_ResetBits(GPIOB, GPIO_Pin_0);
			}
			d = kx122_read(0x17);
		}
		vTaskDelay(200);
#else
		/*if(tilt == true)
		{
			GPIO_SetBits(GPIOB, GPIO_Pin_0);
			rn2483_join();
			rn2483_write("mac tx uncnf 10 3f3f3f02f2a40101\r\n", 34);
			GPIO_ResetBits(GPIOB, GPIO_Pin_0);
			vTaskDelay(1000);

		}*/

		//rn2483_write("mac tx uncnf 10 3f3f3f01f2a404ff1a1385\r\n", 40); //healthy
#endif
#endif

#ifdef SEISMIC
#ifdef TEST
		ins2 = kx122_read(0x13);
		if((ins2 & 0x04) == 0x04)
		{
			GPIO_SetBits(GPIOA, GPIO_Pin_7);
			d = kx122_read(0x17);
		}
		vTaskDelay(500);

		GPIO_ResetBits(GPIOA, GPIO_Pin_7);
#else
		if(seismic == true)
		{
			//seismic = false;
			GPIO_SetBits(GPIOA, GPIO_Pin_7);
			//vTaskDelay(500);
			rn2483_join();
			rn2483_write("mac tx uncnf 10 3f3f3f02f2a40102\r\n", 34);
			GPIO_ResetBits(GPIOA, GPIO_Pin_7);
			vTaskDelay(1000);
		}
#endif
#endif

#ifndef TEST
		GPIO_ResetBits(RN2483_RST_PORT, RN2483_RST_PIN);
		vTaskDelay(1000);
		GPIO_SetBits(RN2483_RST_PORT, RN2483_RST_PIN);
		vTaskDelay(1000);
		GPIO_ResetBits(RN2483_RST_PORT, RN2483_RST_PIN);
		vTaskDelay(1000);
		rn2483_sleep();
		PWR_WakeUpPinCmd(PWR_WakeUpPin_1, ENABLE);
		PWR_EnterSTANDBYMode();
#endif
	}
}
