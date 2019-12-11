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

				// 0        1      2         3       4        5         6     7      8       9   10     11      12    13       14    15      16     17      18   19     20    21      22
char outfield[] = {1,1,1,  0,1,1,  1,1,0,  1,0,1,   1,1,0,  1,1,1,    0,0,1, 1,1,1, 1,0,1, 0,1,0, 1,1,1, 0,1,1, 0,1,1, 1,1,1, 1,0,1, 1,1,1, 0,1,1, 0,1,0, 1,0,1, 0,1,0, 1,1,1, 0,0,1, 1,0,0};
				// 0 1 2  3 4 5  6 7 8  9 10 11
//char outfield[] = {1,1,1};
char points[] = {   1,       1,      2,     1,        1,     1,        3,      1,     2,    3,     1,     1,    2,       1,     1,    1,       1,   3,      1,    3,      1,     3,     3 };

unsigned char outter = 0;// no bigger than 22
unsigned char cycle = 0;

unsigned char score = 0;
unsigned char score_2 = 0;
unsigned char hi_score = 0;
unsigned char hi_score_2 = 0;

unsigned char left_num = 0;	//outfield[outter];
unsigned char mid_num =	0;	//outfield[outter + 1];
unsigned char right_num = 0;//outfield[outter + 2];


unsigned char first_base = 0;
unsigned char second_base = 0;
unsigned char third_base = 0;
unsigned char home_base = 0;

unsigned char strike = 0;
/*
++first_base;
if(first_base == 2){
	first_base = 1;
	++second_base;
}
if(second_base == 2){
	second_base = 1;
	++third_base;
}
if(thrid_base == 2){
	thrid_base = 0;
	++home_base;
}
if(home_base == 1){
	++score;
	home_base = 0;
}

*/
unsigned char pointer = 0;
enum SM1_states{screen_Init, screen_wait, screen_increase, screen_decrease, screen_point};
int SMLCD(int state){
	switch(state){
		case screen_Init:
				left_num = outfield[outter];
				mid_num = outfield[outter + 1];
				right_num = outfield[outter + 2];
			state = screen_wait;
			return state;
		break;
		
		case screen_wait:
			if(receivedData == 0x01){//menu state
				receivedData = 0x00;
				LCD_ClearScreen();
				LCD_DisplayString(1, "Oh! That's a    Baseball L2Start");
			}
			if( receivedData == 0x02){//game state
				left_num = outfield[outter];
				mid_num = outfield[outter + 1];
				right_num = outfield[outter + 2];
				receivedData = 0x00;
				LCD_ClearScreen();
				LCD_DisplayString(1, "Score: 0        Strikes: 0");
			}
			else if( receivedData == 0x03){//end game
				LCD_ClearScreen();
				receivedData = 0x00;
				LCD_DisplayString(1, "Your Score:     High Score: ");
				LCD_Cursor(13);
				LCD_WriteData(score + '0');//only single digit
				//LCD_DisplayString(1, "   High Score: ");
				LCD_Cursor(29);
				LCD_WriteData(hi_score + '0');//only single digit		
				score = 0;		
			}
			else if( receivedData == 0x04){//high score
				LCD_ClearScreen();
				LCD_DisplayString(1, "High Score: ");
				LCD_Cursor(13);
				hi_score = eeprom_read_byte((uint8_t*)1);
				LCD_WriteData(hi_score + '0');//only single digit
				
			}
			else if( receivedData == 0x10){//left

				receivedData = 0x00;
				if(left_num == 0){//foul
					strike += 1;
					cycle += 1;
					if(cycle > 22){
						cycle = 0;
					}
					LCD_Cursor(26);
					LCD_WriteData(strike + '0');
					state = screen_decrease;
				}
				else{/////////////////////////////////////////////////////////////////////////////////point 
					/*++first_base;
					if(first_base == 2){
						first_base = 1;
						++second_base;
					}
					if(second_base == 2){
						second_base = 1;
						++third_base;
					}
					if(third_base == 2){
						third_base = 1;
						++home_base;
					}
					if(home_base == 1){
						++score;
						home_base = 0;
					}*/
					
					//score += 1;
					//LCD_Cursor(8);
					//LCD_WriteData(score + '0');
					//state = screen_increase;
					state = screen_point;
				}
				
				outter += 3;
				if(outter > 66 ){
					outter = 0;
				}
				
				left_num = outfield[outter];
				mid_num = outfield[outter + 1];
				right_num = outfield[outter + 2];
				
				if(strike == 3){
					if(hi_score < score){
						eeprom_update_byte((uint8_t*) 1, (uint8_t) score);
					}
					state = screen_wait;
					receivedData = 0x03;
				}
			}
			else if( receivedData == 0x11){//mid
				receivedData = 0x00;
				

				if(mid_num == 0){
					strike += 1;
					LCD_Cursor(26);
					LCD_WriteData(strike + '0');
					cycle += 1;
					if(cycle > 22){
						cycle = 0;
					}
					state = screen_decrease;
				}
				else{
					/*++first_base;
					if(first_base == 2){
						first_base = 1;
						++second_base;
					}
					if(second_base == 2){
						second_base = 1;
						++third_base;
					}
					if(third_base == 2){
						third_base = 1;
						++home_base;
					}
					if(home_base == 1){
						++score;
						home_base = 0;
					}					
					//score += 1;
					LCD_Cursor(8);
					LCD_WriteData(score + '0');*/
					state = screen_point;
				}
				
				outter += 3;
				if(outter > 66 ){
					outter = 0;
				}
				left_num = outfield[outter];
				mid_num = outfield[outter + 1];
				right_num = outfield[outter + 2];
				if(strike == 3){
					if(hi_score < score){
						eeprom_update_byte((uint8_t*) 1, (uint8_t) score);
					}
					state = screen_wait;
					receivedData = 0x03;
				}
			}
			else if( receivedData == 0x12){//right
				receivedData = 0x00;
				

				if(right_num == 0){
					strike += 1;
					LCD_Cursor(26);
					LCD_WriteData(strike + '0');
					state = screen_decrease;
					cycle += 1;
					if(cycle > 22){
						cycle = 0;
					}
				}
				else{
					/*++first_base;
					if(first_base == 2){
						first_base = 1;
						++second_base;
					}
					if(second_base == 2){
						second_base = 1;
						++third_base;
					}
					if(third_base == 2){
						third_base = 1;
						++home_base;
					}
					if(home_base == 1){
						++score;
						home_base = 0;
					}
					//score += 1;
					LCD_Cursor(8);
					LCD_WriteData(score + '0');*/
					state = screen_point;
				}
				outter += 3;
				if(outter > 66 ){
					outter = 0;
				}
				left_num = outfield[outter];
				mid_num = outfield[outter + 1];
				right_num = outfield[outter + 2];
				if(strike == 3){
					if(hi_score < score){
						eeprom_update_byte((uint8_t*) 1, (uint8_t) score);
					}
					state = screen_wait;
					receivedData = 0x03;
				}
				
			}
			else if( receivedData == 0x13){//strike
				receivedData = 0x00;

					strike += 1;
					LCD_Cursor(26);
					LCD_WriteData(strike + '0');	
					outter += 3;
					if(outter > 66 ){
						outter = 0;
					}
					left_num = outfield[outter];
					mid_num = outfield[outter + 1];
					right_num = outfield[outter + 2];
					if(strike == 3){
						if(hi_score < score){
							eeprom_update_byte((uint8_t*) 1, (uint8_t) score);
						}
						state = screen_wait;
						receivedData = 0x03;
					}
					else{
						state = screen_decrease;
					}					
										

				
				
			}
			else{
				receivedData = 0x00;
				//PORTC = PORT_C[1];//rows
				//PORTB = ~PORT_B[8];//column

				//LCD_Cursor(14);
				//LCD_WriteData(score +'0');
				left_num = outfield[outter];
				mid_num = outfield[outter + 1];
				right_num = outfield[outter + 2];
				state = screen_wait;
				//score = 0;
				//eeprom_update_byte((uint8_t*) 1, (uint8_t) score);//manual reset for score will add more things later
			}
			
			return state;		
		break;
		
		case screen_increase:
			if(receivedData == 0x10 | receivedData == 0x11 | receivedData == 0x12){
				receivedData = 0x00;				
				state = screen_increase;
			}
			else{
				state = screen_wait;
			}
			return state;
		break;
		
		case screen_decrease:
			if(receivedData == 0x13){
				receivedData = 0x00;
				state = screen_decrease;
			}
			else{
				state = screen_wait;
			}
			
			return state;
		break;
		
		case screen_point:
		if(points[cycle] == 1){
				//only for one baser
				//second base = 1 //first base = 0
				//second = 2 first = 1 // second = 0 // third = 1
				
				//now second first = 1 and third = 1 and second = 0
				//first = 2 then third = 2 = > second = 1 and third =0

				//check the bases first
				if(third_base == 1){
					third_base = 0;
					++score;
				}
				if(second_base == 1){
					second_base = 0;
					third_base = 1;
				}
				if(first_base == 1){
					first_base = 0;
					second_base = 1;
				}
				
				if(first_base == 0){
					first_base = 1;
				}

			
				LCD_Cursor(8);
				LCD_WriteData(score + '0');
				home_base = 0;
		}
		else if(points[cycle] == 2){//double
				//empty board senario
				//first = 2 so only second should be up
				
				//second = 1; 
				//first = 2 then = 0; then second = 1
				//first = 1 at this moment				
				//check who is on the board first
				if(third_base == 1){
					third_base =0;
					++score;
				}
				if(second_base == 1){//someone already on the second base then add a score
					second_base = 0;
					++score;
				}

				if(first_base == 1){//someone already on the first base
					first_base = 0;
					third_base = 1;
				}

				
				//now add someone on second base
				
				if(second_base == 0){
					second_base = 1;
				}
				LCD_Cursor(8);
				LCD_WriteData(score + '0');
				home_base = 0;
		}
		else if(points[cycle] == 3){//double
			
			
			if(third_base == 1){
				++score;
				third_base = 0;
			}
			if(second_base == 1){
				++score;
				second_base = 0;
			}
			if(first_base == 1){
				first_base = 0;
				++score;
			}
			//now add someone on second base
			
			if(third_base == 0){
				third_base = 1;
			}
				LCD_Cursor(8);
				LCD_WriteData(score + '0');
				home_base = 0;
			
		}
		++cycle;
		if(cycle > 22){
			cycle = 0;
		}
		state = screen_increase;
		
		
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
		case screen_point:
		
		break;
	};

	return 0;
}



//////////////////////////////////////////////////////////////////////////
/*
PORTC = ~0x92; PORTA = 0x80;

PORTC = ~0x10; PORTA = 0x40;

PORTC = ~0x28; PORTA = 0x20;

PORTC = ~0x44; PORTA = 0x10;

PORTC = ~0x28; PORTA = 0x08;

PORTC = ~0x10; PORTA = 0x04;
*/
unsigned char counter = 0;
unsigned char field = 0x00;
unsigned char second_first = 0x00;
enum SM2_states{matrix_Init, matrix_wait, matrix_game}state_matrix;
int SMmatrix(){
	switch(state_matrix){
		case matrix_Init:
			state_matrix = matrix_wait;
		break;
		case matrix_wait:
			if(receivedData == 0x02){//game state start;
				state_matrix = matrix_game;
			}
			else{
				PORTC = ~0x00;
				PORTA = 0x00;	
				first_base =0;
				second_base = 0;
				third_base = 0;
				state_matrix = matrix_wait;	
			}
				
		break;
		case matrix_game://lets try to do it all in one state as much as possible
			if(strike == 3){
				state_matrix = matrix_wait;
				strike = 0;
			}
			else if(counter == 0){//outfield
				if(left_num == 1){
					field = field | 0x80;
				}
				else{
					field = field & 0x7F;
				}
				if(mid_num == 1){
					field = field | 0x10;
				}
				else{
					field = field & 0xEF;
				}
				if(right_num == 1){
					field = field | 0x02;
				}
				else{
					field = field & 0xFD;
				}
				++counter;
				PORTC = ~field; 
				PORTA = 0x80;
				state_matrix = matrix_game;
			}
			else if(counter == 1){//2nd base
				if(second_base >0){
					PORTC = ~0x10;
				}
				else{
					PORTC = ~ 0x00;
				}
				//PORTC = ~0x10; 
				PORTA = 0x40;
				++counter;
				state_matrix = matrix_game;
			}
			else if(counter == 2){//--
				PORTC = ~0x00; PORTA = 0x20;
				++counter;
				state_matrix = matrix_game;
			}
			else if(counter == 3){//3rd and 1st base
				if(first_base > 0){
					second_first =  second_first | 0x04;
				}
				else{
					second_first = second_first & 0xFB;
				}
				if(third_base >0){
					second_first = second_first | 0x40;
				}
				else{
					second_first = second_first & 0xBF;
				}
				if(points[cycle] >= 3){
					second_first = second_first | 0x01;
					
				}
				else{
					second_first = second_first & 0xFE;
				}
				PORTC = ~second_first;//~0x44;
				PORTA = 0x10;
				++counter;
				state_matrix = matrix_game;
			}
			else if(counter == 4){//--
				if(points[cycle] >= 2){
					PORTC = ~0x01;
				}
				else{
					PORTC = ~0x00; 	
				}
				PORTA = 0x08;
				++counter;
				state_matrix = matrix_game;
			}
			else if(counter == 5){//home base
				if(points[cycle] >= 1){
					PORTC = ~0x11;
				}
				else{
					PORTC = ~0x10; 	
				}
				PORTA = 0x04;
				counter = 0;
				state_matrix = matrix_game;
			}
			
		break;
	}

	switch(state_matrix){
		case matrix_Init:
		break;
		case matrix_wait:
		break;
		case matrix_game:
		break;
	}
	
	return 0;
}


//////////////////////////////////////////////////////////////////////////

int main(void)
{
	
	DDRA = 0xFF; PORTA = 0x00;//slave //will as a output rn
	//DDRB = DDRB & 0x0F; PORTB = PORTB & 0xF0;//Input for the Sensor //at the moment we are using a button
	//DDRC = 0xF0; PORTC = 0x0F;//keypad
	DDRD = 0xFF; PORTD = 0x00;//lcd data bus
	
	DDRB = 0xFF; PORTB = 0x00;//PORTB as output for the matrix: Coloum
	DDRC = 0xFF; PORTC = 0x00;//PORTC as output rows
	if(eeprom_read_byte((uint8_t*)1) == 255) {
		eeprom_update_byte((uint8_t*)1, (uint8_t) 0);
	}
	if(eeprom_read_byte((uint8_t*)4) == 255) {
		eeprom_update_byte((uint8_t*)4, (uint8_t) 0);
	}
		
	hi_score = eeprom_read_byte((uint8_t*)1);
	hi_score_2 = eeprom_read_byte((uint8_t*)4);
	static task task1, task2;
	task *tasks[] = { &task1, &task2};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);
	
	
	//task1 - for the LCD input
	task1.state = screen_Init;
	task1.period = 50;
	task1.elapsedTime = task1.period;
	task1.TickFct = &SMLCD;

	task2.state = matrix_Init;
	task2.period = 1;
	task2.elapsedTime = task2.period;
	task2.TickFct = &SMmatrix;

/*Initializations*/
	TimerSet(1);
	TimerOn();
	SPI_SlaveInit();
	LCD_init();
	LCD_DisplayString(1, "Oh! That's a    Baseball L2Start");
	//LCD_DisplayString(1, "Score: 0        Strikes: 0");
	//LCD_WriteData(score +'0');
	
	
	unsigned short i;
	while(1){
		for (i = 0; i < numTasks; i++){
			if (tasks[i]->elapsedTime == tasks[i]->period){
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1; //value is gcd
		}
		while(!TimerFlag);
		TimerFlag = 0;
		
		
	}
	
	// Error: Program should not exit!
	return 0;
}