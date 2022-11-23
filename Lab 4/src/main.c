/* ------------------------------------------
       ECS642/ECS714 Lab4

   The LED is displayed at different brightness levels using PWM
   The PIT is used to time the transition between the brightness levels
   A button press switches between two rates (by changing the PIT load value): 
       * a fast one cycles through all the brightness levels in 2 s
       * a slow one takes 10 s
  -------------------------------------------- */

#include <MKL25Z4.h>
#include <stdbool.h>
#include "../include/SysTick.h"
#include "../include/button.h"

#include "../include/pit.h"
#include "../include/TPMPWM.h"
#include "../include/triColorLedPWM.h"


/* --------------------------------------
     Documentation
     =============
     This is a cyclic system with a cycle time of 10ms

     The file has a main function, two tasks
       1. pollB1Task: this polls shield button B1
       2. toggleRateTask: this toggles between a fast and slow rate for changing the LED brightness
     and the PIT interrupt service routine which changes the brightness of 
     one of the LEDs
 -------------------------------------- */
 
/* --------------------------------------------
  Variables for communication
*----------------------------------------------------------------------------*/
bool pressedB1_ev ;  // set by task1 (polling) and cleared by task 2

/*----------------------------------------------------------------------------
  task1pollB1
  
  This task polls button B1. Keep this task.
*----------------------------------------------------------------------------*/
int b1State ;        // Current state - corresponds to position
int b1BounceCount ;

void initPollB1Task() {
    b1State = BOPEN ;
    pressedB1_ev = false ; 
    b1BounceCount = 0 ;
}

void pollB1Task() {
    if (b1BounceCount > 0) b1BounceCount -- ;
    switch (b1State) {
        case BOPEN:
            if (isPressed(B1MASK)) {
                b1State = BCLOSED ;
                pressedB1_ev = true ; 
            }
          break ;

        case BCLOSED:
            if (!isPressed(B1MASK)) {
                b1State = BBOUNCE ;
                b1BounceCount = BOUNCEDELAY ;
            }
            break ;

        case BBOUNCE:
            if (isPressed(B1MASK)) {
                b1State = BCLOSED ;
            }
            else if (b1BounceCount == 0) {
                b1State = BOPEN ;
            }
            break ;
    }                
}

/* Activity 4 B2 polling initiation. */ 
bool pressedB2_ev ;
bool pressedToggleB2;
int b2State ;        // Current state - corresponds to position
int b2BounceCount ;

void initPollB2Task() {
    b2State = BOPEN ;
    pressedB2_ev = false ; 
    b2BounceCount = 0 ;
}

void pollB2Task() {
    if (b2BounceCount > 0) b2BounceCount -- ;
    switch (b2State) {
        case BOPEN:
            if (isPressed(B2MASK)) {
                b2State = BCLOSED ;
                pressedB2_ev = true ; 
							  pressedToggleB2 = true;
            }
          break ;

        case BCLOSED:
            if (!isPressed(B2MASK)) {
                b2State = BBOUNCE ;
                b2BounceCount = BOUNCEDELAY ;
            }
            break ;

        case BBOUNCE:
            if (isPressed(B2MASK)) {
                b2State = BCLOSED ;
            }
            else if (b2BounceCount == 0) {
                b2State = BOPEN ;
            }
            break ;
    }                
}



/* ---------------------------- */
/* Creating cyclic system, */

unsigned int blue_bright = 31 ;
unsigned int red_bright = 0 ; 
unsigned int green_bright = 0;

void activity2(){   // PATTERN A
	static enum {ST_BLUE_INC, ST_RED_DEC, ST_GREEN_INC, ST_BLUE_DEC, ST_RED_INC, ST_GREEN_DEC} state = ST_BLUE_INC ;  // Initialising of states.
	
	switch (state){
		case ST_BLUE_INC:
			setLEDBrightness(Blue, blue_bright) ;
			if (blue_bright == MAXBRIGHTNESS){
				// Check if condition is met.
				state = ST_RED_DEC ;
			}else{
				blue_bright = (blue_bright + 1) % (MAXBRIGHTNESS + 1); 
			}	
			break;
		
			
		case ST_RED_DEC:
			setLEDBrightness(Red, red_bright) ; 
			if (red_bright == 0){
				// Check if condition is met.
				state = ST_GREEN_INC ;
			}else{
				
				red_bright = (red_bright - 1) % (MAXBRIGHTNESS + 1); 
			}
			break;
			

		case ST_GREEN_INC:
			setLEDBrightness(Green, green_bright) ;
			if (green_bright == MAXBRIGHTNESS){
				// Check if condition is met.
				state = ST_BLUE_DEC ;
			}else{
				green_bright = (green_bright + 1) % (MAXBRIGHTNESS + 1); 
			}
		
			break;

			
		case ST_BLUE_DEC:
			setLEDBrightness(Blue, blue_bright) ;
			if (blue_bright == 0){
				// Check if condition is met.
				state = ST_RED_INC ;
			}else{
				blue_bright = (blue_bright - 1) % (MAXBRIGHTNESS + 1); 
			}
			break;

			
			
		case ST_RED_INC:
			setLEDBrightness(Red, red_bright) ;
			if (red_bright == MAXBRIGHTNESS){
				// Check if condition is met.
				state = ST_GREEN_DEC ;
			}else{
				 
				red_bright = (red_bright + 1) % (MAXBRIGHTNESS + 1); 
			}
			break;

			
		
		case ST_GREEN_DEC:
			setLEDBrightness(Green, green_bright) ;
			if (green_bright == 0){
				// Check if condition is met.
				state = ST_BLUE_INC ;
			}else{
				
				green_bright = (green_bright - 1) % (MAXBRIGHTNESS + 1); 
			}
			break;
		
		}

}


/* ---------------------------- */




unsigned int pb_blue_bright = 0 ;
unsigned int pb_red_bright = 0 ; 
unsigned int pb_green_bright = 0;
void activity4(){   // PATTERN B
	static enum {StateW, StateX, StateY, StateZ} patternb_state = StateW;
	switch (patternb_state){
		
		
		case StateW:
			if (pb_red_bright == MAXBRIGHTNESS & pb_blue_bright == MAXBRIGHTNESS){
				patternb_state = StateX;
			}else{
				setLEDBrightness(Blue, pb_blue_bright) ;
				setLEDBrightness(Red, pb_red_bright) ;
				pb_blue_bright = (pb_blue_bright + 1) % (MAXBRIGHTNESS + 1);
				pb_red_bright = (pb_red_bright + 1) % (MAXBRIGHTNESS + 1);
			}
			break ;
		
			
		case StateX:
			if (pb_blue_bright == 0 & pb_green_bright == MAXBRIGHTNESS){
				patternb_state = StateY;
			}else{
				setLEDBrightness(Blue, pb_blue_bright) ;
				setLEDBrightness(Green, pb_green_bright) ;
				pb_blue_bright = (pb_blue_bright - 1) % (MAXBRIGHTNESS + 1);
				pb_green_bright = (pb_green_bright + 1) % (MAXBRIGHTNESS + 1);
			}
			break ;
		
		
		case StateY:
			if (pb_blue_bright == MAXBRIGHTNESS & pb_red_bright == 0){
				patternb_state = StateZ;
			}else{
				setLEDBrightness(Blue, pb_blue_bright) ;
				setLEDBrightness(Red, pb_red_bright) ;
				pb_blue_bright = (pb_blue_bright + 1) % (MAXBRIGHTNESS + 1);
				pb_red_bright = (pb_red_bright - 1) % (MAXBRIGHTNESS + 1);
			}
			break ;
		
		
		case StateZ:
			if (pb_blue_bright == 0 & pb_green_bright == 0){
				patternb_state = StateW;
			}else{
				setLEDBrightness(Blue, pb_blue_bright) ;
				setLEDBrightness(Green, pb_green_bright) ;
				pb_blue_bright = (pb_blue_bright - 1) % (MAXBRIGHTNESS + 1);
				pb_green_bright = (pb_green_bright - 1) % (MAXBRIGHTNESS + 1);
			}
			break ;
	}

}












/* -------------------------------------
    Programmable Interrupt Timer (PIT) interrupt handler

      Check each channel to see if caused interrupt
      Write 1 to TIF to reset interrupt flag

    Changes the LED brightness. KEEP THIS ISR but changes are needed
   ------------------------------------- */

void PIT_IRQHandler(void) {
		static enum {PATTERNA, PATTERNB} pattern_state = PATTERNA;
    // clear pending interrupts
    NVIC_ClearPendingIRQ(PIT_IRQn);
		
	
		switch (pattern_state) {
				if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {
        case PATTERNA :  
					// First check if B2 is pressed, if this is the case then we need to change.
					
          if (pressedB2_ev) {                   
             pressedB2_ev = false ; 
						 pattern_state = PATTERNB ;
						 break ; 
          }
					
					activity2();
					PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK ;   // clear interrupt request flag for channel.
          break ;
            
        case PATTERNB :
					if (pressedB2_ev) {                  
							pressedB2_ev = false ;
							pattern_state = PATTERNA ;
							break ;
					}
					activity4();
					PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK ;  // clear interrupt request flag for channel.
					break ;
			}
		}		
}




/*----------------------------------------------------------------------------
   Task: toggleRateTask

   Toggle the rate of upadtes to the LEDs on every signal

   KEEP THIS TASK, but changes may be needed
*----------------------------------------------------------------------------*/

// PIT load values
// The larger the count, the lower the frequency of interrupts
const uint32_t pitSlowCountPatternA = PITCLOCK * 5 / 32 ; // 32 phases in 10 s  = 30 SECONDS TO COMPLETE. (5s X 6)
const uint32_t pitFastCountPatternA = PITCLOCK * 2 / 32 ; // 32 phases in 2 s  = 12 SECONDS TO COMPLETE.     (2s  X 6)    


const uint32_t pitSlowCountPatternB = PITCLOCK * 7.5 / 32 ; // all phases in  32 phases in 15 s = 30 SECONDS TO COMPLETE. (7.5s X 4)
const uint32_t pitFastCountPatternB = PITCLOCK * 3 / 32 ; // all phases in 32 phases in 3 seconds - 12s                (3s X 4)




#define SLOWPATTERNA 0
#define FASTPATTERNA 1
#define SLOWPATTERNB 2
#define FASTPATTERNB 3 

int PatternRateState ;  // this variable holds the current state

// initial state of task
void initToggleRateTask() {
    setTimer(0, pitSlowCountPatternB) ;
    PatternRateState = SLOWPATTERNA ;
}

// We need to find out a way of determining which Pattern is running and so give the correct load value to the PIT.

void toggleRateTask() {
		
    switch (PatternRateState) {
			
				case SLOWPATTERNA:
					  setTimer(0, pitSlowCountPatternA) ;
            if (pressedB1_ev) {                   // signal received
                pressedB1_ev = false ;            // acknowledge
								PatternRateState = FASTPATTERNA ;           // ... the new state
            }
						if (pressedToggleB2){
							PatternRateState = SLOWPATTERNA ;
							pressedToggleB2 = false;
						}
            break ;
			
			
        case FASTPATTERNA:
						setTimer(0, pitFastCountPatternA) ;
            if (pressedB1_ev) {                   // signal received
                pressedB1_ev = false ;            // acknowledge
                PatternRateState = SLOWPATTERNA ;   // ... the new state
            }
						if (pressedToggleB2){
							// I made another signal, because pressedB2_ev is not reliable has it can be cleared by the interrupt and so we have no clue that it happened when changing load value.
							PatternRateState = FASTPATTERNB ;
							pressedToggleB2 = false;
						}
            break ;
						
				
				case SLOWPATTERNB:
						setTimer(0, pitSlowCountPatternB) ;
            if (pressedB1_ev) {                   	 // signal received
                pressedB1_ev = false ;            	 // acknowledge
								PatternRateState = FASTPATTERNB ;    // ... the new state
            }
						if (pressedToggleB2){
							PatternRateState = SLOWPATTERNA ;
							pressedToggleB2 = false;
						}
            break ;
						
								
				case FASTPATTERNB:
						setTimer(0, pitFastCountPatternB) ;
            if (pressedB1_ev) {                   // signal received
                pressedB1_ev = false ;            // acknowledge
								PatternRateState = SLOWPATTERNB ;           // ... the new state
            }
						if (pressedToggleB2){
							PatternRateState = FASTPATTERNA ;
							pressedToggleB2 = false;
						}
            break ;
							
  }
}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
    configureLEDforPWM() ;            // Configure LEDs for PWM control
    configureButtons(B1MASK, false) ; // ConfigureButtons B1 for polling
    configurePIT(0) ;        // configure PIT channel 0
    configureTPMClock() ;    // clocks to all TPM modules
    configureTPM0forPWM() ;  // configure PWM on TPM0 (blue LED)
    configureTPM2forPWM() ;  // configure PWM on TPM2 (red, green LEDs)
   
	
	
		// Added configuration for Activity 4.
		configureButtons(B2MASK, false) ; // Configure button B2 for polling so we can switch state diagrams.
		initPollB2Task();
		
		
	
    Init_SysTick(1000) ;  // initialse SysTick every 1 ms

    // start everything
    setLEDBrightness(Red, 0) ;
    setLEDBrightness(Green, 0) ;
    setLEDBrightness(Blue, 0) ;

    initPollB1Task() ;       // initialise task state
    initToggleRateTask() ;   // initialise task state 
		startTimer(0) ;
		 // startTimer(1); 
    waitSysTickCounter(10) ;  // initialise delay counter
    while (1) {      // this runs forever
        pollB1Task() ;  // Generate signals for a simulated button
				/* Activity 4 */
				pollB2Task();  // Polls specific button for pressed event.
        toggleRateTask();    // Toggle LED update rate on every press signal
			
			
				
        // delay
        waitSysTickCounter(10) ;  // cycle every 10 ms 
    }
}
