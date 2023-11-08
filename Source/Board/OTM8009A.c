#include "SWM341.h"
#include "SysTick.h"

static void LL_WriteData(unsigned char i)
{
  for (unsigned char n = 0; n < 8; n++)
  {
    if (i & 0x80)
    {
      GPIOC->DATAPIN4 = 1;
    }
    else
    {
      GPIOC->DATAPIN4 = 0;
    }
    i <<= 1;
    GPIOC->DATAPIN5 = 0;
    GPIOC->DATAPIN5 = 1;
  }
}


static void LCD_WriteCommand(unsigned char IndexH,unsigned char IndexL)
{
  GPIOD->DATAPIN1 = 0;

  LL_WriteData(0x20);
  LL_WriteData(IndexH);
  
  LL_WriteData(0x00);
  LL_WriteData(IndexL);
	
  GPIOD->DATAPIN1 = 1;
}


static void LCD_WriteData(unsigned char Data)
{
  GPIOD->DATAPIN1 = 0;

  LL_WriteData(0x40);
  LL_WriteData(Data);
	
  GPIOD->DATAPIN1 = 1;
}

void LCD_SPI_Init_OTM8009A(void){
  // PC8 -> 复位

  PORTC->FUNC1 &= ~0x0000000F; // 取消复用功能
  GPIOC->DIR |= (1 << 8); // 输出引脚
  PORTC->PULLU |= (1 << 8); // 引脚上拉
  
  GPIOC->DATAPIN8 = 1;
  SYS_Delay(100);
  GPIOC->DATAPIN8 = 0;
  SYS_Delay(500);
  GPIOC->DATAPIN8 = 1;
  SYS_Delay(200);
    
  // PD0 -> LCD_BL / PD1 -> LCD_CS
  PORTD->FUNC0 &= ~0x000000FF; // 取消复用功能
  GPIOD->DIR |= (1 << 0) | (1 << 1); // 输出引脚
  PORTD->PULLU |= (1 << 0) | (1 << 1); // 引脚上拉
  
  GPIOD->DATAPIN0 = 1;
  GPIOD->DATAPIN1 = 1;
	
  // PC4 -> LCD_SDA / PC5 -> LCD_SCK
  PORTC->FUNC0 &= ~0x00FF0000; // 取消复用功能
  GPIOC->DIR |= (1 << 4) | (1 << 5); // 输出引脚
  PORTC->PULLU |= (1 << 4) | (1 << 5); // 引脚上拉
  
  GPIOC->DATAPIN4 = 1;
  GPIOC->DATAPIN5 = 1;
  
  //************* Start Initial Sequence **********//	

  //OTM8009A+C0401D INIT
  LCD_WriteCommand(0xff,0x00); LCD_WriteData(0x80);
  LCD_WriteCommand(0xff,0x01); LCD_WriteData(0x09);     //enable EXTC
  LCD_WriteCommand(0xff,0x02); LCD_WriteData(0x01);     
  LCD_WriteCommand(0xff,0x80); LCD_WriteData(0x80);     //enable Orise mode
  LCD_WriteCommand(0xff,0x81); LCD_WriteData(0x09);     
  LCD_WriteCommand(0xff,0x03); LCD_WriteData(0x01);     //enable SPI+I2C cmd2 read
                                        
  //gamma DC                            
  LCD_WriteCommand(0xc0,0xb4); LCD_WriteData(0x50);     //column inversion 	
  LCD_WriteCommand(0xC4,0x89); LCD_WriteData(0x08);     //reg off	
  LCD_WriteCommand(0xC0,0xa3); LCD_WriteData(0x00);     //pre-charge //V02             
  LCD_WriteCommand(0xC5,0x82); LCD_WriteData(0xA3);     //REG-pump23
  LCD_WriteCommand(0xC5,0x90); LCD_WriteData(0x96);     //Pump setting (3x=D6)-->(2x=96)//v02 01/11
  LCD_WriteCommand(0xC5,0x91); LCD_WriteData(0x87);     //Pump setting(VGH/VGL)   
  LCD_WriteCommand(0xD8,0x00); LCD_WriteData(0x73);     //GVDD=4.5V  73   
  LCD_WriteCommand(0xD8,0x01); LCD_WriteData(0x71);     //NGVDD=4.5V 71  
                                        
  //VCOMDC                                                                   
  LCD_WriteCommand(0xD9,0x00); LCD_WriteData(0x67);     // VCOMDC= 6A  
  SYS_Delay(20);                         
                        
  // Positive           
  LCD_WriteCommand(0xE1,0x00); LCD_WriteData(0x09);
  LCD_WriteCommand(0xE1,0x01); LCD_WriteData(0x0a);
  LCD_WriteCommand(0xE1,0x02); LCD_WriteData(0x0e);////0e
  LCD_WriteCommand(0xE1,0x03); LCD_WriteData(0x0d);
  LCD_WriteCommand(0xE1,0x04); LCD_WriteData(0x07);
  LCD_WriteCommand(0xE1,0x05); LCD_WriteData(0x17);//18
  LCD_WriteCommand(0xE1,0x06); LCD_WriteData(0x0d);
  LCD_WriteCommand(0xE1,0x07); LCD_WriteData(0x0d);
  LCD_WriteCommand(0xE1,0x08); LCD_WriteData(0x00);//01
  LCD_WriteCommand(0xE1,0x09); LCD_WriteData(0x04);
  LCD_WriteCommand(0xE1,0x0A); LCD_WriteData(0x04);//05
  LCD_WriteCommand(0xE1,0x0B); LCD_WriteData(0x06);   //06
  LCD_WriteCommand(0xE1,0x0C); LCD_WriteData(0x0F);   //0D
  LCD_WriteCommand(0xE1,0x0D); LCD_WriteData(0x25);////22
  LCD_WriteCommand(0xE1,0x0E); LCD_WriteData(0x1D);//20
  LCD_WriteCommand(0xE1,0x0F); LCD_WriteData(0x05);
                        
  // Negative           
  LCD_WriteCommand(0xE2,0x00); LCD_WriteData(0x09);
  LCD_WriteCommand(0xE2,0x01); LCD_WriteData(0x0a);
  LCD_WriteCommand(0xE2,0x02); LCD_WriteData(0x0e);////
  LCD_WriteCommand(0xE2,0x03); LCD_WriteData(0x0d);
  LCD_WriteCommand(0xE2,0x04); LCD_WriteData(0x07);
  LCD_WriteCommand(0xE2,0x05); LCD_WriteData(0x18);//
  LCD_WriteCommand(0xE2,0x06); LCD_WriteData(0x0d);
  LCD_WriteCommand(0xE2,0x07); LCD_WriteData(0x0d);
  LCD_WriteCommand(0xE2,0x08); LCD_WriteData(0x00);
  LCD_WriteCommand(0xE2,0x09); LCD_WriteData(0x04);
  LCD_WriteCommand(0xE2,0x0A); LCD_WriteData(0x04);//
  LCD_WriteCommand(0xE2,0x0B); LCD_WriteData(0x06);   
  LCD_WriteCommand(0xE2,0x0C); LCD_WriteData(0x0F);
  LCD_WriteCommand(0xE2,0x0D); LCD_WriteData(0x24);////
  LCD_WriteCommand(0xE2,0x0E); LCD_WriteData(0x1D);
  LCD_WriteCommand(0xE2,0x0F); LCD_WriteData(0x05);
                        
  LCD_WriteCommand(0xC1,0x81); LCD_WriteData(0x77);     //Frame rate 65Hz//V02   
                        
  // RGB I/F setting VSYNC for OTM8018 0x0e
  LCD_WriteCommand(0xC1,0xa1); LCD_WriteData(0x08);     //external Vsync,Hsync,DE
  LCD_WriteCommand(0xC0,0xa3); LCD_WriteData(0x1b);     //pre-charge //V02
  LCD_WriteCommand(0xC4,0x81); LCD_WriteData(0x83);     //source bias //V02
  LCD_WriteCommand(0xC5,0x92); LCD_WriteData(0x01);     //Pump45
  LCD_WriteCommand(0xC5,0xB1); LCD_WriteData(0xA9);     //DC voltage setting ;[0]GVDD output, default: 0xa8
                        
  // CE8x : vst1, vst2, vst3, vst4
  LCD_WriteCommand(0xCE,0x80); LCD_WriteData(0x85);	// ce81[7:0] : vst1_shift[7:0]
  LCD_WriteCommand(0xCE,0x81); LCD_WriteData(0x03);	// ce82[7:0] : 0000,	vst1_width[3:0]
  LCD_WriteCommand(0xCE,0x82); LCD_WriteData(0x00);	// ce83[7:0] : vst1_tchop[7:0]
  LCD_WriteCommand(0xCE,0x83); LCD_WriteData(0x84);	// ce84[7:0] : vst2_shift[7:0]
  LCD_WriteCommand(0xCE,0x84); LCD_WriteData(0x03);	// ce85[7:0] : 0000,	vst2_width[3:0]
  LCD_WriteCommand(0xCE,0x85); LCD_WriteData(0x00);	// ce86[7:0] : vst2_tchop[7:0]	                                                                                                                      
  LCD_WriteCommand(0xCE,0x86); LCD_WriteData(0x83);	// ce87[7:0] : vst3_shift[7:0]	                                                                                                                      
  LCD_WriteCommand(0xCE,0x87); LCD_WriteData(0x03);	// ce88[7:0] : 0000,	vst3_width[3:0]
  LCD_WriteCommand(0xCE,0x88); LCD_WriteData(0x00);	// ce89[7:0] : vst3_tchop[7:0]                                                     
  LCD_WriteCommand(0xCE,0x89); LCD_WriteData(0x82);	// ce8a[7:0] : vst4_shift[7:0]
  LCD_WriteCommand(0xCE,0x8a); LCD_WriteData(0x03);	// ce8b[7:0] : 0000,	vst4_width[3:0]
  LCD_WriteCommand(0xCE,0x8b); LCD_WriteData(0x00);	// ce8c[7:0] : vst4_tchop[7:0]
                        
  //CEAx : clka1, clka2 
  LCD_WriteCommand(0xCE,0xa0); LCD_WriteData(0x38);	// cea1[7:0] : clka1_width[3:0], clka1_shift[11:8]
  LCD_WriteCommand(0xCE,0xa1); LCD_WriteData(0x02);	// cea2[7:0] : clka1_shift[7:0]
  LCD_WriteCommand(0xCE,0xa2); LCD_WriteData(0x03);	// cea3[7:0] : clka1_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]                                                
  LCD_WriteCommand(0xCE,0xa3); LCD_WriteData(0x21);	// cea4[7:0] : clka1_switch[7:0]                                                                                        
  LCD_WriteCommand(0xCE,0xa4); LCD_WriteData(0x00);	// cea5[7:0] : clka1_extend[7:0]                                                                                        
  LCD_WriteCommand(0xCE,0xa5); LCD_WriteData(0x00);	// cea6[7:0] : clka1_tchop[7:0]                                                                                         
  LCD_WriteCommand(0xCE,0xa6); LCD_WriteData(0x00);	// cea7[7:0] : clka1_tglue[7:0]                                                                                         
  LCD_WriteCommand(0xCE,0xa7); LCD_WriteData(0x38);	// cea8[7:0] : clka2_width[3:0], clka2_shift[11:8]                                                                      
  LCD_WriteCommand(0xCE,0xa8); LCD_WriteData(0x01);	// cea9[7:0] : clka2_shift[7:0]                                                                                         
  LCD_WriteCommand(0xCE,0xa9); LCD_WriteData(0x03);	// ceaa[7:0] : clka2_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]                                                
  LCD_WriteCommand(0xCE,0xaa); LCD_WriteData(0x22);	// ceab[7:0] : clka2_switch[7:0]                                                                                        
  LCD_WriteCommand(0xCE,0xab); LCD_WriteData(0x00);	// ceac[7:0] : clka2_extend                                                                                             
  LCD_WriteCommand(0xCE,0xac); LCD_WriteData(0x00);	// cead[7:0] : clka2_tchop                                                                                              
  LCD_WriteCommand(0xCE,0xad); LCD_WriteData(0x00);	// ceae[7:0] : clka2_tglue                                                                                              
                                                                                                                                                            
  //CEBx : clka3, clka4                                                                                                                                     
  LCD_WriteCommand(0xCE,0xb0); LCD_WriteData(0x38);	// ceb1[7:0] : clka3_width[3:0], clka3_shift[11:8]                                                                      
  LCD_WriteCommand(0xCE,0xb1); LCD_WriteData(0x00);	// ceb2[7:0] : clka3_shift[7:0]                                                                                         
  LCD_WriteCommand(0xCE,0xb2); LCD_WriteData(0x03);	// ceb3[7:0] : clka3_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]                                                
  LCD_WriteCommand(0xCE,0xb3); LCD_WriteData(0x23);	// ceb4[7:0] : clka3_switch[7:0]                                                                                        
  LCD_WriteCommand(0xCE,0xb4); LCD_WriteData(0x00);	// ceb5[7:0] : clka3_extend[7:0]                                                                                        
  LCD_WriteCommand(0xCE,0xb5); LCD_WriteData(0x00);	// ceb6[7:0] : clka3_tchop[7:0]                                                                                         
  LCD_WriteCommand(0xCE,0xb6); LCD_WriteData(0x00);	// ceb7[7:0] : clka3_tglue[7:0]                                                                                         
  LCD_WriteCommand(0xCE,0xb7); LCD_WriteData(0x30);	// ceb8[7:0] : clka4_width[3:0], clka2_shift[11:8]                                                                      
  LCD_WriteCommand(0xCE,0xb8); LCD_WriteData(0x00);	// ceb9[7:0] : clka4_shift[7:0]                                                                                         
  LCD_WriteCommand(0xCE,0xb9); LCD_WriteData(0x03);	// ceba[7:0] : clka4_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]                                                
  LCD_WriteCommand(0xCE,0xba); LCD_WriteData(0x24);	// cebb[7:0] : clka4_switch[7:0]                                                                                        
  LCD_WriteCommand(0xCE,0xbb); LCD_WriteData(0x00);	// cebc[7:0] : clka4_extend                                                                                             
  LCD_WriteCommand(0xCE,0xbc); LCD_WriteData(0x00);	// cebd[7:0] : clka4_tchop                                                                                              
  LCD_WriteCommand(0xCE,0xbd); LCD_WriteData(0x00);	// cebe[7:0] : clka4_tglue                                                                                              
                                                                                                                                                            
  //CECx : clkb1, clkb2                                                                                                                                     
  LCD_WriteCommand(0xCE,0xc0); LCD_WriteData(0x30);	// cec1[7:0] : clkb1_width[3:0], clkb1_shift[11:8]                                                                      
  LCD_WriteCommand(0xCE,0xc1); LCD_WriteData(0x01);	// cec2[7:0] : clkb1_shift[7:0]                                                                                         
  LCD_WriteCommand(0xCE,0xc2); LCD_WriteData(0x03);	// cec3[7:0] : clkb1_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]                                                
  LCD_WriteCommand(0xCE,0xc3); LCD_WriteData(0x25);	// cec4[7:0] : clkb1_switch[7:0]                                                                                        
  LCD_WriteCommand(0xCE,0xc4); LCD_WriteData(0x00);	// cec5[7:0] : clkb1_extend[7:0]                                                                                        
  LCD_WriteCommand(0xCE,0xc5); LCD_WriteData(0x00);	// cec6[7:0] : clkb1_tchop[7:0]                                                                                         
  LCD_WriteCommand(0xCE,0xc6); LCD_WriteData(0x00);	// cec7[7:0] : clkb1_tglue[7:0]                                                                                         
  LCD_WriteCommand(0xCE,0xc7); LCD_WriteData(0x30);	// cec8[7:0] : clkb2_width[3:0], clkb2_shift[11:8]                                                                      
  LCD_WriteCommand(0xCE,0xc8); LCD_WriteData(0x02);	// cec9[7:0] : clkb2_shift[7:0]                                                                                         
  LCD_WriteCommand(0xCE,0xc9); LCD_WriteData(0x03);	// ceca[7:0] : clkb2_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]                                                
  LCD_WriteCommand(0xCE,0xca); LCD_WriteData(0x26);	// cecb[7:0] : clkb2_switch[7:0]                                                                                        
  LCD_WriteCommand(0xCE,0xcb); LCD_WriteData(0x00);	// cecc[7:0] : clkb2_extend                                                                                             
  LCD_WriteCommand(0xCE,0xcc); LCD_WriteData(0x00);	// cecd[7:0] : clkb2_tchop                                                                                              
  LCD_WriteCommand(0xCE,0xcd); LCD_WriteData(0x00);	// cece[7:0] : clkb2_tglue                                                                                              
                                                                                                                                                                  
  //CEDx : clkb3, clkb4                                                                                                                                     
  LCD_WriteCommand(0xCE,0xd0); LCD_WriteData(0x30);	// ced1[7:0] : clkb3_width[3:0], clkb3_shift[11:8]                                                                      
  LCD_WriteCommand(0xCE,0xd1); LCD_WriteData(0x03);	// ced2[7:0] : clkb3_shift[7:0]                                                                                         
  LCD_WriteCommand(0xCE,0xd2); LCD_WriteData(0x03);	// ced3[7:0] : clkb3_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]                                                
  LCD_WriteCommand(0xCE,0xd3); LCD_WriteData(0x27);	// ced4[7:0] : clkb3_switch[7:0]                                                                                        
  LCD_WriteCommand(0xCE,0xd4); LCD_WriteData(0x00);	// ced5[7:0] : clkb3_extend[7:0]                                                                                        
  LCD_WriteCommand(0xCE,0xd5); LCD_WriteData(0x00);	// ced6[7:0] : clkb3_tchop[7:0]                                                                                         
  LCD_WriteCommand(0xCE,0xd6); LCD_WriteData(0x00);	// ced7[7:0] : clkb3_tglue[7:0]                                                                                         
  LCD_WriteCommand(0xCE,0xd7); LCD_WriteData(0x30);	// ced8[7:0] : clkb4_width[3:0], clkb4_shift[11:8]                                                                      
  LCD_WriteCommand(0xCE,0xd8); LCD_WriteData(0x04);	// ced9[7:0] : clkb4_shift[7:0]                                                                                         
  LCD_WriteCommand(0xCE,0xd9); LCD_WriteData(0x03);	// ceda[7:0] : clkb4_sw_tg, odd_high, flat_head, flat_tail, switch[11:8]                                                
  LCD_WriteCommand(0xCE,0xda); LCD_WriteData(0x28);	// cedb[7:0] : clkb4_switch[7:0]                                                                                        
  LCD_WriteCommand(0xCE,0xdb); LCD_WriteData(0x00);	// cedc[7:0] : clkb4_extend                                                                                             
  LCD_WriteCommand(0xCE,0xdc); LCD_WriteData(0x00);	// cedd[7:0] : clkb4_tchop                                                                                              
  LCD_WriteCommand(0xCE,0xdd); LCD_WriteData(0x00);	// cede[7:0] : clkb4_tglue                                                                                              
                                                                                                                                                            
  //CFCx :                                                                                                                                                  
  LCD_WriteCommand(0xCF,0xc0); LCD_WriteData(0x00);	// cfc1[7:0] : eclk_normal_width[7:0]                                                                                   
  LCD_WriteCommand(0xCF,0xc1); LCD_WriteData(0x00);	// cfc2[7:0] : eclk_partial_width[7:0]                                                                                  
  LCD_WriteCommand(0xCF,0xc2); LCD_WriteData(0x00);	// cfc3[7:0] : all_normal_tchop[7:0]                                                                                    
  LCD_WriteCommand(0xCF,0xc3); LCD_WriteData(0x00);	// cfc4[7:0] : all_partial_tchop[7:0]                                                                                   
  LCD_WriteCommand(0xCF,0xc4); LCD_WriteData(0x00);	// cfc5[7:0] : eclk1_follow[3:0], eclk2_follow[3:0]                                                                     
  LCD_WriteCommand(0xCF,0xc5); LCD_WriteData(0x00);	// cfc6[7:0] : eclk3_follow[3:0], eclk4_follow[3:0]                                                                     
  LCD_WriteCommand(0xCF,0xc6); LCD_WriteData(0x00);	// cfc7[7:0] : 00, vstmask, vendmask, 00, dir1, dir2 (0=VGL, 1=VGH)                                                     
  LCD_WriteCommand(0xCF,0xc7); LCD_WriteData(0x00);	// cfc8[7:0] : reg_goa_gnd_opt, reg_goa_dpgm_tail_set, reg_goa_f_gating_en, reg_goa_f_odd_gating, toggle_mod1, 2, 3, 4  
  LCD_WriteCommand(0xCF,0xc8); LCD_WriteData(0x00);	// cfc9[7:0] : duty_block[3:0], DGPM[3:0]                                                                               
  LCD_WriteCommand(0xCF,0xc9); LCD_WriteData(0x00);	// cfca[7:0] : reg_goa_gnd_period[7:0]                                                                                  
                        
  //CFDx :              
  LCD_WriteCommand(0xCF,0xd0);	LCD_WriteData(0x00);// cfd1[7:0] : 0000000, reg_goa_frame_odd_high
  // PARAMETER 1        
                        
  //--------------------------------------------------------------------------------
  //		initial setting 3 < Panel setting >
  //--------------------------------------------------------------------------------
  // cbcx               
  LCD_WriteCommand(0xCB,0xc0); LCD_WriteData(0x00);	//cbc1[7:0] : enmode H-byte of sig1  (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xc1); LCD_WriteData(0x00);	//cbc2[7:0] : enmode H-byte of sig2  (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xc2); LCD_WriteData(0x00);	//cbc3[7:0] : enmode H-byte of sig3  (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xc3); LCD_WriteData(0x00);	//cbc4[7:0] : enmode H-byte of sig4  (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xc4); LCD_WriteData(0x04);	//cbc5[7:0] : enmode H-byte of sig5  (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xc5); LCD_WriteData(0x04);	//cbc6[7:0] : enmode H-byte of sig6  (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xc6); LCD_WriteData(0x04);	//cbc7[7:0] : enmode H-byte of sig7  (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xc7); LCD_WriteData(0x04);	//cbc8[7:0] : enmode H-byte of sig8  (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xc8); LCD_WriteData(0x04);	//cbc9[7:0] : enmode H-byte of sig9  (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xc9); LCD_WriteData(0x04);	//cbca[7:0] : enmode H-byte of sig10 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xca); LCD_WriteData(0x00);	//cbcb[7:0] : enmode H-byte of sig11 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xcb); LCD_WriteData(0x00);	//cbcc[7:0] : enmode H-byte of sig12 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xcc); LCD_WriteData(0x00);	//cbcd[7:0] : enmode H-byte of sig13 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xcd); LCD_WriteData(0x00);	//cbce[7:0] : enmode H-byte of sig14 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xce); LCD_WriteData(0x00);	//cbcf[7:0] : enmode H-byte of sig15 (pwrof_0, pwrof_1, norm, pwron_4 )                          
                                                                                                                                     
  // cbdx                                                                                                                            
  LCD_WriteCommand(0xCB,0xd0); LCD_WriteData(0x00);	//cbd1[7:0] : enmode H-byte of sig16 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xd1); LCD_WriteData(0x00);	//cbd2[7:0] : enmode H-byte of sig17 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xd2); LCD_WriteData(0x00);	//cbd3[7:0] : enmode H-byte of sig18 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xd3); LCD_WriteData(0x00);	//cbd4[7:0] : enmode H-byte of sig19 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xd4); LCD_WriteData(0x00);	//cbd5[7:0] : enmode H-byte of sig20 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xd5); LCD_WriteData(0x00);	//cbd6[7:0] : enmode H-byte of sig21 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xd6); LCD_WriteData(0x00);	//cbd7[7:0] : enmode H-byte of sig22 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xd7); LCD_WriteData(0x00);	//cbd8[7:0] : enmode H-byte of sig23 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xd8); LCD_WriteData(0x00);	//cbd9[7:0] : enmode H-byte of sig24 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xd9); LCD_WriteData(0x04);	//cbda[7:0] : enmode H-byte of sig25 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xda); LCD_WriteData(0x04);	//cbdb[7:0] : enmode H-byte of sig26 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xdb); LCD_WriteData(0x04);	//cbdc[7:0] : enmode H-byte of sig27 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xdc); LCD_WriteData(0x04);	//cbdd[7:0] : enmode H-byte of sig28 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xdd); LCD_WriteData(0x04);	//cbde[7:0] : enmode H-byte of sig29 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xde); LCD_WriteData(0x04);	//cbdf[7:0] : enmode H-byte of sig30 (pwrof_0, pwrof_1, norm, pwron_4 )                          
                                                             
  // cbex                                                    
  LCD_WriteCommand(0xCB,0xe0); LCD_WriteData(0x00);	//cbe1[7:0] : enmode H-byte of sig31 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xe1); LCD_WriteData(0x00);	//cbe2[7:0] : enmode H-byte of sig32 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xe2); LCD_WriteData(0x00);	//cbe3[7:0] : enmode H-byte of sig33 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xe3); LCD_WriteData(0x00);	//cbe4[7:0] : enmode H-byte of sig34 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xe4); LCD_WriteData(0x00);	//cbe5[7:0] : enmode H-byte of sig35 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xe5); LCD_WriteData(0x00);	//cbe6[7:0] : enmode H-byte of sig36 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xe6); LCD_WriteData(0x00);	//cbe7[7:0] : enmode H-byte of sig37 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xe7); LCD_WriteData(0x00);	//cbe8[7:0] : enmode H-byte of sig38 (pwrof_0, pwrof_1, norm, pwron_4 )                          
  LCD_WriteCommand(0xCB,0xe8); LCD_WriteData(0x00);	//cbe9[7:0] : enmode H-byte of sig39 (pwrof_0, pwrof_1, norm, pwron_4 )                                                                                                  
  LCD_WriteCommand(0xCB,0xe9); LCD_WriteData(0x00);	//cbea[7:0] : enmode H-byte of sig40 (pwrof_0, pwrof_1, norm, pwron_4 )                                                                                                  
                                                  
  // cc8x                                         
  LCD_WriteCommand(0xCC,0x80); LCD_WriteData(0x00);	//cc81[7:0] : reg setting for signal01 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0x81); LCD_WriteData(0x00);	//cc82[7:0] : reg setting for signal02 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0x82); LCD_WriteData(0x00);	//cc83[7:0] : reg setting for signal03 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0x83); LCD_WriteData(0x00);	//cc84[7:0] : reg setting for signal04 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0x84); LCD_WriteData(0x0C);	//cc85[7:0] : reg setting for signal05 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0x85); LCD_WriteData(0x0A);	//cc86[7:0] : reg setting for signal06 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0x86); LCD_WriteData(0x10);	//cc87[7:0] : reg setting for signal07 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0x87); LCD_WriteData(0x0E);	//cc88[7:0] : reg setting for signal08 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0x88); LCD_WriteData(0x03);	//cc89[7:0] : reg setting for signal09 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0x89); LCD_WriteData(0x04);	//cc8a[7:0] : reg setting for signal10 selection with u2d mode                                   
                                                                                                                                     
  // cc9x                                                                                                                            
  LCD_WriteCommand(0xCC,0x90); LCD_WriteData(0x00);	//cc91[7:0] : reg setting for signal11 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0x91); LCD_WriteData(0x00);	//cc92[7:0] : reg setting for signal12 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0x92); LCD_WriteData(0x00);	//cc93[7:0] : reg setting for signal13 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0x93); LCD_WriteData(0x00);	//cc94[7:0] : reg setting for signal14 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0x94); LCD_WriteData(0x00);	//cc95[7:0] : reg setting for signal15 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0x95); LCD_WriteData(0x00);	//cc96[7:0] : reg setting for signal16 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0x96); LCD_WriteData(0x00);	//cc97[7:0] : reg setting for signal17 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0x97); LCD_WriteData(0x00);	//cc98[7:0] : reg setting for signal18 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0x98); LCD_WriteData(0x00);	//cc99[7:0] : reg setting for signal19 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0x99); LCD_WriteData(0x00);	//cc9a[7:0] : reg setting for signal20 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0x9a); LCD_WriteData(0x00);	//cc9b[7:0] : reg setting for signal21 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0x9b); LCD_WriteData(0x00);	//cc9c[7:0] : reg setting for signal22 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0x9c); LCD_WriteData(0x00);	//cc9d[7:0] : reg setting for signal23 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0x9d); LCD_WriteData(0x00);	//cc9e[7:0] : reg setting for signal24 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0x9e); LCD_WriteData(0x0B);	//cc9f[7:0] : reg setting for signal25 selection with u2d mode                                   
                                                                                                                                     
  // ccax                                                                                                                            
  LCD_WriteCommand(0xCC,0xa0); LCD_WriteData(0x09);	//cca1[7:0] : reg setting for signal26 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0xa1); LCD_WriteData(0x0F);	//cca2[7:0] : reg setting for signal27 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0xa2); LCD_WriteData(0x0D);	//cca3[7:0] : reg setting for signal28 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0xa3); LCD_WriteData(0x01);	//cca4[7:0] : reg setting for signal29 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0xa4); LCD_WriteData(0x02);	//cca5[7:0] : reg setting for signal20 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0xa5); LCD_WriteData(0x00);	//cca6[7:0] : reg setting for signal31 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0xa6); LCD_WriteData(0x00);	//cca7[7:0] : reg setting for signal32 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0xa7); LCD_WriteData(0x00);	//cca8[7:0] : reg setting for signal33 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0xa8); LCD_WriteData(0x00);	//cca9[7:0] : reg setting for signal34 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0xa9); LCD_WriteData(0x00);	//ccaa[7:0] : reg setting for signal35 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0xaa); LCD_WriteData(0x00);	//ccab[7:0] : reg setting for signal36 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0xab); LCD_WriteData(0x00);	//ccac[7:0] : reg setting for signal37 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0xac); LCD_WriteData(0x00);	//ccad[7:0] : reg setting for signal38 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0xad); LCD_WriteData(0x00);	//ccae[7:0] : reg setting for signal39 selection with u2d mode                                   
  LCD_WriteCommand(0xCC,0xae); LCD_WriteData(0x00);	//ccaf[7:0] : reg setting for signal40 selection with u2d mode                                   
                                                                                                                                     
  // ccbx                                                                                                                            
  LCD_WriteCommand(0xCC,0xb0); LCD_WriteData(0x00);	//ccb1[7:0] : reg setting for signal01 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xb1); LCD_WriteData(0x00);	//ccb2[7:0] : reg setting for signal02 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xb2); LCD_WriteData(0x00);	//ccb3[7:0] : reg setting for signal03 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xb3); LCD_WriteData(0x00);	//ccb4[7:0] : reg setting for signal04 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xb4); LCD_WriteData(0x0D);	//ccb5[7:0] : reg setting for signal05 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xb5); LCD_WriteData(0x0F);	//ccb6[7:0] : reg setting for signal06 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xb6); LCD_WriteData(0x09);	//ccb7[7:0] : reg setting for signal07 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xb7); LCD_WriteData(0x0B);	//ccb8[7:0] : reg setting for signal08 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xb8); LCD_WriteData(0x02);	//ccb9[7:0] : reg setting for signal09 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xb9); LCD_WriteData(0x01);	//ccba[7:0] : reg setting for signal10 selection with d2u mode                                   
                                                                                                                                     
  // cccx                                                                                                                            
  LCD_WriteCommand(0xCC,0xc0); LCD_WriteData(0x00);	//ccc1[7:0] : reg setting for signal11 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xc1); LCD_WriteData(0x00);	//ccc2[7:0] : reg setting for signal12 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xc2); LCD_WriteData(0x00);	//ccc3[7:0] : reg setting for signal13 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xc3); LCD_WriteData(0x00);	//ccc4[7:0] : reg setting for signal14 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xc4); LCD_WriteData(0x00);	//ccc5[7:0] : reg setting for signal15 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xc5); LCD_WriteData(0x00);	//ccc6[7:0] : reg setting for signal16 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xc6); LCD_WriteData(0x00);	//ccc7[7:0] : reg setting for signal17 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xc7); LCD_WriteData(0x00);	//ccc8[7:0] : reg setting for signal18 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xc8); LCD_WriteData(0x00);	//ccc9[7:0] : reg setting for signal19 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xc9); LCD_WriteData(0x00);	//ccca[7:0] : reg setting for signal20 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xca); LCD_WriteData(0x00);	//cccb[7:0] : reg setting for signal21 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xcb); LCD_WriteData(0x00);	//cccc[7:0] : reg setting for signal22 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xcc); LCD_WriteData(0x00);	//cccd[7:0] : reg setting for signal23 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xcd); LCD_WriteData(0x00);	//ccce[7:0] : reg setting for signal24 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xce); LCD_WriteData(0x0E);	//cccf[7:0] : reg setting for signal25 selection with d2u mode                                   
                                                                                                                                     
  // ccdx                                                                                                                            
  LCD_WriteCommand(0xCC,0xd0); LCD_WriteData(0x10);	//ccd1[7:0] : reg setting for signal26 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xd1); LCD_WriteData(0x0A);	//ccd2[7:0] : reg setting for signal27 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xd2); LCD_WriteData(0x0C);	//ccd3[7:0] : reg setting for signal28 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xd3); LCD_WriteData(0x04);	//ccd4[7:0] : reg setting for signal29 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xd4); LCD_WriteData(0x03);	//ccd5[7:0] : reg setting for signal30 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xd5); LCD_WriteData(0x00);	//ccd6[7:0] : reg setting for signal31 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xd6); LCD_WriteData(0x00);	//ccd7[7:0] : reg setting for signal32 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xd7); LCD_WriteData(0x00);	//ccd8[7:0] : reg setting for signal33 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xd8); LCD_WriteData(0x00);	//ccd9[7:0] : reg setting for signal34 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xd9); LCD_WriteData(0x00);	//ccda[7:0] : reg setting for signal35 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xda); LCD_WriteData(0x00);	//ccdb[7:0] : reg setting for signal36 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xdb); LCD_WriteData(0x00);	//ccdc[7:0] : reg setting for signal37 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xdc); LCD_WriteData(0x00);	//ccdd[7:0] : reg setting for signal38 selection with d2u mode                                   
  LCD_WriteCommand(0xCC,0xdd); LCD_WriteData(0x00);	//ccde[7:0] : reg setting for signal39 selection with d2u mode                    
  LCD_WriteCommand(0xCC,0xde); LCD_WriteData(0x00);	//ccdf[7:0] : reg setting for signal40 selection with d2u mode                    
                        
  ///=====================,0x========
  LCD_WriteCommand(0x3A,0x00); LCD_WriteData(0x77);     //RGB 32bits D[23:0]

  LCD_WriteCommand(0x11,0x00);
  SYS_Delay(20);	        
  LCD_WriteCommand(0x29,0x00);
  SYS_Delay(20);     
  LCD_WriteCommand(0x2c,0x00);
  SYS_Delay(20);
}

void LCD_RGB_Init_OTM8009A(void){
  // 下列大多数代码来自公版文件且没什么可读性!
  
  // RGB 显示信号 (来自公版文件)
  PORTC->FUNC0 &= ~0x0000FFFF;
  PORTC->FUNC0 |=  0x00006665;
  PORTC->FUNC1 &= ~0x00FFFF00;
  PORTC->FUNC1 |=  0x00634400;

  PORTA->FUNC0 &= ~0x00000F00;
  PORTA->FUNC0 |=  0x00000400;
  PORTA->FUNC1 &= ~0xFF00FF00;
  PORTA->FUNC1 |=  0x22004500;

  PORTB->FUNC0 &= ~0x00FFFF00;
  PORTB->FUNC0 |=  0x00667600;
  PORTB->FUNC1 &= ~0xF0000000;
  PORTB->FUNC1 |=  0x70000000;

  PORTC->FUNC0 &= ~0x0000FFFF;
  PORTC->FUNC0 |=  0x00006665;
  PORTC->FUNC1 &= ~0x00FFFF00;
  PORTC->FUNC1 |=  0x00634400;
  
  SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_LCD_Pos);
  
  // RGB 时序配置 (也来自公版文件驱动800x480屏幕)
  LCD->CR = 0x00008384;
  LCD->CRH = 0x4DDF1301;
  LCD->CRV = 0x3F1F0F09;
  LCD->BGC = 0x0000FFFF;
  
  // 图层初始化
  LCD->L[0].LCR = 0x000001FF;
  LCD->L[0].WHP = 0x01DF0000;
  LCD->L[0].WVP = 0x031F0000;
  LCD->L[0].ADDR = 0x80000000; // 后面会被LVGL驱动覆盖
  LCD->L[0].LLEN = 480 - 1;
  
  LCD->IF = 1;
  LCD->IE = 1;
  
  NVIC_EnableIRQ(LCD_IRQn);
}

void LCD_Handler(void)
{
  LCD->IF = 1;
  LCD->START |= (1 << LCD_START_GO_Pos);
}