#include<lpc21xx.h>
#include"lcd_header.h"
#include"uart_header.h"

#define delay for(i=0;i<65000;i++);   // This delay seems unnecessary, we’ll remove it and handle delay with more precision
#define trig 1 << 8  // P0.8 for trigger
#define echo (IO0PIN & (1 << 9))  // P0.9 for echo (EINT3)
#define buzzer 1<<21

void delay_us(unsigned int ms)
{
    T0PR = 59;  // Prescale for 1us delay
    T0TCR = 0x01;   // Enable Timer
    while (T0TC < ms);  // Wait for the required delay in microseconds
    T0TCR = 0x03;   // Reset Timer
    T0TCR = 0x00;   // Disable Timer
}

void ultrasonic_init(void);
void send_pulse(void);
unsigned int get_range(void);

int main()
{
    unsigned int range = 0;
    unsigned char water_level_low_flag = 0;  
    unsigned char water_level_full_flag = 0; 

    VPBDIV = 0x01;  
    IODIR0 |= buzzer;
    ultrasonic_init();  
    LCD_INIT();  
    motor_init();
    LCD_String("W.level:");  
    uart_init();

    while (1)
    {
        LCD_COMMAND(0x89);
        range = get_range();  
        range = range * 2;    
        
        if (range < 60 && !water_level_low_flag)  
        {
            IOSET0 = buzzer; 
            motor_on(); 
            gsm_msg("water level low");  
            LCD_INTEGER2(range);  
            water_level_low_flag = 1;  
            delay_ms(500);  
        }
        else if (range > 300 && !water_level_full_flag) 
        {
            IOCLR0 = buzzer;  
            motor_off();  
            gsm_msg("water level full"); 
            LCD_INTEGER2(range);  
            water_level_full_flag = 1; 
            delay_ms(500);  
        }

        else if (range >= 60 && range <= 300)
        {
            IOCLR0 = buzzer; 
            motor_off();  
            LCD_INTEGER2(range);  
            water_level_low_flag = 0;  
            water_level_full_flag = 0;  
            delay_ms(500);
        }

        delay_ms(500);  // Delay for 1 second between readings
    }
}

void ultrasonic_init()
{
    IODIR0 |= trig;  // Set trigger pin as output
    T0TCR = 0;  // Stop timer initially
    T0PR = 59;   // Prescaler for 1us ticks (for accurate timing)
}

void send_pulse()
{
    T0TC = T0PC = 0;  // Reset timer
    IOSET0 = trig;  // Send trigger pulse (set trig pin high)
    delay_us(10);    // Wait for 10us to send pulse
    IOCLR0 = trig;  // Set trig pin low again
}

unsigned int get_range()
{
    unsigned int time = 0;
    send_pulse();  // Send trigger pulse

    // Wait for the echo signal to go high
    while (!echo);

    // Start measuring the time when echo goes high
    T0TCR = 0x01;  // Start Timer
    while (echo);   // Wait for echo to go low
    T0TCR = 0x00;   // Stop Timer

    // The value of T0TC is the duration of the pulse in microseconds
    time = T0TC;

    // Convert time to distance (distance = time * speed_of_sound / 2)
    // speed of sound = 343 meters per second (or 34300 cm/s)
    if (time < 38000) {  // Ignore invalid ranges
        time = time / 59;  // Adjust to actual distance
    } else {
        time = 0;  // No valid range if time is too large
    }

    return time;  // Return the distance in cm
}
