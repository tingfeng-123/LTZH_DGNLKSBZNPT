/*
 * @Author       : yzy
 * @Date         : 2021-05-31 17:03:27
 * @LastEditors  : yzy
 * @LastEditTime : 2021-05-31 19:02:54
 * @Description  : 
 * @FilePath     : \F103_Test\BSP_HARDWARE\hc-sr04.h
 */
#ifndef HCSR04_H_
#define HCSR04_H_
#include "main.h"
#include "stdio.h"

#define		Trig_ON 		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_SET)
#define		Trig_OFF 		HAL_GPIO_WritePin(GPIOA,GPIO_PIN_4,GPIO_PIN_RESET)

void Dlay_us(uint16_t us);
void Trig_signal(void);
float Read_cm(void);

#endif /* HCSR04_H_ */


