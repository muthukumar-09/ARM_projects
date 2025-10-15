#include<LPC21XX.h>

#define LCD_D 0xf<<20
#define RS 1<<17
#define RW 1<<18
#define E 1<<19

void delay_ms(int ms)
{
	T0PR = 15000-1;
	T0TCR = 0X01;
  while(T0TC<ms);
	T0TCR = 0X03;
	T0TCR = 0X00;
}
void LCD_COMMAND(unsigned char cmd)
{
	IOPIN1=(cmd&0xf0)<<16;
	IOCLR1=RS;
	IOCLR1=RW;
	IOSET1=E;
	delay_ms(2);
	IOCLR1=E;
	
	IOPIN1=(cmd&0x0f)<<20;
	IOCLR1=RS;
	IOCLR1=RW;
	IOSET1=E;
	delay_ms(2);
	IOCLR1=E;
}
void LCD_INIT(void)
{
	PINSEL1 |= 0x05000000;              /* ADC function for 0.28 & 0.29        */
	IODIR1|=LCD_D|RS|E|RW;
	LCD_COMMAND(0X01);
	LCD_COMMAND(0X02);
	LCD_COMMAND(0X0C);
	LCD_COMMAND(0X28);
	LCD_COMMAND(0X80);
}
void LCD_Data(unsigned char D)
{
	IOPIN1=(D&0xf0)<<16;
	IOSET1=RS;
	IOCLR1=RW;
	IOSET1=E;
	delay_ms(2);
	IOCLR1=E;
	
	IOPIN1=(D&0x0f)<<20;
	IOSET1=RS;
	IOCLR1=RW;
	IOSET1=E;
	delay_ms(2);
	IOCLR1=E;
}
void LCD_String(unsigned char *s)
{
	while(*s)
	{
	LCD_Data(*s++);
	
	}
}
void LCD_INTEGER(int n)
{
	unsigned char a[5];
	signed char i=0;
	if(n==0)
	{
		LCD_Data('0');
	}
	else
		if(n<0)
		{
			LCD_Data('-');
			n=-n;
		}
		while(n>0)
		{
			a[i++]=n%10;
			n=n/10;
		}
		for(--i;i>=0;i--)
		{
			LCD_Data(a[i]+48);
		}
		
}

 void LCD_FLOAT(float f)
{
	int x=f;
	LCD_INTEGER(x);
	LCD_Data('.');
	f=(f-x)*100;
	LCD_INTEGER(f);
}

void LCD_INTEGER2(int n)
{	
	LCD_Data((n / 100) + 48);  // Hundreds place
    LCD_Data(((n  / 10) % 10) + 48);  // Tens place
    LCD_Data((n % 10) + 48);  // Ones place
    LCD_String(" Ltr");  // Add "cm" to the display
}

