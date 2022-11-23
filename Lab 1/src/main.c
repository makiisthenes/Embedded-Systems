#include <MKL25Z4.h>
#include "SysTick.h"
#include "gpio.h"


/* ------------------------------------------
       ECS642/ECS714 Lab1

   Demonstration of simple digital output
   Use RGB LED on Freedom board
   Introduction to cyclic systems
  -------------------------------------------- */

/* --------------------------------------
     Documentation
     =============
     WARNING: SOME PARTS OF THIS CODE ARE NOT EXPLAINED IN FULL IN WEEK 1

     The code has three principal functions
     1. main: this is where the program starts
        DO NOT CHANGE THIS FUNCTION
     2. configure: this setup the peripherals so the LEDs can be used
        DO NOT CHANGE THIS FUNCTION
     3. every10ms: this function runs every 10 ms
        *****EDIT THIS FUNCTION******

     There are also functions setRedLED, setGreenLED, setBlueLED
     Call these but do not change them

     FILE gpio.h
     - - - - - -
     This file contains some (macro) constants that are used here
     You may need to add or change constants
 -------------------------------------- */

/*----------------------------------------------------------------------------
  Turn LEDs on or off
    onOff can be ON or OFF
*----------------------------------------------------------------------------*/


void setWhiteLED(int onOff) {
		if (onOff == ON) {
    FPTB->PCOR = MASK(RED_LED_POS);
		FPTB->PCOR = MASK(GREEN_LED_POS);
		FPTD->PCOR = MASK(BLUE_LED_POS);
	
  }
  if (onOff == OFF) {
    FPTB->PSOR = MASK(RED_LED_POS);
		FPTB->PSOR = MASK(GREEN_LED_POS);
		FPTD->PSOR = MASK(BLUE_LED_POS);
		
  }
}
	
void setYellowLED(int onOff){
	if (onOff == ON) {
    FPTB->PCOR = MASK(RED_LED_POS);
		FPTB->PCOR = MASK(GREEN_LED_POS);
		FPTD->PSOR = MASK(BLUE_LED_POS);
	
  }
  if (onOff == OFF) {
    FPTB->PSOR = MASK(RED_LED_POS);
		FPTB->PSOR = MASK(GREEN_LED_POS);
		FPTD->PSOR = MASK(BLUE_LED_POS);
		
  }
}

void setRedLED(int onOff) {
  if (onOff == ON) {
		FPTB->PSOR = MASK(GREEN_LED_POS);
		FPTD->PSOR = MASK(BLUE_LED_POS);
    PTB->PCOR = MASK(RED_LED_POS) ;
  }
  if (onOff == OFF) {
		FPTB->PSOR = MASK(GREEN_LED_POS);
		FPTD->PSOR = MASK(BLUE_LED_POS);
    PTB->PSOR =  MASK(RED_LED_POS) ;
  }
  // no change otherwise
}

void setMagentaLED(int onOff){
	if (onOff == ON) {
    PTB->PCOR = MASK(RED_LED_POS) ;
		PTB->PSOR = MASK(GREEN_LED_POS);
		PTD->PCOR = MASK(BLUE_LED_POS);
  }
  if (onOff == OFF) {
    PTB->PSOR =  MASK(RED_LED_POS) ;
		PTB->PSOR = MASK(GREEN_LED_POS);
		PTD->PSOR = MASK(BLUE_LED_POS);
  }
  // no change otherwise
}


void setBlueLED(int onOff) {
  if (onOff == ON) {
    PTD->PCOR = MASK(BLUE_LED_POS) ;
		PTB->PSOR =  MASK(RED_LED_POS) ;
		PTB->PSOR = MASK(GREEN_LED_POS);
  }
  if (onOff == OFF) {
    PTD->PSOR = MASK(BLUE_LED_POS) ;
		PTB->PSOR =  MASK(RED_LED_POS) ;
		PTB->PSOR = MASK(GREEN_LED_POS);
  }
  // no change otherwise
}

void setCyanLED(int onOff){
	if (onOff == ON) {
    PTB->PCOR = MASK(GREEN_LED_POS);
		PTB->PSOR = MASK(RED_LED_POS);
		PTB->PCOR = MASK(BLUE_LED_POS);
  }
  if (onOff == OFF) {
    PTB->PSOR = MASK(GREEN_LED_POS) ;
		PTB->PSOR = MASK(BLUE_LED_POS);
		PTB->PSOR = MASK(RED_LED_POS);
  }

}


void setGreenLED(int onOff) {
  if (onOff == ON) {
    PTB->PCOR = MASK(GREEN_LED_POS) ;
		PTB->PSOR = MASK(RED_LED_POS);
		PTB->PCOR = MASK(BLUE_LED_POS);
  }
  if (onOff == OFF) {
    PTB->PSOR = MASK(GREEN_LED_POS) ;
		PTB->PSOR = MASK(RED_LED_POS);
		PTB->PCOR = MASK(BLUE_LED_POS);
  }
  // no change otherwise
}




	



/*----------------------------------------------------------------------------
  every10ms - this function runs every 10ms

This function evaluates whether the system should change state (only occassionally)

The system stays in each state for a number of cycles, counted by the 'count'
variable. Each cycle is 10ms long, so 100 cycles gives 100 x 10ms = 1 sec
*----------------------------------------------------------------------------*/
int state = REDOFF ;  // this variable holds the current state
int count = OFFPERIOD ; // this counter variable to decremented to zero

void every10ms() {
  if (count > 0) count -- ; // decrement the counter

  switch (state) {

    // there is one case for each state
    // each case has the same structure

		case WHITEOFF:
			if (count == 0) {    // now time to change state
        setWhiteLED(ON) ;    // set the LEDs for the new state
        state = WHITEON ;    // ... the new state
        count = ONPERIOD ; // reset the counter
      }
      break ;

    case WHITEON:
      if (count == 0) {
        setWhiteLED(OFF) ;     // set the LEDs for the new state
        state = YELLOWOFF ;
        count = OFFPERIOD ;
      }
      break ;
		
		case YELLOWOFF:
			if (count == 0) {    // now time to change state
        setYellowLED(ON) ;    // set the LEDs for the new state
        state = YELLOWON ;    // ... the new state
        count = ONPERIOD ; // reset the counter
      }
      break ;

    case YELLOWON:
      if (count == 0) {
        setYellowLED(OFF) ;     // set the LEDs for the new state
        state = REDOFF ;
        count = OFFPERIOD ;
      }
      break ;	
			
			
    case REDOFF:  // the state names are defined in the gpio.h file
      if (count == 0) {    // now time to change state
        setRedLED(ON) ;    // set the LEDs for the new state
        state = REDON ;    // ... the new state
        count = ONPERIOD ; // reset the counter
      }
      break ;

    case REDON:
      if (count == 0) {
        setRedLED(OFF) ;     // set the LEDs for the new state
        state = MAGENTAOFF ;
        count = OFFPERIOD ;
      }
      break ;
			
		case MAGENTAOFF:
			if (count == 0) {    // now time to change state
        setMagentaLED(ON) ;    // set the LEDs for the new state
        state = MAGENTAON ;    // ... the new state
        count = ONPERIOD ; // reset the counter
      }
      break ;

    case MAGENTAON:
      if (count == 0) {
        setMagentaLED(OFF) ;     // set the LEDs for the new state
        state = BLUEOFF ;
        count = OFFPERIOD ;
      }
      break ;
		
			
		case BLUEOFF:
			if (count == 0) {
        setBlueLED(ON) ;    // set the LEDs for the new state
        state = BLUEON ;
        count = ONPERIOD ;
      }
      break ;

    case BLUEON:
      if (count == 0) {
        setBlueLED(OFF) ;   // set the LEDs for the new state
        state = CYANOFF ;
        count = OFFPERIOD ;
      }
      break ;
			
		
	case CYANOFF:
      if (count == 0) {
        setCyanLED(ON) ;    // set the LEDs for the new state
        state = CYANON ;
        count = ONPERIOD ;
      }
      break ;

    case CYANON:
      if (count == 0) {
        setCyanLED(OFF) ;   // set the LEDs for the new state
        state = GREENOFF ;
        count = OFFPERIOD ;
      }
      break ;			
			
			

    case GREENOFF:
      if (count == 0) {
        setGreenLED(ON) ;    // set the LEDs for the new state
        state = GREENON ;
        count = ONPERIOD ;
      }
      break ;

    case GREENON:
      if (count == 0) {
        setGreenLED(OFF) ;   // set the LEDs for the new state
        state = WHITEOFF ;
        count = OFFPERIOD ;
      }
      break ;
			
		
			
			
		
		
			
  }
}





#define task1_default 3000;
#define task2_default 5000;

int task1_counter = task1_default; // 3000 mseconds - 3 seconds.
int task2_counter = task2_default; // 5000 mseconds - 5 seconds.


int task1_state = REDON;
int task2_state = GREENON;

void Task1Red(){
	
	// This function will turn on the RED LED on and off every 3 seconds.
	if (task1_counter > 0) task1_counter -- ;
	switch (task1_state){
		case (REDON):
			// Case of it being on REDON.
			if (task1_counter == 0){
				setRedLED(ON);
				task1_state = REDOFF;
				task1_counter = task1_default;
			}
			break; // break and wait for next cycle, let other tasks run. 
			
		case (REDOFF):
			if (task1_counter == 0){
				setRedLED(OFF);
				task1_state = REDON;
				task1_counter = task1_default;
			}
			break;
	}
}

void Task2Green(){
	// This function will turn on the GREEN LED on and off every 5 seconds.
	if (task2_counter > 0) task2_counter -- ;
	switch (task2_state){
		case(GREENON):
			// Case of it being on REDON.
			if (task2_counter == 0){
				setGreenLED(ON);
				task2_state = GREENOFF;
				task2_counter = task2_default;
			}
			break; // break and wait for next cycle, let other tasks run. 
			
		case (GREENOFF):
			if (task2_counter == 0){
				setGreenLED(OFF);
				task2_state = GREENON;
				task2_counter = task2_default;
			}
			break;
	}
}








// Code for lab challenge 4.3:
// Activity 3, change of LEDs in sequence. 




/*----------------------------------------------------------------------------
  Configuration
  The GPIO ports for the LEDs are configured. This is not explained in week 1
*----------------------------------------------------------------------------*/
void configure() {
  // Configuration steps
  //   1. Enable clock to GPIO ports
  //   2. Enable GPIO ports
  //   3. Set GPIO direction to output
  //   4. Ensure LEDs are off

  // Enable clock to ports B and D
  SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;

  // Make 3 pins GPIO
  PORTB->PCR[RED_LED_POS] &= ~PORT_PCR_MUX_MASK;
  PORTB->PCR[RED_LED_POS] |= PORT_PCR_MUX(1);
  PORTB->PCR[GREEN_LED_POS] &= ~PORT_PCR_MUX_MASK;
  PORTB->PCR[GREEN_LED_POS] |= PORT_PCR_MUX(1);
  PORTD->PCR[BLUE_LED_POS] &= ~PORT_PCR_MUX_MASK;
  PORTD->PCR[BLUE_LED_POS] |= PORT_PCR_MUX(1);

  // Set ports to outputs
  PTB->PDDR |= MASK(RED_LED_POS) | MASK(GREEN_LED_POS);
  PTD->PDDR |= MASK(BLUE_LED_POS);

  // Turn off LEDs
  PTB->PSOR = MASK(RED_LED_POS) | MASK(GREEN_LED_POS);
  PTD->PSOR = MASK(BLUE_LED_POS);
  // end of configuration code
}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
  configure() ;     // configure the GPIO outputs for the LED
  setRedLED(OFF) ;  // ensure all the LEDs are off
  setGreenLED(OFF) ;
  setBlueLED(OFF) ;
  Init_SysTick(1000) ; // initialse SysTick every 1ms
  waitSysTickCounter(10) ;
  while (1) {      // this runs for ever
    
		Task1Red();  
		// This will try run, cycle once decrementing the counter by 1, before going to the next function.
		
		Task2Green();
		// Runs if counter goes to zero, in both cases, both tasks will run within the 10 milliseconds. 
		
		
    waitSysTickCounter(10) ;  // cycle every 10 ms - not explained in week 1
  }
}

// Testing git push file of the lab 1.
