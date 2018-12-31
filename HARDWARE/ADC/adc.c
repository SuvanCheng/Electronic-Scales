#include "adc.h"
#include "delay.h"
#include "key.h"
#include "led.h"
#include "schedule.h"
#include "oled.h"
	   
//初始化ADC
//这里我们仅以规则通道为例
//我们默认将开启通道0~3																	   

extern float adcx;  //接收AD;
extern float adcx1;
extern float adcx2;
extern char is_Ready;
extern float weight;//重量
extern int fur_mode; //去皮标志位 1:去皮 0:不去皮
extern float unit_price , price , total_price ; //单价(元每克) 价格(元) 总价(元)
extern int number ;//称重次数
int a;

void  Adc_Init(void)
{ 	
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1	, ENABLE );	  //使能ADC1通道时钟
 

	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6 72M/6=12,ADC最大时间不能超过14M

	//PA1 作为模拟通道输入引脚                         
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//模拟输入引脚
	GPIO_Init(GPIOA, &GPIO_InitStructure);	

	ADC_DeInit(ADC1);  //复位ADC1 

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;	//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);	//根据ADC_InitStruct中指定的参数初始化外设ADCx的寄存器   

  
	ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准  
	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束
	
	ADC_StartCalibration(ADC1);	 //开启AD校准
 
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
 
//	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能

}				  
//获得ADC值
//ch:通道值 0~3
u16 Get_Adc(u8 ch)   
{
  	//设置指定ADC的规则组通道，一个序列，采样时间
	ADC_RegularChannelConfig(ADC1, ch, 0, ADC_SampleTime_239Cycles5 );	//ADC1,ADC通道,采样时间为239.5周期	  			    
  
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);		//使能指定的ADC1的软件转换启动功能	
	 
	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));//等待转换结束

	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}

u16 Get_Adc_Average(u8 ch,u8 times)
{
	u32 temp_val=0;
	u8 t;
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Adc(ch);
		delay_ms(5);
	}
	return temp_val/times;
} 	 



void D_mode1(void)//去皮
{
		while(!is_Ready)
		{
//			for(a=0;a<number;a++)
//			{
				LED2=1; 
				LED3=1;
				while(KEY_Scan(0)!= S3_KEY3_PRES)//按S3确认皮重量
				adcx1=Get_Adc_Average(ADC_Channel_1,10);
        LED2 = 0;
				while(KEY_Scan(0)!= S3_KEY3_PRES)//按S3确认总重量
				adcx2=Get_Adc_Average(ADC_Channel_1,10);
				LED3 = 0;
				adcx = adcx2-adcx1;
				weight = adcx/4.0;
				price = weight*unit_price;
        showFloat(56,0,weight,12);
        showFloat(65,1,price,12);
				total_price+=price;
			 //}
				showFloat(54,2,total_price,16);
				is_Ready = 1;
		}
}
void D_mode0(void)//不去皮
{
			while(is_Ready==0)
			{
				adcx = Get_Adc_Average(ADC_Channel_1,10);
        weight = adcx/4.0;
				price = weight*unit_price;
        showFloat(65,0,weight,12);
        showFloat(65,1,price,12);
	
				total_price+=price;
        showFloat(54,3,total_price,16);
				is_Ready=1;
			}
}

void D_mode(int fur_mode)
{
    switch(fur_mode)
		{
        case 1:D_mode1();break;
        case 0:D_mode0();break;
				default:break;
		}
}
























