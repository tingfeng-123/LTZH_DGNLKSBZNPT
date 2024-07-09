/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "lcd_init.h"
#include "motor.h"
#include "stdio.h"
#include "hc-sr04.h"
#include "string.h"
#include "dht11.h"
#include "stdarg.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//--------------------------------语音播报
unsigned char Broadcast_PD[5] = {0XAA,0X55,0X03,0X55,0XAA};//坡道
unsigned char Broadcast_LT[5] = {0XAA,0X55,0X09,0X55,0XAA};//楼梯
unsigned char Broadcast_HD[5] = {0XAA,0X55,0X05,0X55,0XAA};//红灯
unsigned char Broadcast_LD[5] = {0XAA,0X55,0X06,0X55,0XAA};//绿灯
unsigned char Broadcast_ZAW[5] = {0XAA,0X55,0X04,0X55,0XAA};//障碍物
unsigned char Broadcast_BMX[5] = {0XAA,0X55,0X08,0X55,0XAA};//斑马线
//unsigned char Broadcast_MD[5] = {0XAA,0X55,0X07,055,0XAA};//盲道
unsigned char Broadcast_GM1[5] = {0XAA,0X55,0X0A,055,0XAA};//当前为白天
unsigned char Broadcast_GM2[5] = {0XAA,0X55,0X0B,055,0XAA};//夜间出行，请注意安全！
unsigned char Broadcast_HW1[5] = {0XAA,0X55,0X0C,055,0XAA};//附近有行人，请慢通行！
unsigned char Broadcast_HW2[5] = {0XAA,0X55,0X0D,055,0XAA};//附近没有行人，请不要超速行驶！
unsigned char Broadcast_YW1[5] = {0XAA,0X55,0X0E,055,0XAA};//当前气体浓度正常，请放心！
unsigned char Broadcast_YW2[5] = {0XAA,0X55,0X0F,055,0XAA};//警告！警告！当前气体浓度异常，请快速离开。
//--------------------------------语音标志位
unsigned char Broadcastflay_YY = 0;//1-语音模式 0-退出语音模式 
unsigned char Broadcastflay_LY = 0;//蓝牙模式
unsigned char Broadcastflay_LH = 1;//领航模式
unsigned char Broadcastflay_JC = 0;
unsigned char Broadcastflay_BZ = 0;//避障
//--------------------------------
unsigned char rx_buf[11];
unsigned char uart_ctrl = 0;
unsigned char temperatureCmd[7] = {0};
unsigned char HumidityCmd[7] = {0};

unsigned int decimalValue; //将接收的角度转为十进制
unsigned char qudong = 1;
unsigned char flay = 0; //判断黄色盲道直线修正、转弯后直线修正+上坡、灰色盲道直线修正
unsigned char flay_choice = 0; //0-置零   1-黄色盲道  2-坡道  3-灰色盲道
unsigned char flay1 = 0; //收到后返回'o',黄色盲道
unsigned char flay2 = 0; //返回'p',坡道
unsigned char flay3 = 0; //返回'b',灰色盲道
unsigned char flay4 = 0; //返回'm',斑马线
unsigned char flay5 = 0; //返回'h',红灯
unsigned char flay6 = 0; //返回'l',绿灯
unsigned char flay7 = 0; //返回't',楼梯
unsigned char flay_ZXD = 0; //转向灯 1-左 2-右
unsigned char flay_ultrasonic = 0; //超声波
unsigned char flay_HD = 0;
unsigned char flay_LD = 0;
unsigned char flay_PD = 0;
unsigned char flay_LT = 0;
unsigned char flay_MD = 0;
unsigned char flay_HMD = 0;
unsigned char flay_ZAW = 0;
unsigned char flay_BMX = 0;
unsigned char flay_STOP = 0;
unsigned char flay_y = 0;
//--------------------------------通信标志位
int pwm1;
int pwm2;
int mn;
unsigned char ff = 0;
unsigned char rr = 0;
unsigned char temp; //将温度整数转为十六进制
unsigned char tempx; //将温度小数转为十六进制
unsigned char Hum; //将湿度整数转为十六进制
unsigned char gogo;
unsigned char stop;
unsigned char left;
unsigned char right;
unsigned char leftz;
unsigned char righty;
unsigned char left_yy;
unsigned char right_yy;
unsigned char backward;
unsigned char hcr;
unsigned char temperature = 1;                     //温度值
unsigned char humidity = 1;
unsigned char temperature_xs = 1;

int fputc(int ch, FILE *f) 
{
  /* Your implementation of fputc(). */
	HAL_UART_Transmit(&huart1,(unsigned char *)&ch,1,50);
  return ch;
}




int fgetc(FILE *f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart6, &ch, 1, 0xffff);
  return ch;
}



void pid_mv() //修正
{
			//----------------------------------------------------黄色盲道
			if(flay_choice == 1) 
			{
				flay = 0;
				if((uart_ctrl >= 0X96 && uart_ctrl <= 0XB4) || (uart_ctrl >= 0X00 && uart_ctrl <= 0X1E))
				{
				//---------------------	
				if(flay_ultrasonic == 0)
				{
				if(decimalValue >= 150 && decimalValue <= 180)
				{
					pwm1 = 150 + (180 - decimalValue);
					pwm2 = 150 - (180 - decimalValue);
					gogo = 1;
				}	
			  if(decimalValue > 0 && decimalValue <= 30)
				{
					pwm1 = 150 - (decimalValue) * 3;
					pwm2 = 150 + (decimalValue) * 3;
					gogo = 2;
				}
			
				}
			//-----------------------
				if(flay_ultrasonic == 1)
				{
					pwm1 = 150;
					pwm2 = 150;
					gogo = 1;
				}
			}
		 }
			
			
			//--------------------------------------------转弯修正加坡道
			if(flay_choice == 2)
			{
				flay = 0;
				if((decimalValue >= 175 && decimalValue <= 180) || (decimalValue >= 0 && decimalValue <= 5))
				{
					flay_choice = 4;
					if(flay_choice == 4)
					{
					if(decimalValue >= 175 && decimalValue <= 180)
					{
						pwm1 = 250 + (180 - decimalValue) * 30;
						pwm2 = 250 - (180 - decimalValue) * 30;
						mn = 420 - (180 - decimalValue);
						gogo = 3;
					}
			
					if(decimalValue >= 0 && decimalValue <= 5)
					{
						pwm1 = 250 - decimalValue * 20;
						pwm2 = 250 + decimalValue * 20;
						mn = 420 - decimalValue;
						gogo = 4;
					}
					}
				}
			
				if(decimalValue >= 160 && decimalValue < 175)
					{
						righty = 1;
						mn = 180 - decimalValue;
					}
			
				if(decimalValue > 5 && decimalValue <= 30)
					{
						leftz = 1;
						mn = decimalValue;
					}
			}
			
			//--------------------------------------------------灰色盲道
			if(flay_choice == 3)
			{
				flay = 0;
				if((uart_ctrl >= 0X78 && uart_ctrl <= 0XB4) || (uart_ctrl >= 0X00 && uart_ctrl <= 0X3C))
				{
					if(decimalValue >= 120 && decimalValue <= 180)
					{
						pwm1 = 150 + (180 - decimalValue);
						pwm2 = 150 - (180 - decimalValue);
						gogo = 1;
					}
					
				
				  if(decimalValue > 0 && decimalValue <= 60)
					{
						pwm1 = 150 - (decimalValue);
						pwm2 = 150 + (decimalValue);
						gogo = 2;
					}
				
				}
			}

			else memset(rx_buf, 0, sizeof(rx_buf)); 
}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) //串口
{
//----------------------------------------------------------MV通信
	if(Broadcastflay_LH == 1)
	{
			if(huart == &huart1)
			{
				uart_ctrl = rx_buf[0];
				decimalValue = uart_ctrl;
				uint8_t receivedChar = (uint8_t)huart->Instance->RDR; // 读取接收到的数据寄存器中的数据
				if(receivedChar == 'v' || receivedChar == 'V')
				{
					// 检查是否已经处理过该字符
					static uint8_t processedV = 0;
					if(processedV == 0)
					{
						processedV = 1;
						flay_choice = 1; //直线修正
						flay_MD = 1;
					}
				}
				if(stop == 0)
				{
					if(receivedChar == 'p' || receivedChar == 'P')
					{
						// 检查是否已经处理过该字符
						static uint8_t processedP = 0;
						if(processedP == 0)
						{
							processedP = 1;
							flay_choice = 2;
							//flay_PD = 1;
						}
					}
				}
				
				if(receivedChar == 'b' || receivedChar == 'B')
				{
					// 检查是否已经处理过该字符
					static uint8_t processedB = 0;
					if(processedB == 0)
					{
						processedB = 1;
						flay_choice = 3;
						flay_HMD = 1;
					}
				}

				if(receivedChar == 'i' || receivedChar == 'I')
				{
					// 检查是否已经处理过该字符
					static uint8_t processedI = 0;
					if(processedI == 0)
					{
						processedI = 1;
						flay_choice = 1;
						flay_HMD = 1;
						//HAL_UART_Transmit_IT(&huart8,Broadcast_MD,5);
					}
				}	
					
				if(receivedChar == 'm' || receivedChar == 'M')
				{
					// 检查是否已经处理过该字符
					static uint8_t processedM = 0;
					if(processedM == 0)
					{
						processedM = 1;
						flay_choice = 1;
						flay_BMX = 1;
					}
				}
				
				if(receivedChar == 'z' || receivedChar == 'Z')
				{
					// 检查是否已经处理过该字符
					static uint8_t processedZ = 0;
					if(processedZ == 0)
					{
						//processedZ = 1;
						left = 1;
						flay_ZXD = 1;
					}
				}
				
				if(receivedChar == 'y' || receivedChar == 'Y')
				{
					// 检查是否已经处理过该字符
					static uint8_t processedY = 0;
					if(processedY == 0)
					{
						//processedY = 1;
						//right = 1;
						flay_ZXD = 2;
						ff++;
						if(ff%2 == 1)
						{
							right = 1;
						}
						else if(ff%2 == 0)
						{
							right = 2;
						}
					}
				}
				

				if(receivedChar == 's' || receivedChar == 'S')
				{
					// 检查是否已经处理过该字符
					static uint8_t processedS = 0;
					if(processedS == 0)
					{
						stop = 1;
						//flay_ZXD = 3;
					}
				}


				if(receivedChar == 'h' || receivedChar == 'H')
				{
					// 检查是否已经处理过该字符
					static uint8_t processedH = 0;
					if(processedH == 0)
					{
						processedH = 1;
						flay_HD = 1;
					}
				}	

				if(receivedChar == 'l' || receivedChar == 'L')
				{
					// 检查是否已经处理过该字符
					static uint8_t processedL = 0;
					if(processedL == 0)
					{
						processedL = 1;
						flay_LD = 1;
					}
				}
				
				if(receivedChar == 'a' || receivedChar == 'A')
				{
					// 检查是否已经处理过该字符
					static uint8_t processedA = 0;
					if(processedA == 0)
					{
						processedA = 1;
						flay_LT = 1;
					}
				}
				if(receivedChar == 'u' || receivedChar == 'U')
				{
					// 检查是否已经处理过该字符
					static uint8_t processedA = 0;
					if(processedA == 0)
					{
						processedA = 1;
						flay_PD = 1;
					}
				}
				if(receivedChar == 'w' || receivedChar == 'W')
				{
					// 检查是否已经处理过该字符
					static uint8_t processedW = 0;
					if(processedW == 0)
					{
						processedW = 1;
						gogo = 6;
						HAL_Delay(5000);
						stop = 1;
						//flay_PD = 1;
					}
				}
				HAL_UART_Receive_IT(&huart1,rx_buf,1);
			}
	}
		//---------------------------------------------------------------------
		if(huart == &huart8) //语音
		{
			uart_ctrl = rx_buf[0];
			temp = temperature;
			tempx = temperature_xs;
			Hum = humidity;			
		switch(uart_ctrl) //8进制，1.2.3.4.5.6.7.8-10.11.12.13.14.15.16.17-.......
		{
			//播报温度
			case 02:
				temperatureCmd[0] = 0xAA;
				temperatureCmd[1] = 0x55;
				temperatureCmd[2] = 0x01;
				temperatureCmd[3] = temp;
				temperatureCmd[4] = tempx;
				temperatureCmd[5] = 0x55;
				temperatureCmd[6] = 0xAA;
				HAL_UART_Transmit_IT(&huart8,temperatureCmd,7);
//			printf("%x,%x,%x,%#x,%x,%x,%x",temperatureCmd[0],temperatureCmd[1],temperatureCmd[2],a,temperatureCmd[4],temperatureCmd[5],temperatureCmd[6]);
			break;
			
			//播报湿度
			case 03:
				HumidityCmd[0] = 0xAA;
				HumidityCmd[1] = 0x55;
				HumidityCmd[2] = 0x02;
				HumidityCmd[3] = Hum;
				HumidityCmd[4] = 0x00;
				HumidityCmd[5] = 0x55;
				HumidityCmd[6] = 0xAA;
				HAL_UART_Transmit_IT(&huart8,HumidityCmd,7);
			break;
			
			//前进
			case 04:
				gogo = 6;
			break;
			
			//向左
			case 05:
				left = 1;
				flay_ZXD = 1;
			break;
			
			//向右
			case 06:
				right = 1;
				flay_ZXD = 2;
			break;
			
			//后退
			case 07:
				backward = 1;
			break;
			
			//停
			case 8:
				stop = 1;
			break;
			
			//语音模式
			case 9:
				Broadcastflay_YY = 1;
			break;
			
			//退出语音模式
			case 0X0A:
				Broadcastflay_YY = 0;
			break;
			
			//蓝牙模式
			case 0X0B:
				Broadcastflay_LY = 1;
			break;
			
			//退出蓝牙模式
			case 0X0C:
				Broadcastflay_LY = 0;
			break;
			
			//领航模式
			case 0X0D:
				Broadcastflay_LH = 1;
			break;
			
			//退出领航模式
			case 0X0E:
				Broadcastflay_LH = 0;
			break;
			
			//开启检测
			case 0X0F:
				Broadcastflay_JC = 1;
			break;
			
			//开启避障
			case 0X10:
				Broadcastflay_BZ = 1;
			break;
		}
		HAL_UART_Receive_IT(&huart8,rx_buf,1);
	}
		

	if(Broadcastflay_LY == 1)
	{
		if(huart == &huart6)
		{
			uart_ctrl = rx_buf[0];
			switch(uart_ctrl)
			{
				case 'w':
					gogo = 7;
				break;
				
				case 's':
					backward = 1;
				break;
				
				case 'a':
					left_yy = 1;
				break;
				
				case 'd':
					right = 1;
				break;
				
				case 't':
					stop = 1;
				break;
			}
			HAL_UART_Receive_IT(&huart6,rx_buf,1);
		}
	}
}




void uart_ever() //通过串口获得的标志位，进行选择
{
		if(flay_MD == 1)
		{
			flay1 = 1;
			flay_MD = 0;
		}
		if(flay_PD == 1)
		{
			HAL_UART_Transmit_IT(&huart8,Broadcast_PD,5);
			flay2 = 1;
			flay_PD = 0;
		}
		if(flay_HMD == 1)
		{
			flay3 = 1;
			flay_HMD = 0;
		}
		if(flay_BMX == 1)
		{
			HAL_UART_Transmit_IT(&huart8,Broadcast_BMX,5);
			flay4 = 1;
			flay_BMX = 0;
		}
		if(flay_ZXD == 1)
		{
			HAL_GPIO_WritePin(LED_LEFT_GPIO_Port, LED_LEFT_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(LED_RIGHT_GPIO_Port, LED_RIGHT_Pin, GPIO_PIN_RESET);
			flay_ZXD = 0;
		}
		if(flay_ZXD == 2)
		{
			HAL_GPIO_WritePin(LED_LEFT_GPIO_Port, LED_LEFT_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED_RIGHT_GPIO_Port, LED_RIGHT_Pin, GPIO_PIN_SET);
			flay_ZXD = 0;
		}
		if(flay_ZXD == 3)
		{
			HAL_GPIO_WritePin(LED_LEFT_GPIO_Port, LED_LEFT_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(LED_RIGHT_GPIO_Port, LED_RIGHT_Pin, GPIO_PIN_RESET);
			flay_ZXD = 0;
		}
		if(flay_HD == 1)
		{
			HAL_UART_Transmit_IT(&huart8,Broadcast_HD,5);
			flay5 = 1;
			flay_HD = 0;
		}
		if(flay_LD == 1)
		{
			HAL_UART_Transmit_IT(&huart8,Broadcast_LD,5);
			flay6 = 1;
			flay_LD = 0;
		}
		if(flay_LT == 1)
		{
			HAL_UART_Transmit_IT(&huart8,Broadcast_LT,5);
			flay7 = 1;
			flay_LT = 0;
		}
	//-----------------------------------------------------
		if(flay1 == 1)
		{
			printf("v");
			flay1 = 0;
		}
		if(flay2 == 1)
		{
			printf("p");
			flay2 = 0;
		}
		if(flay3 == 1)
		{
			printf("b");
			flay3 = 0;
		}
		if(flay4 == 1)
		{
			printf("m");
			flay4 = 0;
		}
		if(flay5 == 1)
		{
			printf("h");
			flay5 = 0;
		}
		if(flay6 == 1)
		{
			printf("l");
			flay6 = 0;
		}
		if(flay7 == 1)
		{
			printf("c");
			flay7 = 0;
		}
}



void hc_bz() //超声波的避障
{
	static uint8_t zaw = 1;
	int count = 0;
	if(HAL_GetTick() - count > 50)//50ms开启一次
	{
		count = HAL_GetTick();
		Trig_signal(); //开启测距;
	}
	if(Read_cm() > 10 && Read_cm() < 45)
	{
		flay_ultrasonic = 1;
	}
  if(Read_cm() >= 45)
	{
		flay_ultrasonic = 0;
    zaw = 0;		
	}
	
	if(Read_cm() < 8)
	{
		if(zaw == 0)
		{
			zaw = 1;
			HAL_UART_Transmit_IT(&huart8,Broadcast_ZAW,5);
			stop = 1;
			printf("t");
		}
	}
	
		if(Broadcastflay_BZ == 1)
		{
			yy_bz();
		}
}




void motor_qd() //标志位版电机驱动
{
		if(gogo == 1)
		{
			MotorControl(0,pwm1,pwm2);
			gogo = 0;
		}
		if(gogo == 2)
		{
			MotorControl(0,pwm1,pwm2);
			gogo = 0;
		}
		if(gogo == 3)
		{
			MotorControl(0,pwm1,pwm2);
			HAL_Delay(mn);
			gogo = 5;
		}
		if(gogo == 4)
		{
			MotorControl(0,pwm1,pwm2);
			HAL_Delay(mn);
			gogo = 5;
		}
		if(gogo == 5)
		{
			MotorControl(0,350,350);
			HAL_Delay(2000);
			MotorControl(0,150,150);
			HAL_Delay(1500);
			MotorControl(0,100,100);
			HAL_Delay(1000);
			MotorControl(0,150,150);
			HAL_Delay(2300);
			stop = 1;
			printf("x");
		}
		if(gogo == 6)
		{
			MotorControl(0,150,150);
			gogo = 0;
		}
		if(gogo == 7)
		{
			MotorControl(0,500,500);
			gogo = 0;
		}
		
		if(stop == 1)
		{
			MotorControl(2, 0, 0);
			//标志位清零
			gogo = 0;
			left = 0;
			right = 0;
			rr = 0;
			backward = 0;
			flay_choice = 0;
			flay_ultrasonic = 2;
			pwm1 = 0;
			pwm2 = 0;
			right_yy = 0;
			left_yy = 0;
			flay_ZXD = 3;
			HAL_Delay(1000);
			printf("t");
			stop = 0;
		}
		if(backward == 1)
		{
			MotorControl(1,250,250);
			backward = 0;
		}
		if(left == 1)
		{
			MotorControl(4,500,500);
			HAL_Delay(550);
			stop = 1;
		}
	
		if(right == 1)
		{
			MotorControl(6,500,500);
			HAL_Delay(550);
			stop = 1;
		}
		if(right == 2)
		{
			MotorControl(6,500,500);
			HAL_Delay(530);
			stop = 1;
			HAL_Delay(100);
			rr = 1;
		}
		if(rr == 1)
		{
			MotorControl(0,150,125);
			HAL_Delay(14500);
			stop = 1;
		}
		
		if(leftz == 1)
		{
			MotorControl(4,220,220);
			leftz = 0;
		}
		if(righty == 1)
		{
			MotorControl(3,220,220);
			righty = 0;
		}
		if(left_yy == 1)
		{
			MotorControl(4,500,500);
			left_yy = 0;
		}
		if(right_yy == 1)
		{
			MotorControl(3,500,500);
			left_yy = 0;
		}
}


void detection_yw()
{
		static float data;
		data = 0;
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1,50);
		HAL_ADC_Stop(&hadc1);
		data = HAL_ADC_GetValue(&hadc1) / 4096 * 3.3;
		printf("%f\r\n",data);
		if(data < 28)
		{
			HAL_UART_Transmit_IT(&huart8,Broadcast_YW1,5);
			HAL_Delay(1000);
			Broadcastflay_JC = 0;
		}
		else if(data > 28)
		{
			HAL_UART_Transmit_IT(&huart8,Broadcast_YW2,5);
			HAL_Delay(1000);
			Broadcastflay_JC = 0;
		}
}

void detection_gm()
{
	HAL_UART_Transmit_IT(&huart8,Broadcast_GM1,5);
	HAL_Delay(1000);
}

void detection_hw()
{
	//HAL_Delay(1000);
	if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_0) == GPIO_PIN_SET)
	{
		HAL_UART_Transmit_IT(&huart8,Broadcast_HW1,5);
		HAL_Delay(1000);
	}
	else 
	{
		HAL_UART_Transmit_IT(&huart8,Broadcast_HW2,5);
		HAL_Delay(1000);
	}
}

void yy_bz()
{
	static int cm;
	int count = 0;
	if(HAL_GetTick() - count > 50)//50ms开启一次
	{
		count = HAL_GetTick();
		Trig_signal(); //开启测距;
	}
	cm = 0;
	if((Read_cm() < 10) && (cm == 0)) //前方有障碍物，先看右30du有没有
	{
		right_yy = 1;
		HAL_Delay(200);
		stop = 1;
		if(Read_cm() >= 10)
		{
			cm = 1;
		}
		else cm = 0;
	}
	if(cm == 1)
	{
		gogo = 6;
		HAL_Delay(500);
		stop = 1;
		if(Read_cm() >= 10)
		{
			left_yy = 1;
			HAL_Delay(200);
			stop = 1;
			if(Read_cm() >= 10)
			{
				cm = 2;
			}
			else cm = 0;
		}
	}
	if(cm == 2)
	{
		left_yy = 1;
		HAL_Delay(200);
		if(Read_cm() >= 10)
		{
			cm = 3;
		}
		else cm = 0;
	}
	if(cm == 3)
	{
		right_yy = 1;
		HAL_Delay(200);
		stop = 1;
		if(Read_cm() >= 10)
		{
			Broadcastflay_BZ = 0;
		}
		else cm = 0;
	}
}
/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_USART6_UART_Init();
  MX_TIM1_Init();
  MX_UART8_Init();
  MX_ADC1_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
	HAL_Delay(1000);
	HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
	HAL_UART_Receive_IT(&huart6,rx_buf,1);
	HAL_UART_Receive_IT(&huart1,rx_buf,1);
	HAL_UART_Receive_IT(&huart8,rx_buf,1);
	DHT11_Rst();
	LCD_Init();
	LCD_Fill(0,0,128,160,WHITE);
	LCD_ShowChinese(10, 40, 2, RED, WHITE, 24);
	LCD_ShowChinese(34, 40, 3, RED, WHITE, 24);
	LCD_ShowChinese(59, 40, 4, RED, WHITE, 24);
	LCD_ShowChinese(82, 40, 5, RED, WHITE, 24);
	
	LCD_ShowChinese(1, 74, 14, RED, WHITE, 16);
	LCD_ShowChinese(17, 74, 4, RED, WHITE, 16);
	LCD_ShowChinese(33, 74, 5, RED, WHITE, 16);
	LCD_ShowChinese(49, 74, 6, RED, WHITE, 16);
	LCD_ShowChinese(65, 74, 7, RED, WHITE, 16);
	LCD_ShowChinese(81, 74, 8, RED, WHITE, 16);
	LCD_ShowChinese(97, 74, 9, RED, WHITE, 16);
	LCD_ShowChinese(33, 90, 10, RED, WHITE, 16);
	LCD_ShowChinese(49, 90, 15, RED, WHITE, 16);
	LCD_ShowChinese(65, 90, 16, RED, WHITE, 16);
	LCD_ShowChinese(81, 90, 11, RED, WHITE, 16);
	LCD_ShowChinese(97, 90, 12, RED, WHITE, 16);
  
//  while(DHT11_Check())                           //检测DHT11连接
//  {
//	  HAL_UART_Transmit(&huart1, CntState, strlen(CntState), 200);
//	  HAL_Delay(500);
//  }
//  CntState = "Success!\r\n";
//  HAL_UART_Transmit(&huart1, CntState, strlen(CntState), 200);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//		if(Broadcastflay_YY == 1)
//		{
////--------------------------------------红外
//			detection_hw();
////--------------------------------------光敏
//			detection_gm();
////--------------------------------------烟雾
//			detection_yw();
//		}
//--------------------------------------温湿度
		DHT11_Read_Data(&humidity,&temperature,&temperature_xs);		
//--------------------------------------交互
		uart_ever(); //标志位变动
//----------------------------------------超声波
		if(Broadcastflay_BZ == 0)
		{
			hc_bz();
		}
//--------------------------------------超声波避障
//		if(Broadcastflay_BZ == 1)
//		{
//			yy_bz();
//		}
//----------------------------------------电机驱动
		motor_qd();
//----------------------------------------修正
		pid_mv();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 9;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOMEDIUM;
  RCC_OscInitStruct.PLL.PLLFRACN = 3072;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
