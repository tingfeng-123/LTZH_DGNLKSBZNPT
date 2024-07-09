/*
 * motor.c
 *
 *  Created on: Apr 4, 2023
 *      Author: 77454
 */
#include "motor.h"
/**
 *    @brief ���Ƶ��������ת����ת��ֹͣ
 *    @param None
 *    @retval None
 */
void LeftMotor_Go() //������ת AIN����෴��ƽ  BINҲ����෴��ƽ
{
	HAL_GPIO_WritePin(AIN1_GPIO_Port, AIN1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(AIN2_GPIO_Port, AIN2_Pin, GPIO_PIN_RESET);
}
void LeftMotor_Back()  //������ת
{
	HAL_GPIO_WritePin(AIN1_GPIO_Port, AIN1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(AIN2_GPIO_Port, AIN2_Pin, GPIO_PIN_SET);
}
void LeftMotor_Stop()  //����ֹͣ AIN��BIN�����ͬ��ƽ
{
	HAL_GPIO_WritePin(AIN1_GPIO_Port, AIN1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(AIN2_GPIO_Port, AIN2_Pin, GPIO_PIN_RESET);
}
void RightMotor_Go() //�ҵ����ת AIN����෴��ƽ  BINҲ����෴��ƽ
{
	HAL_GPIO_WritePin(BIN1_GPIO_Port, BIN1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(BIN2_GPIO_Port, BIN2_Pin, GPIO_PIN_RESET);
}
void RightMotor_Back()  //�ҵ����ת
{
	HAL_GPIO_WritePin(BIN1_GPIO_Port, BIN1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BIN2_GPIO_Port, BIN2_Pin, GPIO_PIN_SET);
}
void RightMotor_Stop()  //�ҵ��ֹͣ AIN��BIN�����ͬ��ƽ
{
	HAL_GPIO_WritePin(BIN1_GPIO_Port, BIN1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(BIN2_GPIO_Port, BIN2_Pin, GPIO_PIN_RESET);
}

/**
 *    @brief ���Ƶ�������ٶȿ���
 *    @param �˶��������ҵ����PWMֵ
 *    @retval None
 */
void MotorControl(char motorDirection, int leftMotorPWM, int rightMotorPWM) {
	switch (motorDirection) {
	case 0:
		//ǰ��
		LeftMotor_Go();
		RightMotor_Go();
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, leftMotorPWM);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, rightMotorPWM);
		break;
	case 1:
		//����
		LeftMotor_Back();
		RightMotor_Back();//midfiy
	   //RightMotor_Go();
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, leftMotorPWM);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, rightMotorPWM);
		break;
	case 2:
		//ͣ
		LeftMotor_Stop();
		RightMotor_Stop();
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 0);
		break;
	case 3:
		//��ת
		LeftMotor_Go();
		//LeftMotor_Back();
		RightMotor_Back();//midfiy
	   //RightMotor_Go();
		__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, leftMotorPWM);
		__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, rightMotorPWM);
		break;
	case 4:
		//��ת
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
