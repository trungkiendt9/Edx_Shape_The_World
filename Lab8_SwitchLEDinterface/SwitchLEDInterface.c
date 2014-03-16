// ***** 0. Documentation Section *****
// SwitchLEDInterface.c for Lab 8
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to toggle an LED
// while a button is pressed and turn the LED on when the
// button is released.  This lab requires external hardware
// to be wired to the LaunchPad using the prototyping board.
// January 11, 2014

// Lab 8
//      Jon Valvano and Ramesh Yerraballi
//      November 21, 2013

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"	// defined all registers map.

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void PortE_Init(void);
void Delay100ms(unsigned long time);

unsigned long button;
unsigned long time_delay = 1;

// ***** 3. Subroutines Section *****

// PE0, PB0, or PA2 connected to positive logic momentary switch using 10 k ohm pull down resistor
// PE1, PB1, or PA3 connected to positive logic LED through 470 ohm current limiting resistor
// To avoid damaging your hardware, ensure that your circuits match the schematic
// shown in Lab8_artist.sch (PCB Artist schematic file) or 
// Lab8_artist.pdf (compatible with many various readers like Adobe Acrobat).
// 1) Make PE1 an output and make PE0 an input.
// 2) The system starts with the LED on (make PE1 =1).
// 3) Wait about 100 ms
// 4) If the switch is pressed (PE0 is 1), then toggle the LED once, else turn the LED on.
// 5) Steps 3 and 4 are repeated over and over.
int main(void){ 
//**********************************************************************
// The following version tests input on PE0 and output on PE1
//**********************************************************************
  TExaS_Init(SW_PIN_PE0, LED_PIN_PE1);  // activate grader and set system clock to 80 MHz
  PortE_Init();
  EnableInterrupts();           // enable interrupts for the grader
  while(1){
		Delay100ms(time_delay);
		button = GPIO_PORTE_DATA_R&0x01;   		  // read PE0 into the switch
		if(button == 1){
			GPIO_PORTE_DATA_R ^=0x02;					// Toggle PE1
		}else{
			GPIO_PORTE_DATA_R |=0x02;					// Set PE1
		}
  }
  
}

void PortE_Init(void){
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000010;     // 1) E clock
  delay = SYSCTL_RCGC2_R;           // delay            
  GPIO_PORTE_AMSEL_R &= 0x00;        // 3) disable analog function
  GPIO_PORTE_PCTL_R &= 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTE_DIR_R &= ~0x01;          // 5.1) PE0 input, positive logic switch, no need pull up resistor
  GPIO_PORTE_DIR_R |= 0x02;          // 5.2) PE1 output  
  GPIO_PORTE_AFSEL_R &= 0x00;        // 6) no alternate function
  GPIO_PORTE_DEN_R |= 0x03;          // 7) enable digital pins PE1-PE0
	GPIO_PORTE_DATA_R |=0x02;          // Set the PE1 bit in Port E DATA so the LED is initially ON	
};

// Subroutine to delay in units of 1/100 seconds
// Inputs: Number of 1/100 seconds to delay
// Outputs: None
// simple delay function
// which delays time*100 milliseconds
// assuming 80 MHz clock
void Delay100ms(unsigned long time){
  unsigned long i;
  while(time > 0){
    i = 1333333;  // this number means 100ms
    while(i > 0){
      i = i - 1;
    }
    time = time - 1; // decrements every 100 ms
  }
}
