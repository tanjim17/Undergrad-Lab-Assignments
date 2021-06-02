#define F_CPU 1000000

#define D4 eS_PORTD4
#define D5 eS_PORTD5
#define D6 eS_PORTD6
#define D7 eS_PORTD7
#define RS eS_PORTC6
#define EN eS_PORTC7

#define VREF 3.5
#define STEP_SIZE 1024

#include <avr/io.h>
#include <util/delay.h>
#include "lcd.h"

int main(void) {
	DDRD = 0xFF;
	DDRC = 0xFF;

	Lcd4_Init();
	Lcd4_Set_Cursor(1,1);
	Lcd4_Write_String("Voltage:  .");
	
	ADMUX = 0b00100000;
	ADCSRA = 0b10000110;
	
	char digits[] = "0123456789";	
	float voltage;
	int display_voltage;
	int digit;
	int cursor_pos;

	while(1) {
		ADCSRA |= 1 << ADSC;
		while(ADCSRA & (1 << ADSC)) {};
		voltage = ((ADCL >> 6) + (ADCH << 2)) * VREF / 1024.0;
		display_voltage = voltage * 100; //for displaying 2 digits after decimal
		
		cursor_pos = 13;
		Lcd4_Set_Cursor(1, cursor_pos);
		for(int i = 0; i < 3; i++) {
			digit = display_voltage % 10;
			display_voltage /= 10;
			Lcd4_Set_Cursor(1, cursor_pos);
			Lcd4_Write_Char(digits[digit]);
			cursor_pos--;
			if(i == 1) cursor_pos--; //skip the decimal point
		}
	}
}