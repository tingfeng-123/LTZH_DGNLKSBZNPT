#ifndef __LCD_INIT_H
#define __LCD_INIT_H
 
#include "main.h"
#define  u8  uint8_t
#define  u16 uint16_t
 
#define USE_HORIZONTAL 1  //设置横屏或者竖屏显示 0或1为竖屏 2或3为横屏
                          //0,1,2,3分别控制4个不同的方向显示
 
 
#if USE_HORIZONTAL==0||USE_HORIZONTAL==1
#define LCD_W 128
#define LCD_H 160
 
#else
#define LCD_W 160
#define LCD_H 128
#endif
 
 
//-----------------LCD端口定义---------------- 
 
#define LCD_SCLK_Clr() HAL_GPIO_WritePin(LCD_SCLK_GPIO_Port,LCD_SCLK_Pin,GPIO_PIN_RESET)//SCLK
#define LCD_SCLK_Set() HAL_GPIO_WritePin(LCD_SCLK_GPIO_Port,LCD_SCLK_Pin,GPIO_PIN_SET)
 
#define LCD_MOSI_Clr() HAL_GPIO_WritePin(LCD_MOSI_GPIO_Port,LCD_MOSI_Pin,GPIO_PIN_RESET)//MOSI
#define LCD_MOSI_Set() HAL_GPIO_WritePin(LCD_MOSI_GPIO_Port,LCD_MOSI_Pin,GPIO_PIN_SET)
 
#define LCD_RES_Clr()  HAL_GPIO_WritePin(LCD_RES_GPIO_Port,LCD_RES_Pin,GPIO_PIN_RESET)//RES
#define LCD_RES_Set()  HAL_GPIO_WritePin(LCD_RES_GPIO_Port,LCD_RES_Pin,GPIO_PIN_SET)
 
#define LCD_DC_Clr()   HAL_GPIO_WritePin(LCD_DC_GPIO_Port,LCD_DC_Pin,GPIO_PIN_RESET)//DC
#define LCD_DC_Set()   HAL_GPIO_WritePin(LCD_DC_GPIO_Port,LCD_DC_Pin,GPIO_PIN_SET)
              
#define LCD_CS_Clr()   HAL_GPIO_WritePin(LCD_CS_GPIO_Port,LCD_CS_Pin,GPIO_PIN_RESET)//CS
#define LCD_CS_Set()   HAL_GPIO_WritePin(LCD_CS_GPIO_Port,LCD_CS_Pin,GPIO_PIN_SET)
 
#define LCD_BLK_Clr()  HAL_GPIO_WritePin(LCD_BLK_GPIO_Port,LCD_BLK_Pin,GPIO_PIN_RESET)//BLK
#define LCD_BLK_Set()  HAL_GPIO_WritePin(LCD_BLK_GPIO_Port,LCD_BLK_Pin,GPIO_PIN_SET)
 
 
 
void LCD_GPIO_Init(void);//初始化GPIO
void LCD_Writ_Bus(u8 dat);//模拟SPI时序
void LCD_WR_DATA8(u8 dat);//写入一个字节
void LCD_WR_DATA(u16 dat);//写入两个字节
void LCD_WR_REG(u8 dat);//写入一个指令
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2);//设置坐标函数
void LCD_Init(void);//LCD初始化
#endif

