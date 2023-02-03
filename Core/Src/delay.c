#include"delay.h"

static uint32_t fac_us = 0;
static uint32_t fac_ms = 0;

void delay_init()
{
    HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK_DIV8); // 配置为外部时钟源，频率为HCLK / 8
    fac_us = SystemCoreClock / 8000000; 	// 当HCLK为72MHz时，fac_us为9，即表示9个时钟周期耗时1μs
    fac_ms = fac_us * 1000;
}

__INLINE static void __delay_impl(uint32_t fac)
{
    /* 记录寄存器中的旧值 */
	uint32_t oldLoad = SysTick->LOAD;
	uint32_t oldVal = SysTick->VAL;
	uint32_t oldCTRL = SysTick->CTRL;

    /* 开启计时并等待计时结束 */
	uint32_t temp;
	SysTick->LOAD=fac; 							// LOAD注意不要超过24bit（换算成时间为1864ms）
	SysTick->VAL=0x00;        					// VAL
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	// CTRL开始计时	  
	do{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		// 判断SysTick->CTRL->COUNTFLAG标志是否为1，并且计时使能打开

	/* 关闭本次计时 */
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	
	SysTick->VAL =0X00;

	/* 恢复上次计时 */
	SysTick->LOAD = oldLoad;
	SysTick->VAL = oldVal;
	SysTick->CTRL = oldCTRL; 
}

void delay_us(uint32_t nus)
{		
	__delay_impl(nus * fac_us);				 
}


void delay_ms(uint32_t nms)
{	 		  	  
	__delay_impl(nms * fac_ms);				 					  	    
}

void delay_s(uint32_t s){
	for(int i = 0; i < s; ++i){
		delay_ms(1000);
	}
}