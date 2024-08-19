#include "CS4398.h"
#include "i2c.h"
#include "math.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//CS4398 ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/24
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

//CS4398�Ĵ���ֵ������(�ܹ�58���Ĵ���,0~57),ռ��116�ֽ��ڴ�
//��ΪCS4398��IIC������֧�ֶ�����,�����ڱ��ر������мĴ���ֵ
//дCS4398�Ĵ���ʱ,ͬ�����µ����ؼĴ���ֵ,���Ĵ���ʱ,ֱ�ӷ��ر��ر���ļĴ���ֵ.
//ע��:CS4398�ļĴ���ֵ��9λ��,����Ҫ��uint16_t���洢. 

//CS4398��ʼ��
//����ֵ:0,��ʼ������
//    ����,�������
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
	CS4398_Power_Set(0,div);//�ϵ�
	CS4398_Channel_Cfg(1,1);
	CS4398_Cross_Set(3,1,0);//�����˲����ڲ���ͨ�˲���
	CS4398_HPvol_Set(g_volume);//��������0
}
uint8_t CS4398_Init(void)
{
	uint8_t ID=0;
//	RCC->AHB1ENR|=1<<0|1<<1|1<<2;			//ʹ������PORTʱ��
//	GPIO_Set(GPIOA,PIN2,GPIO_MODE_OUT,GPIO_OTYPE_PP,GPIO_SPEED_50M,GPIO_PUPD_PU);//DAC_RST
// 	GPIO_Set(GPIOA,PIN4|PIN5|PIN7,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//I2S1 LRCK,I2S1 SCLK,I2S1 SDIN
//	GPIO_Set(GPIOC,PIN4,GPIO_MODE_AF,GPIO_OTYPE_PP,GPIO_SPEED_100M,GPIO_PUPD_PU);	//PC6���ù������I2S1 MCLK
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
//	IIC2_Init();//��ʼ��IIC�ӿ�
	CS4398_RST(0);//��λоƬ
	ID=CS4398_Read_Reg(1);//��ȡID=0x72
	//����Ϊͨ������
	//CS4398_I2S_Cfg(1,2,0);//I2Sģʽ
  CS4398_Cross_Set(3,1,0);//�����˲����ڲ���ͨ�˲���
	CS4398_Mute_Set(0,0,0,0,0);
  CS4398_Channel_Cfg(1,1);
	return ID;
} 
//CS4398д�Ĵ���
//reg:�Ĵ�����ַ
//val:Ҫд��Ĵ�����ֵ 
//����ֵ:0,�ɹ�;
//    ����,�������
uint8_t CS4398_Write_Reg(uint8_t reg,uint8_t val)
{
	uint8_t sendbuf[2] = {0};
	sendbuf[0] = reg;
	sendbuf[1] = val;
	HAL_I2C_Master_Transmit(&hi2c2, CS4398_ADDR, sendbuf, 2, HAL_MAX_DELAY);
	return 0;	
}  
//CS4398���Ĵ���
//���Ƕ�ȡ���ؼĴ���ֵ�������ڵĶ�Ӧֵ
//reg:�Ĵ�����ַ 
//����ֵ:�Ĵ���ֵ 
uint8_t CS4398_Read_Reg(uint8_t reg)
{				  
	uint8_t temp=0;
	HAL_I2C_Master_Transmit(&hi2c2, CS4398_ADDR, &reg, 1, HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c2, CS4398_ADDR, &temp, 1, HAL_MAX_DELAY);
	return temp;
}
//����I2S����ģʽ

//DIF:PCMģʽ��0,LSB(�����);1:�����ֱ�׼I2S2 24bit    2-5:MSB(�Ҷ���);
//DSDģʽ�£�0��DSD64+4xMCLK��1��DSD64+6xMCLK��2��DSD64+8xMCLK��3��DSD64+12xMCLK��
//4��DSD128+2xMCLK��5��DSD128+3xMCLK��6��DSD128+4xMCLK��7��DSD128+6xMCLK��
//DE��1��44.1khzȥ���أ�2��48khzȥ���أ�3��32khzȥ���أ�
//FM��0��30-50khz�����ʣ�1��50-100khz�����ʣ�2��100-200khz�����ʣ�3��DSDģʽ
void CS4398_I2S_Cfg(uint8_t dif,uint8_t de,uint8_t fm)
{
	dif&=0X07;//�޶���Χ
	CS4398_Write_Reg(0x02,(dif<<4)|(de<<2)|fm);	//R3,CS4398����ģʽ����	
}	
//VOLBA��0���������������ƣ�1һ�����
//ATA��������Ĭ��0x09��������
//0ȫ��������1������2�Ҿ���
void CS4398_Channel_Cfg(uint8_t volba,uint8_t ata)
{
	if(ata==0)
	  CS4398_Write_Reg(0x03,volba<<7|0);	//
	else
		CS4398_Write_Reg(0x03,volba<<7|0x09);	//
}
//CS4398 DSD����
//pmen��DSD��λ����ʹ�ܣ�pmmode=0��128fs��λ���ƣ�=1 64fs��λ����
void CS4398_DSD_Cfg(uint8_t staticd,uint8_t invalid,uint8_t pmmode,uint8_t pmen)
{
	CS4398_Write_Reg(0x09,staticd<<3|invalid<<2|pmmode<<1|pmen);
}
//��Դ���� 0:������1:����
//div:��MCLK  2��Ƶ����
void CS4398_Power_Set(uint8_t power,uint8_t div)
{
	CS4398_Write_Reg(0x08,power<<7|1<<6|div<<4);	
  if(power==1)
    HAL_GPIO_WritePin(DAC_RST_PORT, DAC_RST_PIN, 0);		
}
//pamute=1:pcm���־���
//damute=1��dsd���־���
//mutec=1��AB����ͬʱ����
//mutea=0:����  =1��Aͨ������
//muteb=0:����  =1��Bͨ������
//mutep���������Լ��
void CS4398_Mute_Set(uint8_t pamute,uint8_t damute,uint8_t mutec,uint8_t mutea,uint8_t muteb)//��������
{ 
	uint8_t reg=0;
	reg|=pamute<<7|damute<<6|mutec<<5|mutea<<4|mutea<<3|muteb<<0;
 	CS4398_Write_Reg(0x04,reg);	
}
//���ö���������������
//voll:����������(0~100)
//volr:����������(0~100)
void CS4398_HPvol_Set(uint8_t vol)
{
	float voll=0;
	if(vol>100)vol=100;//�޶���Χ
	voll=vol;
	voll=sqrt(voll)*25.5;
	voll=255-voll;
	CS4398_Write_Reg(0x05,voll);			//R6,������������������
	CS4398_Write_Reg(0x06,voll);	//R7,������������������,ͬ������(HPVU=1)
}
//�����+�˲�
//szc=0:���ٱ仯��=1������� =2�����ƽ��=3�����ƽ��+�����
//filt=0:�ڲ��˲��������˲���=1�����˲�
//dir=0���ڲ������������̿ɿ����������˲� =1��ֱ�������������Կ���
void CS4398_Cross_Set(uint8_t szc,uint8_t filt,uint8_t dir)
{
	CS4398_Write_Reg(0x07,szc<<6|2<<4|filt<<2|dir<<0);
}










