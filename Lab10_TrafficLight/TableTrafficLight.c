// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// November 7, 2013

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"
#include "SysTick.h"

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

// ***** 3. Subroutines Section *****
// Linked data structure
struct State {
  unsigned long Out_Road;
	unsigned long Out_Walk;
  unsigned long Time;  
  unsigned long Next[8];}; 
typedef const struct State STyp;
#define goWest   0
#define waitWest 1
#define goSouth   2
#define waitSouth 3
#define goWalk   4
#define dontWalk1 5
#define Walkoff1   6
#define dontWalk2 7
#define Walkoff2 8
#define OUT_ROAD GPIO_PORTB_DATA_R
#define OUT_WALK GPIO_PORTF_DATA_R
#define SENSOR GPIO_PORTE_DATA_R	
// Color    LED(s) PortF
// dark     ---    0
// red      R--    0x02
// blue     --B    0x04
// green    -G-    0x08
// yellow   RG-    0x0A
// sky blue -GB    0x0C
// white    RGB    0x0E
//*********************//
//Color    LEDs PORTB
//goSouth,                 PB5-0 = 100001= 0x21 makes it green on South and red on West
//waitSouth,            PB5-0 = 100010= 0x22 makes it yellow on South and red on West
//goWest,                 PB5-0 = 001100= 0x0C makes it red on South and green on West
//waitWest,            PB5-0 = 010100=0x14 makes it red on South and yellow on West
//goWalk							 PB5-0 = 100100= 0x24 makes it red on South and red on West

STyp FSM[9]={
	{0x0C, 0x02, 500, {0,0,1,1,1,1,1,1}}, //0
	{0x14, 0x02, 50,  {2,2,2,2,4,0,2,2}}, //1
	{0x21, 0x02, 500, {2,3,2,3,2,3,2,3}},//2
	{0x22, 0x02, 50,  {0,0,0,0,4,0,0,4}},//3
	{0x24, 0x08, 500, {4,5,5,5,4,5,5,5}},//4
	{0x24, 0x02, 50,  {6,6,6,6,6,6,6,6}},//5
	{0x24, 0x00, 50,  {7,7,7,7,7,7,7,7}},//6
	{0x24, 0x02, 50,  {8,8,8,8,8,8,8,8}},//7
	{0x24, 0x00, 50,  {0,0,2,0,4,0,2,0}},//8
};
unsigned long S;  // index to the current state 
unsigned long Input;
int main(void){ 
	volatile unsigned long delay;
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210); // activate grader and set system clock to 80 MHz 
  EnableInterrupts();
  SysTick_Init();   // Program 10.2
  SYSCTL_RCGC2_R |= 0x32;      // 1) B E F = FE00B0 = 110010 = 0x32
  delay = SYSCTL_RCGC2_R;      // 2) no need to unlock
  GPIO_PORTE_AMSEL_R &= ~0x07; // 3) disable analog function on PE2-0
  GPIO_PORTE_PCTL_R &= ~0x00000FFF; // 4) enable regular GPIO on PE2-0
  GPIO_PORTE_DIR_R &= ~0x07;   // 5) inputs on PE2-0
  GPIO_PORTE_AFSEL_R &= ~0x07; // 6) regular function on PE2-0
  GPIO_PORTE_DEN_R |= 0x07;    // 7) enable digital on PE2-0
  GPIO_PORTB_AMSEL_R &= ~0x3F; // 3) disable analog function on PB5-0
  GPIO_PORTB_PCTL_R &= ~0x00FFFFFF; // 4) enable regular GPIO on PB5-0
  GPIO_PORTB_DIR_R |= 0x3F;    // 5) outputs on PB5-0
  GPIO_PORTB_AFSEL_R &= ~0x3F; // 6) regular function on PB5-0
  GPIO_PORTB_DEN_R |= 0x3F;    // 7) enable digital on PB5-0  
	GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock PortF PF0  
  GPIO_PORTF_CR_R |= 0x1F;           // allow changes to PF4-0       
  GPIO_PORTF_AMSEL_R &= 0x00;        // 3) disable analog function
  GPIO_PORTF_PCTL_R &= 0x00000000;   // 4) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R |= 0x0E;          // 5.2) PF3, PF2, PF1 output  
  GPIO_PORTF_AFSEL_R &= 0x00;        // 6) no alternate function       
  GPIO_PORTF_DEN_R |= 0x0E;          // 7) enable digital pins PF4-PF0
  S = goWest;  
  while(1){
    OUT_ROAD = FSM[S].Out_Road;  // set lights of road
		OUT_WALK = FSM[S].Out_Walk;  // set lights of walk
    SysTick_Wait10ms(FSM[S].Time);
    Input = SENSOR&0x07;     // read sensors
    S = FSM[S].Next[Input];  
  }
}

