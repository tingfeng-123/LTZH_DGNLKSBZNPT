#ifndef _DH11_H_
#define _DH11_H_
 
#include"main.h"

#define DHT11_ON		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET)
#define DHT11_OFF		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET)

void Delay_us(uint16_t delay);
void Dht11_DATA_OUT(void);
void Dht11_DATA_IN(void);
void DHT11_Rst(void);
uint8_t DHT11_Check(void);
uint8_t DHT11_Read_Bit(void);
uint8_t DHT11_Read_Byte(void);
uint8_t DHT11_Read_Data(uint8_t* humi,uint8_t* temp,uint8_t* temp_xs);



#endif