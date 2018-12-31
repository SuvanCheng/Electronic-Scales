#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "oled.h"
#include "usart.h"	 
#include "adc.h"
#include "schedule.h" 
/************************************************
 ALIENTEK 战舰STM32F103开发板实验17
 ADC 实验 
************************************************/


 
 int main(void)
 {
	delay_init();	    	 //延时函数初始化	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置中断优先级分组为组2：2位抢占优先级，2位响应优先级
	uart_init(115200);	 	//串口初始化为115200
 	LED_Init();			     //LED端口初始化	 
 	Adc_Init();		  		//ADC初始化
	KEY_Init();
	OLED_Init();
  OLED_Clear();
	Display(1);
	while(1)
	{
			Schedule();
	
	}
	
 }

