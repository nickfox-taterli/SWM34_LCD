#ifndef __FT5206_H__
#define __FT5206_H__

typedef struct _tp_dev_t
{
    uint16_t x[5];  // 当前坐标
    uint16_t y[5];  // 电容屏有最多5组坐标
    uint8_t status; // 笔的状态
                    // B7:按下1/松开0
                    // B6~B5:保留
                    // B4~B0:电容触摸屏按下的点数(0表示未按下,1表示按下)
} tp_dev_t;

void ft5206_init(void);
uint8_t ft5206_read_points(tp_dev_t *tp_dev);

#endif
