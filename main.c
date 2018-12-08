#include <REG52.H>
#include <stdio.h>
sbit P1_1 = 0x91;  // Port 1.1 SFR bit
unsigned char flash_counter = 4;
unsigned int flash_timer = 0;

/*------------------------------------------------
The following string is the stuff we're gonna
send into the serial port.
------------------------------------------------*/

/*---------------------------------------------------------
Timer 1 Overflow Interrupt
---------------------------------------------------------*/
unsigned int T1_ISR_count = 0;
void T1_ISR(void) interrupt 3 {	
	T1_ISR_count++;
	if((flash_timer >= 5000) && (flash_counter > 0)){
			P1_1 = ~P1_1;
			flash_counter--;
			flash_timer = 0;
	}		
	flash_timer++;
	TF1 = 0; // Reset the interrupt request
}

unsigned char key_i=0;

// Testing by use of word ".tie5Ronal"
//char xdata key[11] = {'.', 't', 'i', 'e', '5', 'R', 'o', 'n', 'a', 'l', '\0'};
signed char test[11] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\0'};


// Arrays to save time intervals between each user press

//unsigned char xdata msg1 [] = "User A training end\nNow User B, Enter '1234567890' 5 times\n";
//unsigned char xdata msg2 [] = "User B training end\nNow any user enters 1234567890\n";

unsigned char xdata msg [3][20] = {"Train A, Trials:", "Train B, Trials:", "Test Mode"};
unsigned char trials [3] = {0,0,0};	// # of entries needed to be entered by current user


// state_bit
// 0 :Training userA 
// 1 :Training userB
// 2 :Test Phase 
unsigned char states_bit = 0;
unsigned char states_interupt = 0;
	


void main (void) {
	unsigned int time_between_chars_typed_userA[10];
	unsigned int time_between_chars_typed_userB[10];
	unsigned int time_between_chars_typed_recognize[10];
	

	//P1_1 = ~P1_1;
	//flash_counter = 4;

	
	/*Note : timer takes 135 micro seconds to overflow*/
	
	
	/*------------------------------------------------
		Setup the timer 1 to start in mode 2 with auto reload value of 0.
	------------------------------------------------*/
		 				// Setting the global interrupt
	SCON = 0x50; 		// SCON: 	mode 1, 8-bit UART, enable receiver
	TMOD |= 0x20; 	// TMOD:	timer 1, mode 2, 16-bit 
	TH1 = 0x00; 		// TH1:		Seting timer auto reload value to 0
	TR1 = 1; 				// TR1: 	timer 1 run
	TI = 1; 				// TI: 		set TI to send first char of UART
	IT1 = 0;
	//PCON |= 0x80;
	
	/*--------------------------------------
		Enable interrupts for timer 1.
	--------------------------------------*/
	ET1 = 1;				// Setting Timer 1 interrupt
	EA = 1;	


	// Program Logic follows
	//printf("User A,Enter '1234567890' 5 times\n");
	//printf("#%u Entry\n", trials_num);
	

	while (1) {
		
		signed char x;
//		if((trials_num == 0)){
			// Just got last entry by User A @(Training Session)
//			trials_num = 5;   // Reset # of needed entries for (Training Session) by User B
//			states_bit = (states_bit ^ 0x01);		// Set to User B (Training Session) 
//			states_interupt = states_bit;
//		}	
		
		if(states_interupt != 0xFF){
				printf("%s %u\n" , msg[states_interupt],(unsigned int) trials[states_interupt]);
				states_interupt = 0xFF;
		}
		
		x = _getkey();
			
		
		if (x == test[key_i]) {
			// a Key was pressed Logic
			if ((key_i != 0) && (states_bit == 0)) {
				time_between_chars_typed_userA[key_i-1] += (T1_ISR_count);
				//printf("%u\n", time_between_chars_typed_userA[key_i-1]);
			
			} else if((key_i != 0) && (states_bit == 1)) {
				time_between_chars_typed_userB[key_i-1] += (T1_ISR_count);
				//printf("%u\n", time_between_chars_typed_userB[key_i-1]);	
				
			} else if( (key_i != 0) && (states_bit == 2)) {
				time_between_chars_typed_recognize[key_i-1] = T1_ISR_count;
				//printf("%u\n", time_between_chars_typed_recognize[key_i-1]);	
				
			}				
			key_i++;		
	
		} else if(x == ','){ // Toggle Modes
			states_bit = (states_bit ^ 0x02) & 0xFE;
			states_interupt = states_bit;
		}	else if( (x == '.') && (~(states_bit & 0x02))){ // Toggle Users in training Mode
			states_bit = (states_bit ^ 0x01);
			states_interupt = states_bit;
		} else { 
			printf("%c\n", test[key_i]);
			//key_i = 0;
			// TODO does the counter reset in this case?
		}
		
		T1_ISR_count = 0;
				
		if (key_i == 10) {
			// Sequence entered correctly Logic
			key_i = 0;
			
			if((states_bit == 0) || (states_bit == 1)){
				
				// It is a (Training Session)
				trials[states_bit]++;
				states_interupt = states_bit;
			}
			
			

			if(states_bit == 2) {
				// Detection Logic
								
				unsigned char feature_i = 0;
				
				signed int error_diff = 0;
				
				unsigned long int sum_sq_err_A = 0;
				unsigned long int sum_sq_err_B = 0;
				
				
				for(; feature_i < 10; feature_i++) {
					error_diff = (time_between_chars_typed_recognize[feature_i] - (time_between_chars_typed_userA[feature_i] / trials[0]));
					sum_sq_err_A += (error_diff*error_diff);
					
					error_diff = (time_between_chars_typed_recognize[feature_i] - (time_between_chars_typed_userB[feature_i] / trials[1]));
					sum_sq_err_B += (error_diff*error_diff);
				}
				
				if(sum_sq_err_A > sum_sq_err_B) {
					printf("User B\n");
					flash_counter = 4;
				} else {
					printf("User A\n");
					flash_counter = 2;
				} 
				//printf("Oh! no I can not take a guess!");
				
			}
			
		}
	}
}
