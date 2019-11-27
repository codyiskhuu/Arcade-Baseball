/*
 * 122A Project Bluetooth.c
 *
 * Created: 11/21/2019 6:22:40 PM
 * Author : crazy
 */ 

#include <avr/io.h>
#include "usart_ATmega1284.h"
#include <SPI.h>
#include <timer.h>
#include <lcd.h>
#include <avr/eeprom.h> 
//////////////////////////////////////////////////////////////////////////


/*Timer*/
//////////////////////////////////////////////////////////////////////////
//Struct for Tasks represent a running process in our simple real-time operating system
typedef struct _task{
	// Tasks should have members that include: state, period,
	//a measurement of elapsed time, and a function pointer.
	signed 	 char state; 		//Task's current state
	unsigned long int period; 		//Task period
	unsigned long int elapsedTime; 	//Time elapsed since last task tick
	int (*TickFct)(int); 		//Task tick function
} task;
//////////////////////////////////////////////////////////////////////////

unsigned char First=0;// for the EEprom checking
unsigned char data_in;
#define button ~PINB & 0x01

//////////////////////////////////////////////////////////////////////////
/*Variables Used for the Sensor State*/
enum SM1_states{bluetooth_Init, bluetooth_wait, bluetooth_receive, bluetooth_dum}state_blue;
int SMbluetooth(){
	switch(state_blue){
		case bluetooth_Init:
			state_blue = bluetooth_wait;
		break;
		
		case bluetooth_wait:
			
			if(button){
				state_blue = bluetooth_receive;
			}
			else if(!USART_HasReceived(0)){
				state_blue = bluetooth_wait;
	//			PORTA = 0x00;
			}
			else{
				state_blue = bluetooth_dum;
				PORTA = 0x00;	
			}
			
		break;
		
		case bluetooth_receive:
			if(button){
				PORTA = 0x00;	
				state_blue = bluetooth_receive;
			}
			else{
				PORTA = 0x01;
				state_blue = bluetooth_wait;
			}
				
		break;
		
		case bluetooth_dum:
			state_blue = bluetooth_wait;
			PORTA = 0x01;
			USART_Flush(0);
			
		break;
	};
			//	eeprom_update_byte((uint8_t*) 1, (uint8_t) First);
			//	USART_Flush(1);
	switch(state_blue){
		case bluetooth_Init:
		
		
		break;
		case bluetooth_wait:
		break;
		case bluetooth_receive:
		break;
		case bluetooth_dum:
		
		break;
		
	};

	return 0;
}


//////////////////////////////////////////////////////////////////////////

int main(void)
{

	DDRA = 0xFF; PORTA = 0x00; //PORTA  is an output
	DDRB = 0x00; PORTB = 0xFF;// PORTB input
	
	static task task1;
	task *tasks[] = { &task1};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	
	//task1 - for the sensor input
	task1.state = bluetooth_Init;
	task1.period = 100;
	task1.elapsedTime = task1.period;
	task1.TickFct = &SMbluetooth;

	/*Initializations*/
	TimerSet(100);
	TimerOn();
	//SPI_MasterInit();
	
	initUSART(0);//using usart port 0
	PORTA = 0x01;
	//////////////////////////////////////////////////////////////////////////
	unsigned short i;
	while(1){
		for (i = 0; i < numTasks; i++){
			if (tasks[i]->elapsedTime == tasks[i]->period){
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 100; //value is gcd
		}
		while(!TimerFlag);
		TimerFlag = 0;
		
		
	}
	
	// Error: Program should not exit!
	return 0;
}
