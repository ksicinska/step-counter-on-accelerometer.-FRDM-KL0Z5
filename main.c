/*-------------------------------------------------------------------------------------
					Programowanie Systemów Wbudowanych 
					projekt - licznik kroków;
					autor: Karolina Sicińska
----------------------------------------------------------------------------*/

#include "frdm_bsp.h" 
#include "lcd1602.h"
#include "leds.h" 
#include "stdio.h"
#include "math.h"
#include "i2c.h"
#define	ZYXDR_Mask	1<<3	// Maska bitu ZYXDR w rejestrze STATUS

static uint8_t arrayXYZ[6];
static uint8_t sens;
static uint8_t status;
double X, Y, Z;
uint32_t kroki=0;				
uint8_t sekunda=0;			
uint8_t sekunda_OK=0;		

int main (void)
{ 
	char display[]={0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};
	
	LED_Init();
	LCD1602_Init();	// Tu jest również inicjalizacja portu I2C0
	LCD1602_Backlight(TRUE);

	sens=0;	// Wybór czułości: 0 - 2g; 1 - 4g; 2 - 8g
	I2C_WriteReg(0x1d, 0x2a, 0x0);	// ACTIVE=0 - stan czuwania
	I2C_WriteReg(0x1d, 0xe, sens);	 		// Ustaw czułość zgodnie ze zmienną sens
	I2C_WriteReg(0x1d, 0x2a, 0x29);	 		// ACTIVE=1 - stan aktywny


	while(kroki>0)		
	{
		if(sekunda_OK)
		{
			kroki+=1;			
			sekunda_OK=0;
		}
	}
	kroki=0;
	LCD1602_ClearAll();
	sprintf(display,"kroki=%02d",kroki);	// Wyświetl stan początkowy licznika czasu
	LCD1602_Print(display);
	
	while(1)
	{
		I2C_ReadReg(0x1d, 0x0, &status);
		status&=ZYXDR_Mask;
		if(status)	// Czy dane gotowe do odczytu?
		{
			I2C_ReadRegBlock(0x1d, 0x1, 6, arrayXYZ);
			X=((double)((int16_t)((arrayXYZ[0]<<8)|arrayXYZ[1])>>2)/(4096>>sens));
			Y=((double)((int16_t)((arrayXYZ[2]<<8)|arrayXYZ[3])>>2)/(4096>>sens));
			Z=((double)((int16_t)((arrayXYZ[4]<<8)|arrayXYZ[5])>>2)/(4096>>sens));
			
			if(fabs(X)>1.1)
			{
					if(fabs(X)<1.4)
			{
						kroki+=1;
						DELAY(100);
		
			}
		 }
				
			if(fabs(Y)>1.1)
			{
				kroki+=1;
				DELAY(100);
			}
			if(fabs(Z)>1.2)
			{
				kroki+=1;
				DELAY(100);
			}
			
			LCD1602_SetCursor(6,0);
			sprintf(display,"%02d",kroki);	
			LCD1602_Print(display);
				
			sekunda_OK=0;
		}
	}

}
