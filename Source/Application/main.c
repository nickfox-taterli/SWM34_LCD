#include "SWM341.h"
#include "SystemInit.h"
#include "SysTick.h"

#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_demos.h"

int main(void)
{
  uint32_t i = 0;
  
  SystemInit();
  SerialInit(115200);
  SDRAMInit();     
  
  // PD9 => 蜂鸣器
  GPIOD->DIR |= (1 << 9);
  PORTD->PULLD &= ~(1 << 9);
  PORTD->PULLU &= ~(1 << 9);
  PORTD->OPEND &= ~(1 << 9);  
  
  lv_init();
  lv_port_disp_init();
  lv_demo_widgets();
  
  while (true)
  {
    lv_task_handler(); 
#if 0   
    for(i = 0; i < 800 * 480 * 2; i += 2)
    {
       *((volatile uint16_t *)(0x80000000 + i)) = 0x003F;
    }
    
    LCD->L[LCD_LAYER_1].ADDR = (uint32_t)0x80000000;
    LCD->CR |= (1 << LCD_CR_VBPRELOAD_Pos);
    while (0 != (LCD->CR & LCD_CR_VBPRELOAD_Msk))
      __NOP();
    
    SYS_Delay(1000);
    
    for(i = 0; i < 800 * 480 * 2; i += 2)
    {
       *((volatile uint16_t *)(0x80000000 + i)) = 0x3F00;
    }
    
    LCD->L[LCD_LAYER_1].ADDR = (uint32_t)0x80000000;
    LCD->CR |= (1 << LCD_CR_VBPRELOAD_Pos);
    while (0 != (LCD->CR & LCD_CR_VBPRELOAD_Msk))
      __NOP();
    
    SYS_Delay(1000);
#endif
  }
}
