#include "hc-sr04.h"

float distance_cm = 0; //距离cm

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim4;

//延时
void Dlay_us(uint16_t us)
{
	uint16_t time = 0xffff - us - 5;
	__HAL_TIM_SET_COUNTER(&htim4,time); //初始化time
	HAL_TIM_Base_Start(&htim4);
	
	while(time < 0xffff - 5)
	{
		time = __HAL_TIM_GET_COUNTER(&htim4);		
	}
	
	HAL_TIM_Base_Stop(&htim4);
}

//触发
void Trig_signal(void)
{
	Trig_ON;
	Dlay_us(10);
	Trig_OFF;
}


//中断回调
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	static uint32_t time_us = 0;
	
	if(GPIO_Pin == GPIO_PIN_5)
	{
		if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5))
		{
			HAL_TIM_Base_Start(&htim2);
			__HAL_TIM_SetCounter(&htim2,0);
		}
		else if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_5) == 0)
		{
			HAL_TIM_Base_Start(&htim2);
			time_us = __HAL_TIM_GetCounter(&htim2);
			distance_cm = time_us * 340 / 2 * 0.000001 * 100;
			//printf("distance_cm == %f cm\r\n",distance_cm);
			time_us = 0;
		}
	}
}

float Read_cm(void)
{
	return distance_cm;
}

