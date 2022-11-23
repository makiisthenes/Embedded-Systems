/* ------------------------------------------
       ECS642/ECS714 Lab2

   The aim of this lab exercise is to understand how to react
   to button presses.
  -------------------------------------------- */
  
	// Make sure the code, is done using design requirements and guidlines for a cyclic system, 
	// https://qmplus.qmul.ac.uk/mod/page/view.php?id=1396803
	// Design Rules: Cyclic Systems:
	// - The programs has a single main loop.
	// One delat, the loop is timed by a delay, of this magnitude in the program.
	// Tasks, program is organised into one or more tasks each implemented in a function called the main loop. 
	// Variables holding tasks state are clearly distinguised and are only accessed only in the task to which they belong.
	// Communication between tasks, communicate between tasks are clearly lablled and used appropriately. 
	// Local variables, temporary varaibles not declared globally when this is not needed.
	// Interrupt handlers, interrupt handlers, if any areclearly distinguishled from other function.
	
	
	
#include <stdbool.h>
#include <stdint.h>
#include <MKL25Z4.h>

#include "..\inc\SysTick.h"
#include "..\inc\led.h"
#include "..\inc\button.h"

/* --------------------------------------
     Documentation of the Given Code
     ==============================

     Behaviour
     ---------     
     Two buttons on the shield are used:
        * Button B1 is polled. Each press toggles the state of the LED1 
        * Button B5 has an interrupt handler. Each press toggles state of LED5
     
     Files
     -----
     The following file may need to be changed to complete the exercise
     
     main.c  Contains the main loop with 3 tasks and an interrupt handler
     
     The following files should not be changed:
       mask.h   Definition of the MASK macro
       button.h  Definitions and API for shield buttons
       button.c  Code to configure buttons and detect button position
       led.h   Definitions and API for shield LEDs
       led.c   Code to configure and switch shield LEDs
       SysTick.h  Definitions and API for SysTick timer
       SySTick.c  Code for SySTick timer
 -------------------------------------- */

/*----------------------------------------------------------------------------
  Variables for communication
*----------------------------------------------------------------------------*/
bool pressedB1_ev ;  // set by task1 (polling) and cleared by task 2
volatile uint32_t pressedB5_ev ;  // set by interrupt handler
                                  //   Cleared by the task3
                                  
/*----------------------------------------------------------------------------
  task1pollB1
  
  This task polls button B1
*----------------------------------------------------------------------------*/
int b1State ;        // Current state - corresponds to position
int b1BounceCount ;

void initTask1() {
    b1State = BOPEN ;
    pressedB1_ev = false ; 
    b1BounceCount = 0 ;
}

void task1pollB1() {
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




// Iteration 2: [need to test.]

/*
#define MOVE_LIT1TO5 (12)
#define MOVE_LIT5TO1 (13)
#define TASK1_COUNT 100 
int task1_count = TASK1_COUNT;
int task1_state = MOVE_LIT1TO5; // Set initial state.
int led_pos = -1; // So index 0 is used first.



// NextLit function. 
int lit1to5(int n){
	// Set the led on at correct index and turns off the other. 
	int temp_pos = n;
	if (n+1 > 4){
		ledOnOff(n, LED_OFF);
		temp_pos = 0;
		ledOnOff(temp_pos, LED_ON);
	}else{
		// LED initially a value such as 
		ledOnOff(n, LED_OFF);
		temp_pos = n+ 1;
		ledOnOff(temp_pos, LED_ON);
	}
	return temp_pos; // return new value of led_pos to main function.
}


// NextLit function.
int lit5to1(int n){
	int temp_pos = n;
	if (n-1 < 0){
		ledOnOff(temp_pos, LED_OFF); // LED 1 - index 0 is turned off.
		temp_pos = 4;
		ledOnOff(temp_pos, LED_ON);
	}else{
		// Within range. 
		ledOnOff(temp_pos, LED_OFF);
		temp_pos = temp_pos - 1;
		ledOnOff(temp_pos, LED_ON);
	}
	return temp_pos; // return new value of led_pos to main function.
}



void task1(){  // Activity 2
	if (task1_count > 0) task1_count -- ;
	switch (task1_state){
		
		case MOVE_LIT1TO5:  // Forward State.
			if (pressedB1_ev) {
				// Then we have detected a button press and so we should change the state.
				task1_state = MOVE_LIT5TO1;
				pressedB1_ev = false;
			}
			else if (task1_count == 0){
				// Then check to see if the count has went to 0, or not. 
				led_pos = lit1to5(led_pos);
				task1_count = TASK1_COUNT;
			}
			break;
		
			
		case MOVE_LIT5TO1:  // Backward State. 
			if (pressedB1_ev) {
				// Then we have detected a button press and so we should change the state.
				task1_state = MOVE_LIT1TO5;
				pressedB1_ev = false;  // clear the signal and go to the other state. 
			}
			else if (task1_count == 0){
				led_pos = lit1to5(led_pos); // Changes the led to turn on and updates the led_pos also. 
				task1_count = TASK1_COUNT;
			}
			break;
	}
}


*/


// Activity 3 Code Written here (17/10/2022):

// Helper function for min use cases.
void setAllLED(on_off){
      // Sets all LEDs on, so we can switch of only indexed light, to create illusion of changing led.
      for (int i=0; i<5; i++){
                  ledOnOff(i, on_off);
            }
}


// Functions described in the state diagram mentioned here:
// NextLit function [Forward].
int lit1to5(int n){
      // Set the led on at correct index and turns off the other.
      int temp_pos = n;
      if (n+1 > 4){
            setAllLED(LED_OFF);
            temp_pos = 0;
            ledOnOff(temp_pos, LED_ON);
      }else{
            // LED initially a value such as
            setAllLED(LED_OFF);
            temp_pos = n+ 1;
            ledOnOff(temp_pos, LED_ON);
      }
      return temp_pos; // return new value of led_pos to main function.
}


// NextLit function [Backward].
int lit5to1(int n){
      int temp_pos = n;
      if (n-1 < 0){
            setAllLED(LED_OFF);
            // ledOnOff(temp_pos, LED_OFF); // LED 1 - index 0 is turned off.
            temp_pos = 4;
            ledOnOff(temp_pos, LED_ON);
      }else{
            // Within range.
            setAllLED(LED_OFF);
            temp_pos = temp_pos - 1;
            ledOnOff(temp_pos, LED_ON);
      }
      return temp_pos; // return new value of led_pos to main function.
}




// NextLit Inverted function [Forward].
int unlit1to5(int n){
      int temp_pos = n;
      if (n+1 > 4){  // Check if incrementing will make it out of bounds, if not use else condition.
            setAllLED(LED_ON);
            temp_pos = 0; // Go back to normal index.
            ledOnOff(temp_pos, LED_OFF);
      }else{
            // Here the incremented value is within the set.
            temp_pos = temp_pos + 1;
            setAllLED(LED_ON);
            ledOnOff(temp_pos, LED_OFF);
      }
      return temp_pos; // return new value of led_pos to main function.
}



// NextLit Inverted function [Backward].
int unlit5to1(int n){
      int temp_pos = n;
      if (n-1 < 0){  // In this condition a decrement will lead to out of bounds, setting to 4th index.
            setAllLED(LED_ON);
            temp_pos = 4;
            ledOnOff(temp_pos, LED_OFF);
      }else{  // Decrement will lead within bounds, so is fine.
            setAllLED(LED_ON);
            temp_pos = temp_pos - 1;
            ledOnOff(temp_pos, LED_OFF);
      }
            
      return temp_pos; // return new value of led_pos to main function.
}



/*----------------------------------------------------------------------------
 * Interrupt Handler GPIO A
 *    - Clear the pending request
 *    - Test the bit to see if it generated the interrupt (switch pressed)
  ---------------------------------------------------------------------------- */

/*
Another way of determining whether the button has been pressed is to use event-triggering instead of polling.
Here the processor gets a notification from hardware that a specific event has occured and processing is needed.
Of course a context switch will occur and allow for this interrupt to by handled.
Interrupts are send to the processor has interrupt requests.
In this case we need a ISR (Interrupt Service Routine), which in essence is a handler that deals with the interrupt by performing some processing.

I think the lab wants us to keep b1 as a polling function and leave b5 as a interrupt handler.



*/




// Using Embedded Systems Fundamentals with ARM Cortex Pg 60 Chapter 3: Basics of Software Concurrency.
// Do we need to deal with bounce problem, because that is tiresome.
bool inverted_leds;
void PORTA_IRQHandler(void) {  
    NVIC_ClearPendingIRQ(PORTA_IRQn);
    if ((PORTA->ISFR & B5MASK)) {
                  // Check if b5 is pressed, by using PDIR. Located on Port A.
                  // FPTA, read this bypasses Peripheral Bridge and allows you to connect to pin more faster.
                  // inc\button.h(40) : #define B5MASK MASK(B5pin)
                        // B5 button has been pressed.
                        pressedB5_ev = 1 ;  // volatile set by Interrupt Handler.
                        
                  }  
   
    // Clear status flags
    PORTA->ISFR = B5MASK ;   // PORTA -> ISFR = Oxffffffff;
}





#define MOVE_LIT1TO5 (1)
#define MOVE_LIT5TO1 (2)
#define MOVE_UNLIT1TO5 (3)
#define MOVE_UNLIT5TO1 (4)
#define TASK2_COUNT 100
int task2_count = 2;  // = TASK2_COUNT;
int task2_state = MOVE_LIT1TO5; // Set initial state.
int task2_led_pos = -1; // So index 0 is lit up first.

void task2(){  
      // Activity 3
      if (task2_count > 0) task2_count -- ;
      switch (task2_state){
            
            // Listing of all cases.
            case MOVE_LIT1TO5:  // Forward State.
                  
            // First check if B5 has been pressed so we can invert.
                  if (pressedB5_ev){
                        task2_state = MOVE_UNLIT1TO5;  // Change state to the inverted version.
                        pressedB5_ev = false; // Clear the signal, set by the interrupt handler.
                        break;
                  }
                  if (pressedB1_ev) {
                        // Then we have detected a button press and so we should change the state.
                        task2_state = MOVE_LIT5TO1;
                        pressedB1_ev = false;
                        break;
                  }
                  if (task2_count == 0){
                        // Then check to see if the count has went to 0, or not.
                        task2_led_pos = lit1to5(task2_led_pos);
                        task2_count = TASK2_COUNT;
                  
                        
                  }
                  break;
                  // Using additional breaks, if unsure how else if conditions work in C and error occurs.
            
                  
            case MOVE_LIT5TO1:  // Backward State.
                  if (pressedB5_ev){
                        task2_state = MOVE_UNLIT5TO1;  // Change state to the inverted version.
                        pressedB5_ev = false; // Clear the signal, set by the interrupt handler.
                        
                  }
                  if (pressedB1_ev) {
                        // Then we have detected a button press and so we should change the state.
                        task2_state = MOVE_LIT1TO5;
                        pressedB1_ev = false;  // clear the signal and go to the other state.
                  }
                  if (task2_count == 0){
                        
                        task2_led_pos = lit5to1(task2_led_pos); // Changes the led to turn on and updates the led_pos also.
                        task2_count = TASK2_COUNT;
                        
                  }
                  break;
            
                  
            case MOVE_UNLIT1TO5:
                  // B5 PRESSED.
                  if (pressedB5_ev){
                        task2_state = MOVE_LIT1TO5;  // Change state to the inverted version.
                        pressedB5_ev = false; // Clear the signal, set by the interrupt handler.
                        
                  }
                  else if (pressedB1_ev) {
                        // Then we have detected a button press and so we should change the state.
                        task2_state = MOVE_UNLIT5TO1;
                        pressedB1_ev = false;
                        
                  }
                  else if (task2_count == 0){
                        // Then check to see if the count has went to 0, or not.
                        task2_led_pos = unlit1to5(task2_led_pos);
                        task2_count = TASK2_COUNT;
                        
                  }
                  break;
                  // Using additional breaks, if unsure how else if conditions work in C and error occurs.
            
            
            
            case MOVE_UNLIT5TO1:
                  if (pressedB5_ev){
                        task2_state = MOVE_LIT5TO1;  // Change state to the inverted version.
                        pressedB5_ev = false; // Clear the signal, set by the interrupt handler.
                        
                  }
                  else if (pressedB1_ev) {
                        // Then we have detected a button press and so we should change the state.
                        task2_state = MOVE_UNLIT1TO5;
                        pressedB1_ev = false;  // clear the signal and go to the other state.
                  }
                  else if (task2_count == 0){
                        task2_led_pos = unlit5to1(task2_led_pos); // Changes the led to turn on and updates the led_pos also.
                        task2_count = TASK2_COUNT;
                  }
                  break;
      }

}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
    configureLEDs() ;                         // configure LED1-5 using GPIO
    configureButtons(B1MASK | B5MASK, true) ; // configures buttons B1 and B5 (with an interrupt)
    initTask1() ;  // initialise task1 state
    // initTask2() ;  // initialise task2 state
    // initTask3() ;  // initialise task3 state
    Init_SysTick(1000) ; // initialse SysTick every 1ms
    waitSysTickCounter(10) ;  // wait 10 ms. 
    while (1) {      // this runs for ever
        task1pollB1() ;
			  // task1(); // This is excercise 2, need to test.
				task2();
        // delay
        waitSysTickCounter(10) ;  // cycle every 10 ms - not explained until week 4
    }
}