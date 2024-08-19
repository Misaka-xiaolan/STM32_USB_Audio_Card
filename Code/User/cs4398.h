#ifndef __CS4398_H
#define __CS4398_H
#include "main.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//CS4398 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/24
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
 
 
#define CS4398_ADDR				0X98	//CS4398的器件地址,固定为0X4C，但是根据STM32的规则，应为0x98
 
#define EQ1_80Hz		0X00
#define EQ1_105Hz		0X01
#define EQ1_135Hz		0X02
#define EQ1_175Hz		0X03

#define EQ2_230Hz		0X00
#define EQ2_300Hz		0X01
#define EQ2_385Hz		0X02
#define EQ2_500Hz		0X03

#define EQ3_650Hz		0X00
#define EQ3_850Hz		0X01
#define EQ3_1100Hz		0X02
#define EQ3_14000Hz		0X03

#define EQ4_1800Hz		0X00
#define EQ4_2400Hz		0X01
#define EQ4_3200Hz		0X02
#define EQ4_4100Hz		0X03

#define EQ5_5300Hz		0X00
#define EQ5_6900Hz		0X01
#define EQ5_9000Hz		0X02
#define EQ5_11700Hz		0X03
#define	DAC_RST_PORT	GPIOA
#define	DAC_RST_PIN		GPIO_PIN_5

extern int g_volume;

void CS4398_GPIO_Init(void);
void CS4398_RST(uint8_t div);
void CS4398_RESET(void);
uint8_t CS4398_Init(void); 
void CS4398_Power_Set(uint8_t power,uint8_t div);
//CS4398 DSD配置
void CS4398_DSD_Cfg(uint8_t staticd,uint8_t invalid,uint8_t pmmode,uint8_t pmen);
void CS4398_I2S_Cfg(uint8_t dif,uint8_t de,uint8_t fm);
void CS4398_Mute_Set(uint8_t pamute,uint8_t damute,uint8_t mutec,uint8_t mutea,uint8_t muteb);//静音控制
void CS4398_HPvol_Set(uint8_t vol);
uint8_t CS4398_Read_Reg(uint8_t reg);
uint8_t CS4398_Write_Reg(uint8_t reg,uint8_t val);
void CS4398_Cross_Set(uint8_t szc,uint8_t filt,uint8_t dir);
void CS4398_Channel_Cfg(uint8_t volba,uint8_t ata);
#endif





















