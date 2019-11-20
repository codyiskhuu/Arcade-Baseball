//////////////////////////////////////////////////////////////////////////
#include <avr/io.h>
#include <SPI.h>
#include <timer.h>
#include <lcd.h>
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

//////////////////////////////////////////////////////////////////////////
/*Variables Used for the Sensor State*/



enum SM1_states{screen_Init, screen_wait, screen_increase, screen_decrease};
int SMLCD(int state){
	switch(state){
		case screen_Init:
			state = screen_wait;
			return state;
		break;
		
		case screen_wait:
			if(receivedData == 0x01){
				state = screen_increase;
			}
			else{
				LCD_DisplayString(1, "Taco Bell");
				state = screen_wait;
			}
			
			return state;		
		break;
		
		case screen_increase:
			state = screen_wait;
			return state;
		break;
		
		case screen_decrease:
			state = screen_wait;
			return state;
		break;
	};

	switch(state){
		case screen_Init:
		break;
		
		case screen_wait:
		break;
		
		case screen_increase:
		break;
		
		case screen_decrease:
		break;
	};

	return 0;
}


//////////////////////////////////////////////////////////////////////////

int main(void)
{
	
	DDRA = 0xFF; PORTA = 0x00;//slave //will as a output rn
	//DDRB = 0x00; PORTB = 0xFF;//Input for the Sensor //at the moment we are using a button
	//DDRC = 0xF0; PORTC = 0x0F;//keypad
	DDRD = 0xFF; PORTD = 0x00;//lcd data bus
	
	static task task1;
	task *tasks[] = { &task1};//, &task2};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	
	//task1 - for the sensor input
	task1.state = screen_Init;
	task1.period = 100;
	task1.elapsedTime = task1.period;
	task1.TickFct = &SMLCD;


/*Initializations*/
	TimerSet(100);
	TimerOn();
	SPI_SlaveInit();
	LCD_init();

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