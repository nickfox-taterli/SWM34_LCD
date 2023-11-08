#include "SWM341.h"
#include "FT5206.h"
#include "SysTick.h"

#define FT_CMD_WR 0X70 // 写命令
#define FT_CMD_RD 0X71 // 读命令

#define FT_DEVIDE_MODE 0x00 // FT5206模式控制寄存器
#define FT_REG_NUM_FINGER 0x02 // 触摸状态寄存器

#define FT_TP1_REG 0X03 // 第一个触摸点数据地址
#define FT_TP2_REG 0X09 // 第二个触摸点数据地址
#define FT_TP3_REG 0X0F // 第三个触摸点数据地址
#define FT_TP4_REG 0X15 // 第四个触摸点数据地址
#define FT_TP5_REG 0X1B // 第五个触摸点数据地址

#define FT_ID_G_LIB_VERSION 0xA1  // 版本
#define FT_ID_G_MODE 0xA4         // FT5206中断模式控制寄存器
#define FT_ID_G_THGROUP 0x80      // 触摸有效值设置寄存器
#define FT_ID_G_PERIODACTIVE 0x88 // 激活状态周期设置寄存器

#define TP_PRES_DOWN 0x80 // 触屏被按下
#define TP_CATH_PRES 0x40 // 有按键按下

volatile uint8_t ft5206_touched = 0;

uint8_t I2C_Start(I2C_TypeDef *I2Cx, uint8_t addr)
{
    I2Cx->TXDATA = addr;
    I2Cx->MCR = (1 << I2C_MCR_STA_Pos) | (1 << I2C_MCR_WR_Pos);
    while (I2Cx->MCR & I2C_MCR_WR_Msk)
        __NOP();
    return (I2Cx->TR & I2C_TR_RXACK_Msk) ? 0 : 1;
}

uint8_t I2C_Write(I2C_TypeDef *I2Cx, uint8_t data)
{
    I2Cx->TXDATA = data;
    I2Cx->MCR = (1 << I2C_MCR_WR_Pos);
    while (I2Cx->MCR & I2C_MCR_WR_Msk)
        __NOP();
    return (I2Cx->TR & I2C_TR_RXACK_Msk) ? 0 : 1;
}

void I2C_Stop(I2C_TypeDef *I2Cx)
{
    I2Cx->MCR = (1 << I2C_MCR_STO_Pos);
    while (I2Cx->MCR & I2C_MCR_STO_Msk)
        __NOP();
}

uint8_t I2C_Read(I2C_TypeDef *I2Cx, uint8_t ack)
{
    I2Cx->TR = ((ack ? 0 : 1) << I2C_TR_TXACK_Pos);

    I2Cx->MCR = (1 << I2C_MCR_RD_Pos);
    while (I2Cx->MCR & I2C_MCR_RD_Msk)
        __NOP();

    return I2Cx->RXDATA;
}

static inline void delay(void)
{
    for (uint32_t i = 0; i < 600; i++)
        __NOP();
}

static uint8_t ft5206_write_regs(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint32_t i;
    uint8_t ack;

    ack = I2C_Start(I2C1, FT_CMD_WR);
    if (ack == 0)
        goto wr_fail;

    ack = I2C_Write(I2C1, reg & 0XFF);
    if (ack == 0)
        goto wr_fail;

    for (i = 0; i < len; i++)
    {
        ack = I2C_Write(I2C1, buf[i]);
        if (ack == 0)
            goto wr_fail;
    }

    I2C_Stop(I2C1);
    delay();
    return 0;

wr_fail:
    I2C_Stop(I2C1);
    delay();
    return 1;
}

static uint32_t ft5206_read_regs(uint16_t reg, uint8_t *buf, uint8_t len)
{
    uint8_t i;
    uint8_t ack;

    ack = I2C_Start(I2C1, FT_CMD_WR);
    if (ack == 0)
        goto rd_fail;

    ack = I2C_Write(I2C1, reg & 0XFF);
    if (ack == 0)
        goto rd_fail;

    delay(); // 此处必须延时等待一会再启动

    ack = I2C_Start(I2C1, FT_CMD_RD); // ReStart
    if (ack == 0)
        goto rd_fail;

    for (i = 0; i < len; i++)
    {
        buf[i] = I2C_Read(I2C1, (i == (len - 1) ? 0 : 1));
    }

    I2C_Stop(I2C1);
    delay();
    return 0;

rd_fail:
    I2C_Stop(I2C1);
    delay();
    return 1;
}

void ft5206_init(void)
{
    uint8_t temp;

    // I2C 硬件驱动引脚
    PORTC->FUNC0 &= ~0x00FF0000;
    PORTC->FUNC0 |= 0x00110000;
    PORTC->INEN |= (1 << 4) | (1 << 5);
    PORTC->PULLU |= (1 << 4) | (1 << 5);
    PORTC->OPEND |= (1 << 4) | (1 << 5);

    SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_I2C1_Pos);

    // 禁用I2C
    I2C1->CR &= ~(0x01 << I2C_CR_EN_Pos);

    // 主机模式
    I2C1->CR |= (1 << I2C_CR_MASTER_Pos);
    // 时钟配置 (400kHz)
    I2C1->CLK = 0x0001162D;
    // 清除所有Flag
    I2C1->IF = 0xFFFFFFFF;
    // 启用I2C
    I2C1->CR |= (0x01 << I2C_CR_EN_Pos);

    // 配置TP INT引脚
    PORTN->FUNC0 &= ~0x00F00000;
    PORTN->INEN |= (1 << 5);
    GPION->DIR &= ~(0x01 << 5);
    PORTN->PULLD &= ~(1 << 5);

    GPION->INTLVLTRG &= ~(1 << 5);
    GPION->INTRISEEN &= ~(1 << 5);
    GPION->INTCLR = (1 << 5);
    GPION->INTEN |= (0x01 << 5);
    NVIC_EnableIRQ(GPION_IRQn);

    // 不需要手动复位,否则可能还有其他问题,具体未查明.
    // PORTC->FUNC1 &= ~0x0000000F;
    // GPIOC->DIR |= (0x01 << 8);
    // PORTC->PULLU |= (1 << 8);
    // GPIOC->DATAPIN8 = 0;
    // SYS_Delay(300);
    // GPIOC->DATAPIN8 = 1;
    // SYS_Delay(150);

    // 进入正常操作模式
    temp = 0;
    ft5206_write_regs(FT_DEVIDE_MODE, &temp, 1);

    // 0-查询模式 1-触发模式
    temp = 1;
    ft5206_write_regs(FT_ID_G_MODE, &temp, 1);

    // 触摸有效值,越小越灵敏.
    temp = 22;
    ft5206_write_regs(FT_ID_G_THGROUP, &temp, 1);

    // 激活周期 [12,14]
    temp = 12;
    ft5206_write_regs(FT_ID_G_PERIODACTIVE, &temp, 1);
}

uint8_t ft5206_read_points(tp_dev_t *tp_dev)
{
    uint8_t mode = 0;
    uint8_t buf[4];
    uint8_t i = 0;
    uint8_t temp;

    static const uint16_t FT5206_TPX_Table[5] = {FT_TP1_REG, FT_TP2_REG, FT_TP3_REG, FT_TP4_REG, FT_TP5_REG};
    
    if(ft5206_touched) ft5206_touched = 0;
    else return 0;

    ft5206_read_regs(FT_REG_NUM_FINGER, &mode, 1); // 读取触摸点的状态

    if ((mode & 0XF) && ((mode & 0XF) <= 5))
    {
        // 将点的个数转换为1的位数,匹配 tp_dev.sta 定义
        temp = 0XFF << (mode & 0XF);
        tp_dev->status = (~temp) | TP_PRES_DOWN | TP_CATH_PRES;

        for (i = 0; i < 5; i++)
        {
            if (tp_dev->status & (1 << i)) // 触摸有效?
            {
                ft5206_read_regs(FT5206_TPX_Table[i], buf, 4); // 读取XY坐标值
                // 横屏
                //  tp_dev->x[i] = ((uint16_t)(buf[0] & 0X0F) << 8) + buf[1];//FT5406 7INCH  //tp_dev->y[i] = ((uint16_t)(buf[0] & 0X0F) << 8) + buf[1];
                //  tp_dev->y[i] = ((uint16_t)(buf[2] & 0X0F) << 8) + buf[3];//FT5406 7INCH  //tp_dev->x[i] = ((uint16_t)(buf[2] & 0X0F) << 8) + buf[3];
                tp_dev->y[i] = ((uint16_t)(buf[2] & 0X0F) << 8) + buf[3];
                tp_dev->x[i] = /*LCD_HDOT -*/ ((uint16_t)(buf[0] & 0X0F) << 8) + buf[1];
            }
        }
        if (tp_dev->x[0] == 0 && tp_dev->y[0] == 0)
        {
            // 读到的数据都是0,则忽略此次数据
            mode = 0; 
        }
    }

    // 无触摸点按下
    if ((mode & 0X1F) == 0)
    {
        if (tp_dev->status & TP_PRES_DOWN)
        {
            // 标记按键松开
            tp_dev->status &= ~TP_PRES_DOWN;
        }
        else
        {
            tp_dev->x[0] = 0xffff;
            tp_dev->y[0] = 0xffff;
            tp_dev->status &= 0XE0;
        }
    }
    return 1;
}

void GPION_Handler(void)
{
    if (GPION->INTSTAT & (1 << 5))
    {
        GPION->INTCLR = (1 << 5);
        ft5206_touched = 1;
    }
}