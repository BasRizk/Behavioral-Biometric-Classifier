#include <REG52.H>
#include <stdio.h>
#include <string.h>
/*------------------------------------------------
The following string is the stuff we're gonna
send into the serial port.
------------------------------------------------*/
unsigned char xdata serial_input_buffer [] =
"This is a test to see if this data gets "
"injected into the serial port.\r\n"
"Have fun.\r\n"
"\r\n\r\n";

/*---------------------------------------------------------
Timer 1 Overflow Interrupt
---------------------------------------------------------*/
unsigned int T1_ISR_count = 0;
void T1_ISR(void) interrupt 3 {	
	T1_ISR_count++;
	TF1 = 0; // Reset the interrupt request
}

int key_i=0;

// Testing by use of word ".tie5Ronal"
//char key[11] = {'.', 't', 'i', 'e', '5', 'R', 'o', 'n', 'a', 'l', '\0'};
char test[11] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\0'};

// Arrays to save time intervals between each user press
unsigned int time_between_chars_typed_userA[10];
unsigned int time_between_chars_typed_userB[10];
unsigned int time_between_chars_typed_recognize[10];


unsigned char trials_num = 5;	// # of entries needed to be entered by current user

// state_bit
// 0 :userA training phase
// 1 :means userB training phase
// 2 :means entering test phase  
unsigned char states_bit = 0;

	
void main (void) {
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
	printf("User A, Please enter the following Sequance 5 times:\n '1234567890' \n");
	
	while (1) {
		
		char x = _getkey();
		printf("#%d Entry", trials_num);
		
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
				time_between_chars_typed_userA[key_i-1] = T1_ISR_count;
				printf("%u\n", time_between_chars_typed_userA[key_i-1]);
			
			} else if(key_i != 0 && states_bit == 1) {
				time_between_chars_typed_userB[key_i-1] = T1_ISR_count;
				printf("%u\n", time_between_chars_typed_userB[key_i-1]);	
				
			} else {
				time_between_chars_typed_recognize[key_i-1] = T1_ISR_count;
				printf("%u\n", time_between_chars_typed_recognize[key_i-1]);	
				
			}				
			key_i++;
			T1_ISR_count = 0;				
	
		} else { 
			printf("Another key pressed; Repeat from the beginning of the sequence.\n");
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
				printf("User A finished training phase\n");
				printf("Now User B, Please enter the following Sequance 5 times:\n '1234567890' \n");
				
			} else if(states_bit == 1 && trials_num == 0){
				printf("User B finished training phase\n");
				printf("Now any user enters 1234567890 and wait for detection\n");
		
			} else if(states_bit == 2) {
				// Detection Logic
				unsigned char feature_i = 0;
				unsigned short error_sum = 0;
				
				for(; feature_i < 10; feature_i++) {
		
					error_sum += (time_between_chars_typed_recognize[feature_i] - time_between_chars_typed_userA[feature_i]);
					error_sum -= (time_between_chars_typed_recognize[feature_i] - time_between_chars_typed_userB[feature_i]);
				}
				
				if(error_sum > 0) {
					printf("This was probably User B.");
				} else if (error_sum < 0) {
					printf("This was probably User A.");
				} else {
					printf("Oh! no I can not take a guess!");
				}
				
			}
			
		}
	}
}
