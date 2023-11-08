/**
 * @file lv_port_disp_templ.c
 *
 */

/*Copy this file as "lv_port_disp.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_disp.h"
#include "lv_conf.h"
#include <stdbool.h>

#include "SWM341.h"
#include "OTM8009A.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void disp_init(void);

static void disp_flush(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map);

/**********************
 *  STATIC VARIABLES
 **********************/
#pragma location = ".SDRAM"  
static lv_color_t buf_3_1[LV_HOR_RES_MAX * LV_VER_RES_MAX]; //A screen sized buffer

#pragma location = ".SDRAM"  
static lv_color_t buf_3_2[LV_HOR_RES_MAX * LV_VER_RES_MAX]; //A screen sized buffer

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_disp_init(void)
{
    /*-------------------------
     * Initialize your display
     * -----------------------*/
    disp_init();

    /*------------------------------------
     * Create a display and set a flush_cb
     * -----------------------------------*/
    lv_display_t * disp = lv_display_create(LV_HOR_RES_MAX, LV_VER_RES_MAX);
    lv_display_set_flush_cb(disp, disp_flush);

    // /* Example 1
    //  * One buffer for partial rendering*/
    // static lv_color_t buf_1_1[MY_DISP_HOR_RES * 10];                          /*A buffer for 10 rows*/
    // lv_display_set_draw_buffers(disp, buf_1_1, NULL, sizeof(buf_1_1), LV_DISPLAY_RENDER_MODE_PARTIAL);

    // /* Example 2
    //  * Two buffers for partial rendering
    //  * In flush_cb DMA or similar hardware should be used to update the display in the background.*/
    // static lv_color_t buf_2_1[MY_DISP_HOR_RES * 10];
    // static lv_color_t buf_2_2[MY_DISP_HOR_RES * 10];
    // lv_display_set_draw_buffers(disp, buf_2_1, buf_2_2, sizeof(buf_2_1), LV_DISPLAY_RENDER_MODE_PARTIAL);
    
    /* Example 3
     * Two buffers screen sized buffer for double buffering.
     * Both LV_DISPLAY_RENDER_MODE_DIRECT and LV_DISPLAY_RENDER_MODE_FULL works, see their comments*/
    lv_display_set_draw_buffers(disp, buf_3_1, buf_3_2, sizeof(buf_3_1), LV_DISPLAY_RENDER_MODE_DIRECT);

}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*Initialize your display and the required peripherals.*/
static void disp_init(void)
{
   LCD_SPI_Init_OTM8009A();   
   LCD_RGB_Init_OTM8009A();
   
   LCD->L[LCD_LAYER_1].ADDR = (uint32_t)buf_3_1;
   LCD->START |= (1 << LCD_START_GO_Pos);
}

volatile bool disp_flush_enabled = true;

/* Enable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_enable_update(void)
{
    disp_flush_enabled = true;
}

/* Disable updating the screen (the flushing process) when disp_flush() is called by LVGL
 */
void disp_disable_update(void)
{
    disp_flush_enabled = false;
}

/*Flush the content of the internal buffer the specific area on the display.
 *`px_map` contains the rendered image as raw pixel map and it should be copied to `area` on the display.
 *You can use DMA or any hardware acceleration to do this operation in the background but
 *'lv_display_flush_ready()' has to be called when it's finished.*/
static void disp_flush(lv_display_t * disp_drv, const lv_area_t * area, uint8_t * px_map)
{
    if(disp_flush_enabled) {
        /*The most simple case (but also the slowest) to put all pixels to the screen one-by-one*/

      LCD->L[LCD_LAYER_1].ADDR = (uint32_t)px_map;
      LCD->CR |= (1 << LCD_CR_VBPRELOAD_Pos);
      while (0 != (LCD->CR & LCD_CR_VBPRELOAD_Msk))
		__NOP();

    }

    /*IMPORTANT!!!
     *Inform the graphics library that you are ready with the flushing*/
    lv_display_flush_ready(disp_drv);
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
