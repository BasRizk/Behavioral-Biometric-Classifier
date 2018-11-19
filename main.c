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

int i=0;
int count =0;

// Testing by use of word ".tie5Ronal"
char key[10] = {'.', 't', 'i', 'e','5','R','o','n','a','l','\0'};
void main (void) {
	
	/*------------------------------------------------
		Setup the serial port for 2400 baud at 12MHz.
	------------------------------------------------*/
	SCON = 0x50; 		// SCON: 	mode 1, 8-bit UART, enable receiver
	TMOD |= 0x20; 	// TMOD:	timer 1, mode 2, 8-bit reload
	TH1 = 0xF3; 		// TH1:		reload value for 2400 baud @ 12MHz
	TR1 = 1; 				// TR1: 	timer 1 run
	TI = 1; 				// TI: 		set TI to send first char of UART
	
	// Allocating space for the data to be saved
	int r = 9;	// Number of observed calculation
	//int c = 2;	// Number of users
	double *features = (double *)malloc(r* sizeof(double)); 
 
	
	while (1) {
		char x = _getkey();
		
		if (x == key[i]) {
			i++;
			if(i % 2 == 0) {
				
			}
		} else {
			i = 0;
		}
		
		if (i == 7) {
			count++;
			printf("%d", count);
		}

	}
}