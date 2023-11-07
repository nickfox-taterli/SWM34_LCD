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

  // ϵͳ��ʼ��ȫ����ʱ�ر��ж�
  __disable_irq();

  SYS->CLKEN0 |= (1 << SYS_CLKEN0_ANAC_Pos);

  // ����Flash��д�ӳ�?
  ((int (*)(uint32_t, uint32_t, uint32_t))0x11000431)(0x16589, 0x4C74, 0x0B11FFAC);

  // �����ڲ�20MHz��
  SYS->HRCCR = (1 << SYS_HRCCR_ON_Pos) | (0 << SYS_HRCCR_DBL_Pos); // HRC = 20Hz

  // ����һ���ӳ�ʹ��ʱ���ȶ�����.
  for (i = 0; i < 20000; i++)
    __NOP();

  // �Ȱ�ϵͳʱ���л����ڲ�20MHz(HRC).
  SYS->CLKSEL |= (1 << SYS_CLKSEL_SYS_Pos);

  // ��������Ҫ����(��Ȼ�����ϵ�Ĭ�Ͼ��幦��)

  // PA3,PA4 => XTAL
  PORTA->FUNC0 &= ~0x000FF000;
  PORTA->FUNC0 |= 0x000FF000;
  PORTA->INEN &= ~((1 << 3) | (1 << 4));

  // XTAL����
  SYS->XTALCR |= (1 << SYS_XTALCR_ON_Pos) | (15 << SYS_XTALCR_DRV_Pos) | (1 << SYS_XTALCR_DET_Pos);

  // ����һ���ӳ�ʹ��ʱ���ȶ�����.
  for (i = 0; i < 20000; i++)
    __NOP();
  for (i = 0; i < 20000; i++)
    __NOP();

  // �л�PLLԴʱ�ӵ�XTAL
  SYS->PLLCR &= ~(1 << SYS_PLLCR_INSEL_Pos);

  // ����PLL����,Ŀ��150MHz.
  SYS->PLLDIV &= ~(SYS_PLLDIV_INDIV_Msk |
                   SYS_PLLDIV_FBDIV_Msk |
                   SYS_PLLDIV_OUTDIV_Msk);
  SYS->PLLDIV |= (PLL_IN_DIV << SYS_PLLDIV_INDIV_Pos) |
                 (PLL_FB_DIV << SYS_PLLDIV_FBDIV_Pos) |
                 (PLL_OUT_DIV << SYS_PLLDIV_OUTDIV_Pos);

  // ����PLL.
  SYS->PLLCR &= ~(1 << SYS_PLLCR_OFF_Pos);

  // �ȴ�PLL����
  while (SYS->PLLLOCK == 0)
    ;

  // PLL���.
  SYS->PLLCR |= (1 << SYS_PLLCR_OUTEN_Pos);

  // 1 = �ر�CLKDIV
  SYS->CLKDIVx_ON = 1;

  // �л��ڲ�ʱ��Դʱ�ӵ�PLL.
  SYS->CLKSEL &= ~SYS_CLKSEL_CLK_Msk;
  SYS->CLKSEL |= (1 << SYS_CLKSEL_CLK_Pos);

  // PLL ����Ƶ
  SYS->CLKSEL &= ~(1 << SYS_CLKSEL_CLK_DIVx_Pos);

  // ѡ���ڲ�ʱ��ԴΪ����ϵͳʱ��,��PLL.
  SYS->CLKSEL &= ~(1 << SYS_CLKSEL_SYS_Pos);

  SystemCoreClock = HSE_VALUE / PLL_IN_DIV * PLL_FB_DIV * 4 / (2 << (2 - PLL_OUT_DIV));
  CyclesPerUs = SystemCoreClock / 1000000;

  // ������IOʱ��
  SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_GPIOA_Pos);
  SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_GPIOB_Pos);
  SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_GPIOC_Pos);
  SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_GPIOD_Pos);
  SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_GPIOM_Pos);
  SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_GPION_Pos);

  SysTick_Config(SystemCoreClock / 1000);

  // �ָ��ж�
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
  
    // �Ƚ�������������.
    UART0->CTRL &= ~(0x01 << UART_CTRL_EN_Pos);
    
    UART0->BAUD &= ~(UART_BAUD_BAUD_Msk | UART_BAUD_FRAC_Msk);
    UART0->BAUD |= (((SystemCoreClock/Baudrate - 1) / 16) << UART_BAUD_BAUD_Pos) | 
                   (((SystemCoreClock/Baudrate - 1) % 16) << UART_BAUD_FRAC_Pos);
    
    // CTRL����(Ĭ�Ͼ���8λ��У��,1ֹͣλ,�޳�ʱ��FIFO����)
    // UART0->CTRL |= ...
    
    // ��������
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

    SDRAMC->TIM = 0x00000059 | ((CyclesPerUs * 200)  << SDRAMC_TIM_T100US_Pos);
    SDRAMC->CFG = 0x00000004;
    SDRAMC->T64 = (64*1000 / 4096 + 1) * CyclesPerUs;

    SDRAMC->CR = (1 << SDRAMC_CR_PWRON_Pos);
	
    while(SDRAMC->CR & SDRAMC_CR_PWRON_Msk) __NOP();	
}
