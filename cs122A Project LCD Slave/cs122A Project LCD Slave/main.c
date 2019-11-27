//////////////////////////////////////////////////////////////////////////
#include <avr/io.h>
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

//////////////////////////////////////////////////////////////////////////
/*Variables Used for the Sensor State*/
#define one 0x03
#define two 0x0C
#define three 0x30
#define four 0xC0
char PORT_B[]= {0,0,0,0,0,0,0,0,0,0,0,0,  two, two,four, four, two, two, four, four, two, two, 0, two, two, two,two,one,one,one,one, 0,0,0,0,0,0,0,0,0,0,one, one,three, three, one, one, three, three, one, one, 0,  one, one, two,two, three,three,three,three, 0,0,0,0,0,0,0,0,0,0,three,three, two,two,three,three,two,two,three,three,0, four,four, four,0,one,one,0,0,one,one,0,0,one,one,0,0,one,one,0,0,one,one,0,0,one,one,one,one
	,0,0,0,0,0,0,0,0,0,0,0

};			//0x01, 0x03
char PORT_C[8] = {1,2,4,8,16,32,64,128}; //pin values of a port 2^0,2^1,2^2??2^7
unsigned char score = 0;

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
				score++;
				LCD_Cursor(14);
				LCD_WriteData(score +'0');
				eeprom_update_byte((uint8_t*) 1, (uint8_t) score);
			}
			else{

				//PORTC = PORT_C[1];//rows
				//PORTB = ~PORT_B[8];//column
				//PORTC = ~0xFF;
				//PORTB = 0xFF;
				LCD_Cursor(14);
				LCD_WriteData(score +'0');
				state = screen_wait;
			}
			
			return state;		
		break;
		
		case screen_increase:
			if(receivedData == 0x01){
				receivedData = 0x00;				
				state = screen_increase;
			}
			else{
				state = screen_wait;
			}
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
	
	DDRB = 0xFF; PORTB = 0x00;//PORTB as output for the matrix: Coloum
	DDRC = 0xFF; PORTC = 0x00;//PORTC as output rows
	if(eeprom_read_byte((uint8_t*)1) == 255) {
		eeprom_update_byte((uint8_t*)1, (uint8_t) 0);
	}
	score = eeprom_read_byte((uint8_t*)1);
	
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
	LCD_DisplayString(1, "Okay B0omer: ");
	LCD_WriteData(score +'0');
	
	
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