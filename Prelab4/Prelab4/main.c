/**************************
Universidad del Valle de Guatemala
Programación de Microcrontroladores
Proyecto: Lab 4
Hardware: ATMEGA328p
Created: 05/04/2024 08:11:46
Author : James Ramírez
***************************/

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdint.h>

void setup(void);
void delay(uint8_t ciclos);
void initADC(void);
void multiplexacion(void);
void ADC_start();
volatile uint8_t adcValueHigh = 0;
volatile uint8_t adcValueLow = 0;
uint8_t contador = 0;

const uint8_t display[16] = {0x7E, 0x0C, 0xB6, 0x9E, 0xCC, 0xDA, 0xFA, 0x0E, 0xFE, 0xDE, 0xEE, 0xF8, 0x72, 0xBC, 0xF2, 0xE2};


int main(void)
{
	cli();
	
	//Incremento
	DDRB &= ~(1<<DDB3);		// PB1 como entrada
	PORTB |= (1<<PORTB3);	// Activamos Pull-Up

	//Decremento
	DDRB &= ~(1<<DDB2);		// PB2 como entrada
	PORTB |= (1<<PORTB2);	// Activamos Pull-Up
	
	DDRD=0xFF;
	
	// Habilitar las interrupciones por cambio de pin
	PCICR |= (1<<PCIE0);
	PCMSK0 |= (1<<PCINT2) | (1<<PCINT3);
	
	PORTD = 0;
	UCSR0B = 0;
	initADC();
	
	sei();
	
	// Limpiar la bandera de interrupción PCINT0
	PCIFR |= (1<<PCIF0);
	
	
	
	
	while (1)
	{
		//INICIO DE LA LECTURA ADC
		ADCSRA |= (1<<ADSC);
		
		if (ADCH>contador)
		{
		PORTB |= (1<<PORTB5);
		}
		else
		{
		PORTB &= ~(1<<PORTB5);	
		}
		
		//MULTIPLEXACIÓN
		
		//SE MUESTRA EL PRIMER DISPLAY CON LOS 4 BITS MÁS SIGNIFICATIVOS
		PORTB |= (1<<PORTB0);
		PORTD = display[adcValueHigh];
		_delay_ms(5);
		PORTB &= ~(1<<PORTB0);
		
		//SE MUESTRA EL SEGUNDO DISPLAY CON LOS 4 BITS MENOS SIGNIFICATIVOS
		PORTB |= (1<<PORTB1);
		PORTD = display[adcValueLow];
		_delay_ms(5);
		PORTB &= ~(1<<PORTB1);
		
		//SE MUESTRA EL VALOR DEL CONTADOR DE 8 BITS
		PORTB |= (1<<PORTB4);
		PORTD = contador;
		_delay_ms(5);
		PORTB &= ~(1<<PORTB4);
	
	}
	
	return 0;
}

void initADC(void){
	ADMUX = 6;
	//REFERENCIA AVCC = 5V
	ADMUX |= (1<<REFS0);
	ADMUX &= ~(1<<REFS1);
	
	//JUSTIFICACION A LA IZQUIERDA
	ADMUX |= (1<<ADLAR);
	
	ADCSRA = 0;
	
	//HABILITA INTERRUPCION
	ADCSRA	|= (1<<ADIE);
	
	//HABILITA PREESCALER A 128 - 125kHz
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	
	//HABILITA EL ADC
	ADCSRA |= (1<<ADEN);
	
}

ISR(ADC_vect){
	//SE ALMACENA EL VALOR DE ADC Y SE BORRA EL VALOR DE LA BANDERA
	uint16_t adc_val16 = ADC;
	uint8_t adcValue = (adc_val16 >> 8);

	ADCSRA |= (1<<ADIF);
	
	//SE SEPARA EL VALOR DEL ADC EN 4 BITS CADA UNO
	adcValueHigh = adcValue >> 4;
	adcValueLow = adcValue & 0x0F;
	ADCSRA |= (1<<ADSC);
}


// Interrupción para el pulsador de incremento
ISR(PCINT0_vect) {
	if (!(PINB & (1<<PB2))) {
		_delay_ms(5); // Antirrebote
		if (!(PINB & (1<<PB2))) {
			contador++;
		}
	}
	// Interrupción para el pulsador de decremento
	if (!(PINB & (1<<PB3))) {
		_delay_ms(5); // Antirrebote
		if (!(PINB & (1<<PB3))) {
			contador--;
		}
	}

}




