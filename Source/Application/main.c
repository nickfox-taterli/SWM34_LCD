#include "SWM341.h"
#include "SystemInit.h"
#include "SysTick.h"

#include "lvgl.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lv_demos.h"

#include "FreeRTOS.h"
#include "task.h"


void vApplicationTickHook()
{
  lv_tick_inc(1);
}

void task_lvgl(void *arg)
{
  // 等待其他外设OK
  vTaskDelay(200);
  
  lv_init();
  lv_port_disp_init();
  lv_port_indev_init();

  lv_demo_widgets();
    
  while(1)
  {
      lv_task_handler();
  }
  vTaskDelete(NULL);
}

int main(void)
{
  uint32_t i = 0;
  
  SystemInit();
  SerialInit(115200);
  SDRAMInit();
  
  xTaskCreate(task_lvgl, (const char *)"lvgl", 1024, NULL, 2, NULL);
  vTaskStartScheduler();
  
  while (true)
  {

  }
}
