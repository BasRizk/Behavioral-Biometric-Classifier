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
unsigned int T1_ISR_count = 2;
void T1_ISR(void) interrupt 1 {
T1_ISR_count++;
TF1 = 0; // Reset the interrupt request
}

int i=0;
int count =0;


// Testing by use of word ".tie5Ronal"
char key[11] = {'.', 't', 'i', 'e', '5', 'R', 'o', 'n', 'a', 'l', '\0'};
char test[11] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '\0'};

// Array to save time intervals between each user press
unsigned int time_btw_ms[10];

void main (void) {
	/*Note : timer takes 277 micro seconds to overflow*/
	
	
	/*------------------------------------------------
		Setup the serial port for 2400 baud at 12MHz.
	------------------------------------------------*/
	SCON = 0x50; 		// SCON: 	mode 1, 8-bit UART, enable receiver
	TMOD |= 0x20; 	// TMOD:	timer 1, mode 1, 16-bit 
	TH1 = 0x00; 		// TH1:		Seting timer value to 0
	TR1 = 1; 				// TR1: 	timer 1 run
	TI = 1; 				// TI: 		set TI to send first char of UART
	ET1 = 1;				// Setting Timer 1 interupt
	//EA = 1;	
	// Allocating space for the data to be saved
	//int r = 9;	// Number of observed calculation
	//int c = 2;	// Number of users
	//float *features = (float *)malloc(r* sizeof(float));
	
	
	
	while (1) {
		char x = _getkey();
		if (x == test[i]) {
			i++;
			while(!INT0);
			time_btw_ms[i] = 277 * T1_ISR_count;
			printf("Letter number %d : %d\n" , i , time_btw_ms[i]);			
			}
		
		
		 else { 
			printf("Another Key Pressed\n");
			i = 0;
		}
		
		if (i == 10) {
			count++;
			printf("%d\n" , count);
		}

	}
}
