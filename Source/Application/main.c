#include "SWM341.h"
#include "SystemInit.h"
#include "SysTick.h"
#include "OTM8009A.h"

#pragma location = ".SDRAM"
uint8_t lcdbuf_gui[100 * 100]; //A screen sized buffer

int main(void)
{
  uint32_t i = 0,val = 0;
  SystemInit();
  SerialInit(115200);
  SDRAMInit();
  
  LCD_SPI_Init_OTM8009A();
     
  // PD9 => 蜂鸣器
  GPIOD->DIR |= (1 << 9);
  PORTD->PULLD &= ~(1 << 9);
  PORTD->PULLU &= ~(1 << 9);
  PORTD->OPEND &= ~(1 << 9);  
  
  while (true)
  {
    uint32_t i = 0;
    for(i = 0; i < 100*100;i++){
      lcdbuf_gui[i] = i & 0xFF;
    }
  }
}
