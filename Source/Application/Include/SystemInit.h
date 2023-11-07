#ifndef __SYSTEM_INIT_H__
#define __SYSTEM_INIT_H__

extern uint32_t SystemCoreClock;		// System Clock Frequency (Core Clock)
extern uint32_t CyclesPerUs;			// Cycles per micro second

void SystemInit(void);
void SerialInit(uint32_t Baudrate);
void SDRAMInit(void);
void LCDInit(void);

#endif
