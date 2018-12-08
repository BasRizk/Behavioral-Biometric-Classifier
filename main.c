#include <REG52.H>
#include <stdio.h>

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
	TF1 = 0; // Reset the interrupt request
}

unsigned char key_i=0;

// Testing by use of word ".tie5Ronal"
//char xdata key[11] = {'.', 't', 'i', 'e', '5', 'R', 'o', 'n', 'a', 'l', '\0'};
signed char test[11] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\0'};


// Arrays to save time intervals between each user press

unsigned char xdata msg1 [] = "User A training end\nNow User B, Enter '1234567890' 5 times\n";
unsigned char xdata msg2 [] = "User B training end\nNow any user enters 1234567890\n";

unsigned char trials_num = 5;	// # of entries needed to be entered by current user


// state_bit
// 0 :userA training phase
// 1 :means userB training phase
// 2 :means entering test phase  
unsigned char states_bit = 0;
	


void main (void) {
	unsigned int time_between_chars_typed_userA[10];
	unsigned int time_between_chars_typed_userB[10];
	unsigned int time_between_chars_typed_recognize[10];
	
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
	printf("User A,Enter '1234567890' 5 times\n");
	//printf("#%u Entry\n", trials_num);

	while (1) {
		

		signed char x = _getkey();
		
		if(states_bit == 0 && trials_num == 0){
			// Just got last entry by User A @(Training Session)
			trials_num = 5;   // Reset # of needed entries for (Training Session) by User B
			states_bit = 1;		// Set to User B (Training Session) 
		
		} else if(states_bit == 1 && trials_num == 0 ) {
			// Just got last entry by User B @(Training Session)
			states_bit = 2; // Set to (Test Session)
			
		}
		
		
		if (x == test[key_i]) {
			// a Key was pressed Logic
			if (key_i != 0 && states_bit == 0) {
				time_between_chars_typed_userA[key_i-1] += (T1_ISR_count / 5);
				//printf("%u\n", time_between_chars_typed_userA[key_i-1]);
			
			} else if(key_i != 0 && states_bit == 1) {
				time_between_chars_typed_userB[key_i-1] += (T1_ISR_count / 5);
				//printf("%u\n", time_between_chars_typed_userB[key_i-1]);	
				
			} else if( key_i != 0 && states_bit == 2) {
				time_between_chars_typed_recognize[key_i-1] = T1_ISR_count;
				//printf("%u\n", time_between_chars_typed_recognize[key_i-1]);	
				
			}				
			key_i++;
			T1_ISR_count = 0;				
	
		} else { 
			printf("Repeat\n");
			key_i = 0;
			// TODO does the counter reset in this case?
		}
		
		
		if (key_i == 10) {
			// Sequence entered correctly Logic
			key_i = 0;
			
			if(states_bit == 0 || states_bit == 1)
				// It is a (Training Session)
				trials_num --;
			
			if(states_bit == 0 && trials_num == 0){
				printf("%s\n" , msg1);
				
			} else if(states_bit == 1 && trials_num == 0){
				printf("%s\n", msg2);
		
			} else if(states_bit == 2) {
				// Detection Logic
								
				unsigned char feature_i = 0;
				
				signed int error_diff = 0;
				
				unsigned long int sum_sq_err_A = 0;
				unsigned long int sum_sq_err_B = 0;
				
				
				for(; feature_i < 10; feature_i++) {
					error_diff = (time_between_chars_typed_recognize[feature_i] - time_between_chars_typed_userA[feature_i]);
					sum_sq_err_A += (error_diff*error_diff);
					
					error_diff = (time_between_chars_typed_recognize[feature_i] - time_between_chars_typed_userB[feature_i]);
					sum_sq_err_B += (error_diff*error_diff);
				}
				
				if(sum_sq_err_A > sum_sq_err_B) {
					printf("User B\n");
				} else {
					printf("User A\n");
				} 
				//printf("Oh! no I can not take a guess!");
				
			}
			
		}
	}
}
