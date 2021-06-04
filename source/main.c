/* Author: Jeffrey Wang jwang619@ucr.edu
 * Lab Section: 22
 * Assignment: Lab # 11  Exercise # 1
 * Exercise Description: [optional - include for your own benefit]
 *
 * I acknowledge all content contained herein, excluding template or example
 * code is my own original work.
 *
 *  Demo Link:
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn() {
	TCCR1B = 0x0B;

	OCR1A = 125;

	TIMSK1 = 0x02;

	TCNT1 = 0;

	_avr_timer_cntcurr = _avr_timer_M;

	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00;
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}



typedef struct _task {
	signed char state;
	unsigned long int period;
	unsigned long int elapsedTime;
	int (*TickFct) (int);
} task;

unsigned char patterns[8] = {0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
unsigned char rows[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; 
unsigned char pass3 = 0xF1;
unsigned char pass1 = 0xFB;
unsigned char start3 = 0xF8;
unsigned char start1 = 0xFE;
unsigned int currLevel = 0;

enum checkStates {checkStart, Wait, Pressed, PressBuffer};

int SMCheck(int state) {
	unsigned char inputA = 0x00;
	inputA = ~PINA;
	switch(state) {
		case checkStart:
                        state = Wait;
                        break;
                case Wait:
			if((inputA & 0x01) == 0x01) {
                       		state = Pressed;
			}
			break;
                case Pressed:
                        state = PressBuffer;
                        break;
                case PressBuffer:
			if((inputA & 0x01) != 0x01) {
                                state = Wait;
                        }
                        break;
                default:
                        state = checkStart;
                        break;
	}
	switch(state) {
                case checkStart:
                        
                        break;
                case Wait:
                        
                        break;
                case Pressed:
                        if (currLevel >= 0 && currLevel <= 3) {
				if (rows[currLevel] == pass3) {
					++currLevel;
					rows[currLevel] = start3;
				}
			}
			else if (currLevel >= 4 && currLevel <= 6) {
                                if (rows[currLevel] == pass1) {
                                        ++currLevel;
                                        rows[currLevel] = start1;
                                }
                        }
			else if (currLevel >= 7) {
				if (rows[currLevel] == pass1) {
                                        currLevel = 0;
					for(unsigned int iReset = 0; iReset < 8; ++iReset) {
						rows[iReset] = 0xFF;
					}
                                        rows[currLevel] = start3;
                                }
			}

                        break;
                case PressBuffer:
                        
                        break;
                default:
                        
                        break;
        }
	return state;
}

enum PlayerStates {SMStart, ThreeState1, ThreeState2, ThreeState3, ThreeBack2, OneState1, OneState2, OneState3, OneState4, OneState5, OneBack1, OneBack2, OneBack3};

int PlayerLights(int state) {
	//static unsigned char pattern = 0x40;
	//static unsigned char row = 0xF8;

	switch(state) {
		case SMStart:
			if(currLevel > 3) {
				state = OneState1;
				break;
			}
			state = ThreeState1;
			break;
		case ThreeState1:
			if(currLevel > 3) {
                                state = OneState1;
                                break;
                        }
       	        	state = ThreeState2;
       	       		break;
		case ThreeState2:
			if(currLevel > 3) {
                                state = OneState1;
                                break;
                        }
        	        state = ThreeState3;
       		        break;
		case ThreeState3:
			if(currLevel > 3) {
                                state = OneState1;
                                break;
                        }
	                state = ThreeBack2;
               		break;
		case ThreeBack2:
			if(currLevel > 3) {
                                state = OneState1;
                                break;
                        }
			state = ThreeState1;
			break;

		case OneState1:
			if(currLevel < 4) {
                                state = ThreeState1;
                                break;
                        }
                        state = OneState2;
                        break;
                case OneState2:
			if(currLevel < 4) {
                                state = ThreeState1;
                                break;
                        }
                        state = OneState3;
                        break;
                case OneState3:
			if(currLevel < 4) {
                                state = ThreeState1;
                                break;
                        }
                        state = OneState4;
                        break;
                case OneState4:
			if(currLevel < 4) {
                                state = ThreeState1;
                                break;
                        }
                        state = OneState5;
                        break;
                case OneState5:
			if(currLevel < 4) {
                                state = ThreeState1;
                                break;
                        }
                        state = OneBack1;
                        break;
		case OneBack1:
			if(currLevel < 4) {
                                state = ThreeState1;
                                break;
                        }
                        state = OneBack2;
                        break;
                case OneBack2:
			if(currLevel < 4) {
                                state = ThreeState1;
                                break;
                        }
                        state = OneBack3;
                        break;
                case OneBack3:
			if(currLevel < 4) {
                                state = ThreeState1;
                                break;
                        }
                        state = OneState1;
                        break;

		default:
			state = SMStart;
			break;
	}
	switch(state) {
                case SMStart:
                        break;
                case ThreeState1:
			rows[currLevel] = 0xF8;
                        break;
                case ThreeState2:
                        rows[currLevel] = (rows[currLevel] << 1) | 0x01;
                        break;
                case ThreeState3:
                        rows[currLevel] = (rows[currLevel] << 1) | 0x01;
                        break;
		case ThreeBack2:
			rows[currLevel] = (rows[currLevel] >> 1) | 0x10;
			break;

		case OneState1:
			rows[currLevel] = 0xFE;
                        break;
                case OneState2:
			rows[currLevel] = (rows[currLevel] << 1) | 0x01;
                        break;
                case OneState3:
                        rows[currLevel] = (rows[currLevel] << 1) | 0x01;
                        break;
                case OneState4:
                        rows[currLevel] = (rows[currLevel] << 1) | 0x01;
                        break;
                case OneState5:
                        rows[currLevel] = (rows[currLevel] << 1) | 0x01;
                        break;
		case OneBack1:
			rows[currLevel] = (rows[currLevel] >> 1) | 0x80;
                        break;
                case OneBack2:
			rows[currLevel] = (rows[currLevel] >> 1) | 0x80;
                        break;
                case OneBack3:
                        rows[currLevel] = (rows[currLevel] >> 1) | 0x80;
                        break;


                default:
                        state = SMStart;
                        break;
        }
	//PORTC = pattern;	// Pattern to display
	//PORTD = rows[1];		// Row(s) displaying pattern
	return state;

}

enum DisplayStates {DisplayStart, Display1, Display2, Display3, Display4, Display5, Display6, Display7, Display8};

int DisplayLights(int state) {
        //static unsigned char pattern = 0x40;
        //static unsigned char row = 0xF8;

        switch(state) {
                case DisplayStart:
                        state = Display1;
                        break;
                case Display1:
                        state = Display2;
                        break;
                case Display2:
                        state = Display3;
                        break;
                case Display3:
                        state = Display4;
                        break;
                case Display4:
                        state = Display5;
                        break;
		case Display5:
                        state = Display6;
                        break;
		case Display6:
                        state = Display7;
                        break;
                case Display7:
                        state = Display8;
                        break;
                case Display8:
                        state = Display1;
                        break;

                default:
                        state = DisplayStart;
                        break;
        }
        switch(state) {
		case DisplayStart:
                        break;
                case Display1:
                        PORTC = patterns[0];
			PORTD = rows[0];
                        break;
                case Display2:
                        PORTC = patterns[1];
                        PORTD = rows[1];
                        break;
                case Display3:
                        PORTC = patterns[2];
                        PORTD = rows[2];
                        break;
                case Display4:
                        PORTC = patterns[3];
                        PORTD = rows[3];
                        break;
                case Display5:
                        PORTC = patterns[4];
                        PORTD = rows[4];
                        break;
		case Display6:
                        PORTC = patterns[5];
                        PORTD = rows[5];
                        break;
                case Display7:
                        PORTC = patterns[6];
                        PORTD = rows[6];
                        break;
                case Display8:
                        PORTC = patterns[7];
                        PORTD = rows[7];
                        break;
                default:
                        break;
        }
	return state;
}

unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
	unsigned long int c;
	while(1){
		c = a%b;
		if(c==0){return b;}
		a = b;
		b = c;
	}
	return 0;
}


int main(void) {
    /* Insert DDR and PORT initializations */
	unsigned char x;
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRD = 0xFF; PORTD = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	
	static task task1, task2, task3;
	task *tasks[] = {&task1, &task2, &task3};
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	const char start = -1;

	task1.state = start;
	task1.period = 400;
	task1.elapsedTime = task1.period;
	task1.TickFct = &PlayerLights;

	task2.state = start;
        task2.period = 1;
        task2.elapsedTime = task2.period;
        task2.TickFct = &DisplayLights;

	task3.state = start;
        task3.period = 1;
        task3.elapsedTime = task3.period;
        task3.TickFct = &SMCheck;

	unsigned short i;

	unsigned long GCD = tasks[0]->period;
	for(i = 1; i < numTasks; i++) {
		GCD = findGCD(GCD, tasks[i]->period);
	}

	TimerSet(GCD);
	TimerOn();

    /* Insert your solution below */
    while (1) {
    	for(i = 0; i < numTasks; i++) {
		if (tasks[i]->elapsedTime == tasks[i]->period ) {
			tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
			tasks[i]->elapsedTime = 0;
		}
		tasks[i]->elapsedTime += GCD;
	}
	while(!TimerFlag);
	TimerFlag = 0;
    }
    return 0;
}
