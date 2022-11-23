// This is my own attempt at making the LED flash based on my understanding from the book.
#include "SysTick.h" // This contains the sleep function. (Lab1\lab1-makiisthenes\src\SysTick.h)
#include <MKL25Z4.h>  // Data structue of Microcontroller given to us. 



// First thing we need to do, is to provide clock signals to specific ports that we will like to use in this project. 

// By looking at the schematic, the leds on board are on pins, PTB18, PTB19 and PTD1 meaning ports B and D need to be opened.

#define RED_LED1 (18)  // PORT B
#define GREEN_LED2 (19)  // PORT B
#define BLUE_LED3 (1)  // PORT D

#define MASK(X) (1UL << (x))


void setting_up(){
	
		// Now we enable the clocks for ports B and D based on SIM.
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;

	// Now that we enabled them via the SIM module, we can now go on to configure the specific pins to GPIO pins.
	// We go to that port and look at the pin control register specifically the pin we are looking for.
	PORTB -> PCR[RED_LED1] &= ~PORT_PCR_MUX_MASK;  // CLEAR THE MUX FIELD ON THE PCR PORTB
	PORTB -> PCR[RED_LED1] |= PORT_PCR_MUX(1);

	PORTB -> PCR[GREEN_LED2] &= ~PORT_PCR_MUX_MASK;  
	PORTB -> PCR[GREEN_LED2] |= PORT_PCR_MUX(1);

	PORTD -> PCR[BLUE_LED3] &= ~PORT_PCR_MUX_MASK;  
	PORTD -> PCR[BLUE_LED3] |= PORT_PCR_MUX(1);

	// Now we need to define each as outputs by using PCR direction register for each pin.
	FPTB->PDDR |= MASK(RED_LED1) | MASK(GREEN_LED2);
	FPTD->PDDR |= MASK(BLUE_LED3);
}

void Activity3(){
	// Clearing because cathode connected to pin and so need ground to light up led.
	
	
	// Make white light first.
	FPTB->PCOR = MASK(RED_LED1);
	FPTB->PCOR = MASK(GREEN_LED2);
	FPTD->PCOR = MASK(BLUE_LED3);
	
	waitSysTickCounter(50);  // half a second.
	
	// Make yellow light.
	FPTD->PSOR = MASK(BLUE_LED3);
	
	waitSysTickCounter(50);  // half a second.
	
	// Make red light.
	FPTB->PSOR = MASK(GREEN_LED2);
	
	waitSysTickCounter(50);  // half a second.
	
	// Make magenta light.
	FPTD->PCOR = MASK(BLUE_LED3);
	
	waitSysTickCounter(50);  // half a second.
	
	// Make blue light.
	FPTB->PSOR = MASK(RED_LED1);
	
	waitSysTickCounter(50);  // half a second.
	
	// Make cyan light.
	FPTB->PSOR = MASK(GREEN_LED2);
	
	waitSysTickCounter(50);  // half a second.
	
	// Make green light.
	FPTD->PSOR = MASK(BLUE_LED3);
	
	waitSysTickCounter(50);  // half a second.
	
	// Loop again.
	
}



int main(void){
	Init_SysTick(1000) ; // initialse SysTick every 1ms
  waitSysTickCounter(10) ;
  while (1) {      // this runs for ever
    Activity3() ;  // call this every 10ms
    
    waitSysTickCounter(10) ;  // cycle every 10 ms - not explained in week 1
  }
}










