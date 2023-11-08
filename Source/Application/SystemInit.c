#include "SWM341.h"
#include "SysTick.h"

#define HSE_VALUE (12000000UL)

#define PLL_OUT_DIV8 0
#define PLL_OUT_DIV4 1
#define PLL_OUT_DIV2 2

#define PLL_IN_DIV 3
#define PLL_FB_DIV 75
#define PLL_OUT_DIV PLL_OUT_DIV8

uint32_t SystemCoreClock = 0;
uint32_t CyclesPerUs = 0;

void SystemInit(void)
{
  uint32_t i = 0;

  // 系统初始化全程临时关闭中断
  __disable_irq();

  SYS->CLKEN0 |= (1 << SYS_CLKEN0_ANAC_Pos);

  // 配置Flash读写延迟?
  ((int (*)(uint32_t, uint32_t, uint32_t))0x11000431)(0x16589, 0x4C74, 0x0B11FFAC);

  // 开启内部20MHz震荡
  SYS->HRCCR = (1 << SYS_HRCCR_ON_Pos) | (0 << SYS_HRCCR_DBL_Pos); // HRC = 20Hz

  // 给予一定延迟使得时钟稳定起来.
  for (i = 0; i < 20000; i++)
    __NOP();

  // 先把系统时钟切换到内部20MHz(HRC).
  SYS->CLKSEL |= (1 << SYS_CLKSEL_SYS_Pos);

  // 晶体引脚要设置(居然不是上电默认晶体功能)

  // PA3,PA4 => XTAL
  PORTA->FUNC0 &= ~0x000FF000;
  PORTA->FUNC0 |= 0x000FF000;
  PORTA->INEN &= ~((1 << 3) | (1 << 4));

  // XTAL启动
  SYS->XTALCR |= (1 << SYS_XTALCR_ON_Pos) | (15 << SYS_XTALCR_DRV_Pos) | (1 << SYS_XTALCR_DET_Pos);

  // 给予一定延迟使得时钟稳定起来.
  for (i = 0; i < 20000; i++)
    __NOP();
  for (i = 0; i < 20000; i++)
    __NOP();

  // 切换PLL源时钟到XTAL
  SYS->PLLCR &= ~(1 << SYS_PLLCR_INSEL_Pos);

  // 配置PLL参数,目标150MHz.
  SYS->PLLDIV &= ~(SYS_PLLDIV_INDIV_Msk |
                   SYS_PLLDIV_FBDIV_Msk |
                   SYS_PLLDIV_OUTDIV_Msk);
  SYS->PLLDIV |= (PLL_IN_DIV << SYS_PLLDIV_INDIV_Pos) |
                 (PLL_FB_DIV << SYS_PLLDIV_FBDIV_Pos) |
                 (PLL_OUT_DIV << SYS_PLLDIV_OUTDIV_Pos);

  // 启动PLL.
  SYS->PLLCR &= ~(1 << SYS_PLLCR_OFF_Pos);

  // 等待PLL锁定
  while (SYS->PLLLOCK == 0)
    ;

  // PLL输出.
  SYS->PLLCR |= (1 << SYS_PLLCR_OUTEN_Pos);

  // 1 = 关闭CLKDIV
  SYS->CLKDIVx_ON = 1;

  // 切换内部时钟源时钟到PLL.
  SYS->CLKSEL &= ~SYS_CLKSEL_CLK_Msk;
  SYS->CLKSEL |= (1 << SYS_CLKSEL_CLK_Pos);

  // PLL 不分频
  SYS->CLKSEL &= ~(1 << SYS_CLKSEL_CLK_DIVx_Pos);

  // 选择内部时钟源为最终系统时钟,即PLL.
  SYS->CLKSEL &= ~(1 << SYS_CLKSEL_SYS_Pos);

  SystemCoreClock = HSE_VALUE / PLL_IN_DIV * PLL_FB_DIV * 4 / (2 << (2 - PLL_OUT_DIV));
  CyclesPerUs = SystemCoreClock / 1000000;

  // 打开所有IO时钟
  SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_GPIOA_Pos);
  SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_GPIOB_Pos);
  SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_GPIOC_Pos);
  SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_GPIOD_Pos);
  SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_GPIOM_Pos);
  SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_GPION_Pos);

  SysTick_Config(SystemCoreClock / 1000);

  // 恢复中断
  __enable_irq();
}

void SerialInit(uint32_t Baudrate)
{    
    // PM0->RXD PM1->TXD
    PORTM->FUNC0 &= ~0x000000FF;
    PORTM->FUNC0 |=  0x00000011;
    
    PORTM->INEN |=  (1 << 0);
    PORTM->INEN &=  ~(1 << 1);
    
    SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_UART0_Pos);
  
    // 先禁用外设再设置.
    UART0->CTRL &= ~(0x01 << UART_CTRL_EN_Pos);
    
    UART0->BAUD &= ~(UART_BAUD_BAUD_Msk | UART_BAUD_FRAC_Msk);
    UART0->BAUD |= (((SystemCoreClock/Baudrate - 1) / 16) << UART_BAUD_BAUD_Pos) | 
                   (((SystemCoreClock/Baudrate - 1) % 16) << UART_BAUD_FRAC_Pos);
    
    // CTRL配置(默认就是8位无校验,1停止位,无超时和FIFO管理)
    // UART0->CTRL |= ...
    
    // 开启外设
    UART0->CTRL |= (0x01 << UART_CTRL_EN_Pos);
}

void SDRAMInit(void){
  PORTB->FUNC0 &= ~0xFF000000;
  PORTB->FUNC0 |=  0x11000000;
  PORTB->FUNC1 &= ~0x00000FFF;
  PORTB->FUNC1 |=  0x00000444;
  
  PORTC->FUNC1 &= ~0xFF000000;
  PORTC->FUNC1 |=  0x11000000;
  PORTC->INEN  |= (1 << 15) | (1 << 14);
  
  PORTE->FUNC0 = 0x11111111;
  PORTE->FUNC1 = 0x11111111;
  PORTE->INEN  |= 0x3FFF;

  PORTM->FUNC1 &= ~0xFFFF0000;
  PORTM->FUNC1 |=  0x33220000;
  
  PORTN->FUNC0 &= ~0xFF00FF00;
  PORTN->FUNC0 |=  0x11002100;
  PORTN->FUNC1 = 0x11111111;

  SYS->CLKEN1 |= (1 << SYS_CLKEN1_SDRAM_Pos);
	
  SDRAMC->TIM = (1  << SDRAMC_TIM_TRP_Pos) |
              (1 << SDRAMC_TIM_TRCD_Pos) |
              (8 << SDRAMC_TIM_TRFC_Pos) |
              ((CyclesPerUs * 200)  << SDRAMC_TIM_T100US_Pos);

  SDRAMC->CFG = (0 /* SDRAM_SIZE_8MB */       << SDRAMC_CFG_SIZE_Pos)     |
              (0 /* SDRAM_CLKDIV_1 */   << SDRAMC_CFG_CLKDIV_Pos)   |
              (1 /* SDRAM_CASLATENCY_3 */ << SDRAMC_CFG_CASDELAY_Pos) |
              (((SystemCoreClock) > 66000000) << SDRAMC_CFG_HIGHFREQ_Pos);

  SDRAMC->T64 = (64*1000 / 4096 /* SDRAM_SIZE_8MB */ + 1) * CyclesPerUs;

  SDRAMC->CR = (1 << SDRAMC_CR_PWRON_Pos);
  SDRAMC->CR &= ~SDRAMC_CR_ENTERSRF_Msk;
  
  SYS_Delay(100);
  
  // 我也想正规判断,但是他偶尔失效,经测试,延迟一会等待更靠谱?
  // while(SDRAMC->CR & SDRAMC_CR_PWRON_Msk) __NOP();
}
