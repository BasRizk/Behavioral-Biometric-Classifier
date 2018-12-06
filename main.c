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

int i=0;

// Testing by use of word ".tie5Ronal"
char key[11] = {'.', 't', 'i', 'e', '5', 'R', 'o', 'n', 'a', 'l', '\0'};
char test[11] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\0'};

// Array to save time intervals between each user press
unsigned int time_between_chars_typed_userA[10];
unsigned int time_between_chars_typed_userB[10];
unsigned int time_between_chars_typed_recognize[10];


int trials_num = 5;
int states_bit = 0;		//0 means userA training phase, 1 means userB training phase, 2 means entering test phase  
	
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
	// Allocating space for the data to be saved
	//int r = 9;	// Number of observed calculation
	//int c = 2;	// Number of users
	//float *features = (float *)malloc(r* sizeof(float));
	printf("User A enters Sequance 1234567890 5 times\n");
	
	
	while (1) {
		char x = _getkey();
		
		if(states_bit == 0 && trials_num == 0){
			trials_num = 5;   //Setting number of trials for user B training 
			states_bit = 1;		//finished training user A and entering training user B
		}
		else if(states_bit == 1 && trials_num == 0)
			states_bit = 2;		//finished training user B and entering test phase
		
		
		if (x == test[i]) {
			if (i != 0 && states_bit == 0) {
				time_between_chars_typed_userA[i-1] = T1_ISR_count;
				printf("%u\n", time_between_chars_typed_userA[i-1]);
			}
			else if(i != 0 && states_bit == 1) {
				time_between_chars_typed_userB[i-1] = T1_ISR_count;
				printf("%u\n", time_between_chars_typed_userB[i-1]);	
			}
			i++;
			T1_ISR_count = 0;				
		}
		
		
		 else { 
			printf("Another Key Pressed\n");
			i = 0;
		}
		
		if (i == 10) {
			i = 0;
			if(states_bit == 0 || states_bit == 1)
				trials_num --;
			if(states_bit == 0 && trials_num == 0){
				printf("user A finished training phase\nNow user B enters 1234567890 5 times\n");
			}
			else if(states_bit == 1 && trials_num == 0){
				printf("user B finished training phase\nNow any user enters 1234567890\n");
			}
		}
	}
}
