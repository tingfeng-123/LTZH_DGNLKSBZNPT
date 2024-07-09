/*
 * motor.c
 *
 *  Created on: Apr 4, 2023
 *      Author: 77454
 */
#include "motor.h"
/**
 *    @brief 控制电机进行正转、反转、停止
 *    @param None
 *    @retval None
 */
void LeftMotor_Go() //左电机正转 AIN输出相反电平  BIN也输出相反电平
{
	HAL_GPIO_WritePin(AIN1_GPIO_Port, AIN1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(AIN2_GPIO_Port, AIN2_Pin, GPIO_PIN_RESET);
}
void LeftMotor_Back()  //左电机反转
{
	HAL_GPIO_WritePin(AIN1_GPIO_Port, AIN1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(AIN2_GPIO_Port, AIN2_Pin, GPIO_PIN_SET);
}
void LeftMotor_Stop()  //左电机停止 AIN和BIN输出相同电平
{
	HAL_GPIO_WritePin(AIN1_GPIO_Port, AIN1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(AIN2_GPIO_Port, AIN2_Pin, GPIO_PIN_RESET);
}
void RightMotor_Go() //右电机正转 AIN输出相反电平  BIN也输出相反电平
{
	HAL_GPIO_WritePin(BIN1_GPIO_Port, BIN1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(BIN2_GPIO_Port, BIN2_Pin, GPIO_PIN_RESET);
}
void RightMotor_Back()  //右电机反转
{
	HAL_GPIO_WritePin(BIN1_GPIO_Port, BIN1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BIN2_GPIO_Port, BIN2_Pin, GPIO_PIN_SET);
}
void RightMotor_Stop()  //右电机停止 AIN和BIN输出相同电平
{
	HAL_GPIO_WritePin(BIN1_GPIO_Port, BIN1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BIN2_GPIO_Port, BIN2_Pin, GPIO_PIN_RESET);
}

/**
 *    @brief 控制电机进行速度控制
 *    @param 运动方向，左右电机的PWM值
 *    @retval None
 */
void MotorControl(char motorDirection, int leftMotorPWM, int rightMotorPWM) {
	switch (motorDirection) {
	case 0:
		//前进
		LeftMotor_Go();
		RightMotor_Go();
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, leftMotorPWM);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, rightMotorPWM);
		break;
	case 1:
		//后退
		LeftMotor_Back();
		RightMotor_Back();//midfiy
	   //RightMotor_Go();
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, leftMotorPWM);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, rightMotorPWM);
		break;
	case 2:
		//停
		LeftMotor_Stop();
		RightMotor_Stop();
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
		break;
	case 3:
		//右转
		LeftMotor_Go();
		//LeftMotor_Back();
		RightMotor_Back();//midfiy
	   //RightMotor_Go();
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, leftMotorPWM);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, rightMotorPWM);
		break;
	case 4:
		//左转
		LeftMotor_Back();
		//RightMotor_Back();//midfiy
	   RightMotor_Go();
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, leftMotorPWM);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, rightMotorPWM);
		break;
	case 5:
		LeftMotor_Back();
		RightMotor_Go();
	  //RightMotor_Back();
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, leftMotorPWM);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, rightMotorPWM);
		break;
	case 6:
		LeftMotor_Go();
	//LeftMotor_Back();
		//RightMotor_Go();
	RightMotor_Back();
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, leftMotorPWM);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, rightMotorPWM);
		break;
	default:
		break;
	}
}
