/*
 * 122A Project Motion Sensor.c
 *
 * Created: 11/7/2019 6:27:12 PM
 * Author : crazy
 */ 
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
#define sensor1 (PINB & 0x01)//one of the outfield sensors // dont do ~PINB because it will be high all the time
#define sensor2 (PINB & 0x02)//the sensor is at the strike zone

static unsigned char score = 0x01;
static unsigned char strikes = 0x02;

enum SM1_states{sensor_Init, sensor_wait, sensor_on, sensor_decrement}state;
int SMSensor(){
	switch(state){
		case sensor_Init:
			state = sensor_wait;
			return state;
		break;
		
		case sensor_wait:
			if(sensor1){//when the "outfield" sensor detects the ball increase the score
				state = sensor_on;
				SPI_MasterTransmit(score);//should send codes to tell the LCD whether its a score or a strike
			}
			else if(sensor2){//when the "strike zone" sensor detects the ball gives a strike
				state = sensor_decrement;
				SPI_MasterTransmit(strikes);//should send codes to tell the LCD whether its a score or a strike
			}
			else{
				state = sensor_wait;	
			}
			

			return state;
		break;
		
		case sensor_on:
			if(sensor1){
				PORTA = 0x01;
				state = sensor_on;
			}
			else{
				PORTA = 0x00;
				state = sensor_wait;
			}
			return state;
		break;

		case sensor_decrement:
			if(sensor2){
				PORTA = 0x01;
				state = sensor_decrement;
			}
			else{
				PORTA = 0x00;
				state = sensor_wait;
			}
			return state;

		break;
	};

	switch(state){
		case sensor_Init:
		break;
		
		case sensor_wait:
			
		break;
		
		case sensor_on:
			
		break;
		
		case sensor_decrement:
			
		break;
		
	};

	return 0;
}


//////////////////////////////////////////////////////////////////////////

int main(void)
{

	DDRA = 0xFF; PORTA = 0x00;//slave //will as a output rn
	DDRB = 0x00; PORTB = 0xFF;//Input for the Sensor //at the moment we are using a button
//	DDRC = 0xF0; PORTC = 0x0F;//keypad
//	DDRD = 0xFF; PORTD = 0x00;//lcd data bus

	static task task1;
	task *tasks[] = { &task1};//, &task2};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	
	//task1 - for the sensor input
	task1.state = sensor_Init;
	task1.period = 100;
	task1.elapsedTime = task1.period;
	task1.TickFct = &SMSensor;

/*Initializations*/
	TimerSet(100);
	TimerOn();
	SPI_MasterInit();

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

/*
int ledPin = 13;  // LED on Pin 13 of Arduino
int pirPin = 7; // Input for HC-S501

int pirValue; // Place to store read PIR Value


void setup() {
	pinMode(ledPin, OUTPUT);//makes this input
	pinMode(pirPin, INPUT);//makes this output
	digitalWrite(ledPin, LOW);//make the ledPin low
}
void loop() {
	pirValue = digitalRead(pirPin); //wat
	digitalWrite(ledPin, pirValue);

}
*/
/*
basically the ~PINB 0x01 is initially 0 so when motion is detected it is high
*/