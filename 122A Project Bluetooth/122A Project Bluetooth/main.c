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
/*Global Variable*/
//0x00 will be a null state
unsigned char game_state =0x00; //game state: 0x01 = menu; 0x02 = game; 0x03 = game over screen
unsigned char game_sensors = 0x00;//While Game is in action: 0x10 = Sensor 1; 0x11 = Sensor 2; 0x12 = Sensor 3; 0x14 = Sensor 4;
unsigned char strikes = 0;// reach 3 and game is over
unsigned char score = 0; //maybe will be only in the LCD slave atmega

unsigned char First=0;// for the EEprom checking
unsigned char data_in;

//////////////////////////////////////////////////////////////////////////
/*Defines*/

#define left (~PINB & 0x01) // start/left
#define rightt (~PINC & 0x01) // right
#define motor (~PINC & 0x08)
#define relay (~PINC & 0x10)

#define sensor_1 PINB & 0x04 // outfield left
#define sensor_2 PINB & 0x08 // strike
#define sensor_3 PINC & 0x02 // outfield mid
#define sensor_4 PINC & 0x04 // outfield right

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
unsigned char sensor_counter = 0;
unsigned char out = 0x00;
enum SM1_states{bluetooth_Init, bluetooth_wait, bluetooth_receive, bluetooth_dum}state_blue;
int SMbluetooth(){
	switch(state_blue){
		case bluetooth_Init:
			state_blue = bluetooth_wait;
		break;
		
		case bluetooth_wait:
			/*if(left){//temporary button that allows me to trigger the solenoid 
				sensor_counter = 0;
				out = out | 0x01;//turn the first digit off
				state_blue = bluetooth_receive;
			}*/
			if(relay){
				sensor_counter = 0;	
				out = out | 0x01;
				state_blue = bluetooth_receive;				
			}
			else if(!USART_HasReceived(0)){//when we do not receive a signal from the PI
				USART_Flush(0);//flush USART at pin 0 so we don't stay in the it 
				state_blue = bluetooth_wait;
			}
			else{//we get the signal and trigger the solenoid
				USART_Flush(0);
				out = out | 0x01;
				sensor_counter = 0;	
				state_blue = bluetooth_receive;
			}
			
		break;
		
		case bluetooth_receive:
			if(sensor_counter < 8){//if the signal still there we should wait until its done
				++sensor_counter;
				state_blue = bluetooth_receive;
			}
			else{
				out = out & 0xFE;//turn the first digit off
				state_blue = bluetooth_wait;
			}
				
		break;
		
		case bluetooth_dum://dummy state
			//state_blue = bluetooth_wait;
			//PORTA = 0x01;
			
		break;
	};
			//	eeprom_update_byte((uint8_t*) 1, (uint8_t) First);
			//	USART_Flush(1);
	switch(state_blue){
		case bluetooth_Init:
		
		break;
		case bluetooth_wait:
			PORTA = out;
		break;
		case bluetooth_receive:
			PORTA = out;
		break;
		case bluetooth_dum:
		
		break;
		
	};

	return 0;
}

//////////////////////////////////////////////////////////////////////////
/*Variables Used for the Menu State*/
/*Menu state would basically send information to the Slave Atmega through SPI with specific numbers indicating which state it is*/
/*Menu: 0x01; Game State: 0x02; End Game: 0x03; // 0x00 would be a NULL state 
While Game is in action: 0x10 = Sensor 1; 0x11 = Sensor 2; 0x12 = Sensor 3; 0x14 = Sensor 4; */

enum SM2_states{menu_Init, menu_wait, menu_game, menu_end, menu_highscore, menu_higher, menu_waiter}state_menu;
int SMmenu(){
	switch(state_menu){
		case menu_Init:
			state_menu = menu_wait;
		break;
		
		case menu_wait://this will be the main menu
			if(left & (game_state == 0x01)){//we press the start game button
				
				//out = out | 0x02 ;
				game_state = 0x02;
				SPI_MasterTransmit(game_state);
				state_menu = menu_game;//start the game
			}
			else if(rightt & (game_state == 0x01) ){//go to the high score menu
				//out = out | 0x04;
				game_state = 0x04;
				SPI_MasterTransmit(game_state);
				state_menu = menu_highscore;
			}
		break;
		
		case menu_game://will probably continue to send information to through SPI in this state// game logic here?
			if(strikes == 3){//3 strikes youre out
				game_state = 0x03;
				//SPI_MasterTransmit(game_state);
				state_menu = menu_end;
			}
			else{
				
				state_menu = menu_game;
			}
		break;
		
		case menu_end:
			if(left & game_state == 0x03){
				game_state = 0x01;
				SPI_MasterTransmit(game_state);
				state_menu = menu_waiter;
			}
			else{
				state_menu = menu_end;
			}
		break;
		
		case menu_waiter:
			if(left){
				state_menu = menu_waiter;
			}
			else{
				state_menu = menu_wait;
			}
		break;
		
		case menu_highscore:
			if((left == 0x01) & (game_state == 0x04)){
				//out = out | 0x04;
				game_state = 0x01;
				SPI_MasterTransmit(game_state);
				state_menu = menu_higher;
			}
			else{
				//out = out | 0x02;
				state_menu = menu_highscore;
			}
		break;
		case menu_higher:
			if(left){
				state_menu = menu_higher;
			}
			else{
				state_menu = menu_wait;
			}
		
		break;
		
		
	}
	switch(state_menu){
		case menu_Init:
		break;
		
		case menu_wait:
		PORTA = out;
		break;
		
		case menu_game:
		//PORTA = out;
		break;
		
		case menu_end:
		break;
		
		case menu_highscore:
		break;	
		case menu_higher:
		
		break;
	}
	return 0;
}
/*Menu: 0x01; Game State: 0x02; End Game: 0x03; // 0x00 would be a NULL state
While Game is in action: 0x10 = Sensor 1; 0x11 = Sensor 2; 0x12 = Sensor 3; 0x14 = Sensor 4; */
char outfield[] = {1,1,1,  0,1,1,  1,1,0,  1,0,1,   1,1,0,  1,1,1,    0,0,1, 1,1,1, 1,0,1, 0,1,0, 1,1,1, 0,1,1, 0,1,1, 1,1,1, 1,0,1, 1,1,1, 0,1,1, 0,1,0, 1,0,1, 0,1,0, 1,1,1, 0,0,1, 1,0,0};
unsigned char left_num = 0;	//outfield[outter];
unsigned char mid_num =	0;	//outfield[outter + 1];
unsigned char right_num = 0;//outfield[outter + 2];
unsigned char outter = 0;// no bigger than 22

enum SM3_states{sensor_Init,sensor_menu, sensor_wait, sensor_left, sensor_mid, sensor_right, sensor_decrement}state_sensor;
int SMSensor(){
	switch(state_sensor){
		case sensor_Init:
			left_num = outfield[outter];
			mid_num = outfield[outter + 1];
			right_num = outfield[outter + 2];
			state_sensor = sensor_menu;
			
		break;
		
		case sensor_menu://if the game state is in the game state then allow the sensors to send data
			if(game_state == 0x02){
				state_sensor = sensor_wait;
			}
			else{//else sensors do nothing
				state_sensor = sensor_menu;
			}
			
		break;
		
		case sensor_wait:
		if(strikes == 3){
			//game_state = 0x03;
			state_sensor = sensor_menu;
			strikes = 0;
		}
		if(game_state == 0x03){//end state
			state_sensor = sensor_menu;
		}
		else if(sensor_1){//when the "outfield" sensor detects the ball increase the score
			if(left_num == 0){//foul
				
				state_sensor = sensor_decrement;
			}
			else{
				state_sensor = sensor_left;
			}
			outter += 3;
			if(outter > 66 ){
				outter = 0;
			}
			left_num = outfield[outter];
			mid_num = outfield[outter + 1];
			right_num = outfield[outter + 2];
			
			
			out = out | 0x02;
			SPI_MasterTransmit(0x10);//should send codes to tell the LCD whether its a score or a strike
		}
		else if(sensor_2){
			outter += 3;
			if(outter > 66 ){
				outter = 0;
			}
			left_num = outfield[outter];
			mid_num = outfield[outter + 1];
			right_num = outfield[outter + 2];			
			out = out | 0x04;
			state_sensor = sensor_decrement;
			SPI_MasterTransmit(0x13);//should send codes to tell the LCD whether its a score or a strike
		}
		else if(sensor_3){
			if(mid_num == 0){//foul
				
				state_sensor = sensor_decrement;
			}
			else{
				state_sensor = sensor_mid;
			}
			outter += 3;
			if(outter > 66 ){
				outter = 0;
			}
			
			left_num = outfield[outter];
			mid_num = outfield[outter + 1];
			right_num = outfield[outter + 2];
			//out = out | 0x02;
			
			SPI_MasterTransmit(0x11);//should send codes to tell the LCD whether its a score or a strike
		}
		else if(sensor_4){//when the "strike zone" sensor detects the ball gives a strike
			//state_sensor = sensor_decrement;
			//out = out | 0x04;
			if(right_num == 0){//foul
				
				state_sensor = sensor_decrement;
			}
			else{
				state_sensor = sensor_right;
			}
			outter += 3;
			if(outter > 66 ){
				outter = 0;
			}
			
			left_num = outfield[outter];
			mid_num = outfield[outter + 1];
			right_num = outfield[outter + 2];
			SPI_MasterTransmit(0x12);//should send codes to tell the LCD whether its a score or a strike
		}
		else{
			state_sensor = sensor_wait;
			//out = out | 0x04;
			//out = out & 0xF9;
		}
		
		break;
		
		case sensor_left:
		if(sensor_1){
			state_sensor = sensor_left;
		}
		else{
			//SPI_MasterTransmit(0x10);//send something?
			state_sensor = sensor_wait;
		}

		break;
		case sensor_mid:
		if(sensor_3){
			state_sensor = sensor_mid;
		}
		else{
			//SPI_MasterTransmit(0x11);
			//out = out & 0xF3;
			state_sensor = sensor_wait;
		}

		break;
		case sensor_right:
		if(sensor_4){
			state_sensor = sensor_right;
		}
		else{
			//SPI_MasterTransmit(0x12);
			state_sensor = sensor_wait;
		}

		break;
		case sensor_decrement:
		if(sensor_1 | sensor_2 | sensor_3 | sensor_4){
			//PORTA = 0x01;
			state_sensor = sensor_decrement;
		}
		else{
			//PORTA = 0x00;
			//SPI_MasterTransmit(0x13);
			++strikes;
			state_sensor = sensor_wait;
		}


		break;
	};

	switch(state_sensor){
		case sensor_Init:
		break;
		
		case sensor_menu:
		
		break;
		
		case sensor_wait:
		
		break;
		
		case sensor_left:
		
		break;
		case sensor_mid:
		
		break;
		case sensor_right:
		
		break;
		case sensor_decrement:
		
		break;
		
	};

	return 0;
}

//////////////////////////////////////////////////////////////////////////
unsigned char motor_time = 0;
enum SM4_states{motor_Init, motor_wait, motor_up, motor_down}state_motor;
int SMmotor(){
	switch(state_motor){
		case motor_Init:
			DDRD |= (1<<PD5);	/* Make OC1A pin as output */
			TCNT1 = 0;		/* Set timer1 count zero */
			ICR1 = 2499;		/* Set TOP count for timer1 in ICR1 register */

			/* Set Fast PWM, TOP in ICR1, Clear OC1A on compare match, clk/64 */
			TCCR1A = (1<<WGM11)|(1<<COM1A1);
			TCCR1B = (1<<WGM12)|(1<<WGM13)|(1<<CS10)|(1<<CS11);
			state_motor = motor_wait;
		break;
		case motor_wait:
			if(motor){
				state_motor = motor_up;
			}
			else{
				state_motor = motor_wait;
			}		
		break;
		
		case motor_up:
			if(motor_time < 14){
				OCR1A = 70;
				++motor_time;
			}
			else{
				motor_time = 0;
				state_motor = motor_down;
			}
			
		break;
		
		case motor_down:
			if(motor_time < 14){
				OCR1A = 175;
				++motor_time;
			}
			else{
				motor_time = 0;
				state_motor = motor_wait;
			}
		break;
		
	};

	switch(state_motor){
		case motor_Init:
		
		break;
		
		case motor_wait:
		
		break;
		
		case motor_up:
		
		break;
		
		case motor_down:
		
		break;
		
	};
	
}

//////////////////////////////////////////////////////////////////////////

int main(void)
{

	DDRA = 0x0F; PORTA = 0xF0; //PORTA  is an output
	DDRB = 0x00; PORTB = 0xFF;// PORTB input
	DDRC = 0x00; PORTC = 0xFF;
	
	static task task1 ,task2, task3;//, task4;
	task *tasks[] = { &task1 ,&task2, &task3};//, &task4};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	
	//task1 - for the blue tooth input
	task1.state = bluetooth_Init;
	task1.period = 100;
	task1.elapsedTime = task1.period;
	task1.TickFct = &SMbluetooth;

	//task2 - for the menu input
	task2.state = menu_Init;
	task2.period = 100;
	task2.elapsedTime = task2.period;
	task2.TickFct = &SMmenu;

	//task3 - for the sensor input
	task3.state = sensor_Init;
	task3.period = 100;
	task3.elapsedTime = task3.period;
	task3.TickFct = &SMSensor;
	/*
	//task4 - for the motor
	task4.state = motor_Init;
	task4.period = 100;
	task4.elapsedTime = task4.period;
	task4.TickFct = &SMmotor;*.


	/*Initializations*/
	TimerSet(100);
	TimerOn();
	SPI_MasterInit();//so we can send data to the LCD.
	initUSART(0);//using usart port 0
	game_state = 0x01;
	SPI_MasterTransmit(game_state);

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
