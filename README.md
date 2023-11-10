# SWM34SRET6 LCD 开发板 (淘宝120元电容触摸开发板)

**特别声明:我不是商家,也没义务为这个板子提供任何技术支持,这个只能说是我自己个人移植记录,推荐使用官方SDK(至少这样可能有技术支持),目前仍在低优先级的把玩开发中,很多东西还不完善.**

![PCB图片](https://github.com/nickfox-taterli/SWM34_LCD/blob/main/assets/board.png "PCB图片")

## 硬件资源

- 4.0 寸 480 * 800 电容触摸屏 (SPI + RGB)
- SWD(排针),USB UART,USB A,SD(SDIO)
- 2MB SPI Flash
- PCF8563 RTC
- SC8002D DAC
- ESP8266MOD (未焊接)

## 开发进度

- [ ] 基本外设验证
	- [x] SDRAMC 模块
	- [ ] 串行接口
		- [x] UART
		- [ ] SPI
		- [x] I2C
		- [ ] CAN
	- [ ] PWM 控制模块
	- [ ] 定时器 模块
	- [ ] QEI 模块
	- [ ] WDT 模块
	- [ ] RTC 模块
	- [ ] CORIDC 模块
	- [ ] CRC 模块
	- [ ] DIV 模块
	- [x] TFT LCD 驱动模块
	- [ ] JPEG
	- [ ] DMA2D
	- [ ] SDIO
	- [ ] DMA
	- [ ] USB
	- [ ] SFC
	- [x] GPIO
	- [ ] 模拟外设
		- [ ] SAR ADC
		- [ ] OPAMP
		- [ ] COMP
		- [ ] DAC
	- [ ] 欠压检测
	- [ ] 环境温度传感器
- [ ] 板载外设验证
	- [x] 屏幕
	- [x] SWD
	- [x] UART
	- [ ] USB A 
	- [ ] SDIO
	- [ ] SPI Flash
	- [ ] RTC
	- [ ] DAC
	- [ ] ESP8266MOD

## 特别说明

如果你复制我的代码到你产品最后被坑了请不要找我,用国产芯片本来坑就多,再说我这个就是个玩,也没想过有多完善.