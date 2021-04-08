#define F_CPU 1000000

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

ISR(INT1_vect) {
	PORTC = ~PORTC; //static mode: 0, rotating mode: 1
}

char rotateRight(char x) {
	char lsb = x & 1;
	x = (x >> 1) & 0b01111111;
	x = x | (lsb << 7);
	return x;
}

int main(void) {
	DDRA = 0xff;
	DDRB = 0xff;
	DDRC = 0x01;
	
	GICR = 1 << INT1;
	MCUCR = 1 << ISC11;
	sei();
	
	PORTC = 0;
	char col = 0b00000100;
	char row = 0xff;
	char row1 = 0b11100000; //left part of symbol E (1st column)
	char row2 = 0b11101010; //right part of symbol E (2nd & 3rd column)

    while(1) {
		for(int k=0; k<50; k++) {
			if(col == 0b00000100) {
				row = row1;
			} else {
				row = row2;
			}
			PORTA = col;
			PORTB = row;
			col = col << 1;
			if(col > 0b00010000) {
				col = 0b00000100;
			}
			_delay_ms(10);	
		}
		if(PORTC) {
			row1 = rotateRight(row1);
			row2 = rotateRight(row2);	
		}
	}
}

