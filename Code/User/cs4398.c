#include "CS4398.h"
#include "i2c.h"
#include "math.h"

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

//CS4398寄存器值缓存区(总共58个寄存器,0~57),占用116字节内存
//因为CS4398的IIC操作不支持读操作,所以在本地保存所有寄存器值
//写CS4398寄存器时,同步更新到本地寄存器值,读寄存器时,直接返回本地保存的寄存器值.
//注意:CS4398的寄存器值是9位的,所以要用uint16_t来存储. 

//CS4398初始化
//返回值:0,初始化正常
//    其他,错误代码
int g_volume=44;
void CS4398_GPIO_Init(void)
{
	__HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_Init = {0};
	GPIO_Init.Pin = DAC_RST_PIN;
	GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
	//GPIO_KEY_Init.Pull = GPIO_PULLUP;
	GPIO_Init.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(DAC_RST_PORT, &GPIO_Init);
}

void CS4398_RESET(void)
{
	HAL_GPIO_WritePin(DAC_RST_PORT, DAC_RST_PIN, 0);
	HAL_Delay(10);
	HAL_GPIO_WritePin(DAC_RST_PORT, DAC_RST_PIN, 1);	
}
void CS4398_RST(uint8_t div)
{
	HAL_GPIO_WritePin(DAC_RST_PORT, DAC_RST_PIN, 0);
	HAL_Delay(10);
	HAL_GPIO_WritePin(DAC_RST_PORT, DAC_RST_PIN, 1);
	CS4398_Power_Set(0,div);//上电
	CS4398_Channel_Cfg(1,1);
	CS4398_Cross_Set(3,1,0);//慢速滤波（内部低通滤波）
	CS4398_HPvol_Set(g_volume);//声道音量0
}
uint8_t CS4398_Init(void)
{
	uint8_t ID=0;
//	RCC->AHB1ENR|=1<<0|1<<1|1<<2;			//使能外设PORT时钟
//	GPIO_Set(GPIOA,PIN2,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//DAC_RST
// 	GPIO_Set(GPIOA,PIN4|PIN5|PIN7,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//I2S1 LRCK,I2S1 SCLK,I2S1 SDIN
//	GPIO_Set(GPIOC,PIN4,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PC6复用功能输出I2S1 MCLK
//	GPIO_Set(GPIOA,PIN1,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//I2S4 SDIN
	
	//GPIO_Set(GPIOB,PIN13,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//I2S4 SCK
	
	//GPIO_AF_Set(GPIOB,13,6);   //I2S4 SCK
	
//	GPIO_AF_Set(GPIOA,1,5);   //I2S4 SDIN
//	
//	GPIO_AF_Set(GPIOA,4,5);	//PA4,AF5  I2S1_LRCK
//	GPIO_AF_Set(GPIOA,5,5);	//PA5,AF5  I2S1_SCLK 
//	GPIO_AF_Set(GPIOA,7,5);	//PA7 ,AF5  I2S1_DACDATA 
//	GPIO_AF_Set(GPIOC,4,5);		//PC4 ,AF5  I2S1_MCK	
// 
//	IIC2_Init();//初始化IIC接口
	CS4398_RST(0);//复位芯片
	ID=CS4398_Read_Reg(1);//读取ID=0x72
	//以下为通用设置
	//CS4398_I2S_Cfg(1,2,0);//I2S模式
  CS4398_Cross_Set(3,1,0);//慢速滤波（内部低通滤波）
	CS4398_Mute_Set(0,0,0,0,0);
  CS4398_Channel_Cfg(1,1);
	return ID;
} 
//CS4398写寄存器
//reg:寄存器地址
//val:要写入寄存器的值 
//返回值:0,成功;
//    其他,错误代码
uint8_t CS4398_Write_Reg(uint8_t reg,uint8_t val)
{
	uint8_t sendbuf[2] = {0};
	sendbuf[0] = reg;
	sendbuf[1] = val;
	HAL_I2C_Master_Transmit(&hi2c2, CS4398_ADDR, sendbuf, 2, HAL_MAX_DELAY);
	return 0;	
}  
//CS4398读寄存器
//就是读取本地寄存器值缓冲区内的对应值
//reg:寄存器地址 
//返回值:寄存器值 
uint8_t CS4398_Read_Reg(uint8_t reg)
{				  
	uint8_t temp=0;
	HAL_I2C_Master_Transmit(&hi2c2, CS4398_ADDR, &reg, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c2, CS4398_ADDR, &temp, 1, HAL_MAX_DELAY);
	return temp;
}
//设置I2S工作模式

//DIF:PCM模式下0,LSB(左对齐);1:飞利浦标准I2S2 24bit    2-5:MSB(右对齐);
//DSD模式下：0，DSD64+4xMCLK；1，DSD64+6xMCLK；2，DSD64+8xMCLK；3，DSD64+12xMCLK；
//4，DSD128+2xMCLK；5，DSD128+3xMCLK；6，DSD128+4xMCLK；7，DSD128+6xMCLK；
//DE：1，44.1khz去加重；2，48khz去加重；3，32khz去加重；
//FM：0，30-50khz采用率；1，50-100khz采用率；2，100-200khz采用率，3，DSD模式
void CS4398_I2S_Cfg(uint8_t dif,uint8_t de,uint8_t fm)
{
	dif&=0X07;//限定范围
	CS4398_Write_Reg(0x02,(dif<<4)|(de<<2)|fm);	//R3,CS4398工作模式设置	
}	
//VOLBA：0左右音量单独控制，1一起控制
//ATA：混音，默认0x09立体声；
//0全部静音，1左静音，2右静音
void CS4398_Channel_Cfg(uint8_t volba,uint8_t ata)
{
	if(ata==0)
	  CS4398_Write_Reg(0x03,volba<<7|0);	//
	else
		CS4398_Write_Reg(0x03,volba<<7|0x09);	//
}
//CS4398 DSD配置
//pmen：DSD相位调制使能，pmmode=0，128fs相位调制，=1 64fs相位调制
void CS4398_DSD_Cfg(uint8_t staticd,uint8_t invalid,uint8_t pmmode,uint8_t pmen)
{
	CS4398_Write_Reg(0x09,staticd<<3|invalid<<2|pmmode<<1|pmen);
}
//电源设置 0:开机；1:掉电
//div:主MCLK  2分频设置
void CS4398_Power_Set(uint8_t power,uint8_t div)
{
	CS4398_Write_Reg(0x08,power<<7|1<<6|div<<4);	
  if(power==1)
    HAL_GPIO_WritePin(DAC_RST_PORT, DAC_RST_PIN, 0);		
}
//pamute=1:pcm数字静音
//damute=1：dsd数字静音
//mutec=1：AB声道同时静音
//mutea=0:正常  =1：A通道静音
//muteb=0:正常  =1：B通道静音
//mutep：静音极性检测
void CS4398_Mute_Set(uint8_t pamute,uint8_t damute,uint8_t mutec,uint8_t mutea,uint8_t muteb)//静音控制
{ 
	uint8_t reg=0;
	reg|=pamute<<7|damute<<6|mutec<<5|mutea<<4|mutea<<3|muteb<<0;
 	CS4398_Write_Reg(0x04,reg);	
}
//设置耳机左右声道音量
//voll:左声道音量(0~100)
//volr:右声道音量(0~100)
void CS4398_HPvol_Set(uint8_t vol)
{
	float voll=0;
	if(vol>100)vol=100;//限定范围
	voll=vol;
	voll=sqrt(voll)*25.5;
	voll=255-voll;
	CS4398_Write_Reg(0x05,voll);			//R6,耳机左声道音量设置
	CS4398_Write_Reg(0x06,voll);	//R7,耳机右声道音量设置,同步更新(HPVU=1)
}
//零点检测+滤波
//szc=0:快速变化，=1：零点检测 =2：软件平滑=3：软件平衡+零点检测
//filt=0:内部滤波器快速滤波；=1慢速滤波
//dir=0：内部正常解码流程可控制音量和滤波 =1：直解码音量不可以控制
void CS4398_Cross_Set(uint8_t szc,uint8_t filt,uint8_t dir)
{
	CS4398_Write_Reg(0x07,szc<<6|2<<4|filt<<2|dir<<0);
}










