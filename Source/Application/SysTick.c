#include "SWM341.h"
#include "lvgl.h"

__IO uint32_t uwTick;

void SYS_IncTick(void)
{
  
}

uint32_t SYS_GetTick(void)
{
  return uwTick;
}


void SYS_Delay(uint32_t Delay)
{
  uint32_t tickstart = SYS_GetTick();
  uint32_t wait = Delay;

  if (wait < 0xFFFFFFFFU)
  {
    wait++;
  }

  while ((SYS_GetTick() - tickstart) < wait)
  {
  }
}

void SysTick_Handler(void)
{	
  uwTick++;
  lv_tick_inc(1);
}