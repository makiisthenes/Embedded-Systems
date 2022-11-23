/* ------------------------------------------
       ECS642/ECS714 Lab3

   The aim of this lab exercise is to investigate and 
   measure timings:
        - the cycle time of the cyclic system
        - the proportion of the cycle spent execute code / waiting
        - the time to respond to an interrupt
  -------------------------------------------- */
  
#include <stdbool.h>
#include <stdint.h>
#include <MKL25Z4.h>

#include "..\inc\SysTick.h"
#include "..\inc\button.h"
#include "..\inc\rgb.h"

/* --------------------------------------
    Documentation of the Given Code
    ==============================

    Behaviour
    ---------     
    There are two tasks
    * Task 1 flashes the RED and GREEN leds in turn
    * Task 2 toggles the BLUE led when button B5 is pressed.

    Two buttons are configured with an interrupt:
        Button B5 - this has hardware debounce and is designed for 
                    use with an interrupt
        Button B1 - has no hardware debounce so would not usually 
                    be used with an interrupt handler. 

     Files
     -----
     The following file may need to be changed to complete the exercise
     
     main.c  Contains the main loop with 2 tasks and an interrupt handler
     
     The following files should not be changed:
       mask.h     Definition of the MASK macro
       button.h   Definitions and API for shield buttons
       button.c   Code to configure buttons and detect button position
       rgb.h      Definitions and API for tri-colour LEDs on the development board
       rgb.c      Code to configure and operate the tri-colour LEDs on the development board
       SysTick.h  Definitions and API for SysTick timer
       SySTick.c  Code for SySTick timer
 -------------------------------------- */

/*----------------------------------------------------------------------------
  Configuration of additional GPIO outputs
       PTE23 - known as OUT1
       PTE30 - known as OUT2
  These outputs are not used in the given code, but are needed for measurements
*----------------------------------------------------------------------------*/
#define OUT1 (23)
#define OUT2 (30)

void configureOUT() {
  // Configuration steps
  //   1. Enable clock to GPIO port E
  //   2. Connect GPIO pins to GPIO 
  //   3. Set GPIO direction to output
  //   4. Ensure outputs are off

  // Enable clock to ports E
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK ;

  // Make 2 pins GPIO
  PORTE->PCR[OUT1] &= ~PORT_PCR_MUX_MASK;
  PORTE->PCR[OUT1] |= PORT_PCR_MUX(1);
  PORTE->PCR[OUT2] &= ~PORT_PCR_MUX_MASK;
  PORTE->PCR[OUT2] |= PORT_PCR_MUX(1);

  // Set ports to outputs
  PTE->PDDR |= MASK(OUT1) | MASK(OUT2);

  // Turn off outputs
  PTE->PCOR = MASK(OUT1) | MASK(OUT2);
  // end of configuration code
}

/*----------------------------------------------------------------------------
  Special configuration of B1 with an interrupt

  B1 is designed for polling but here we use an interrupt
  As a result, button bounce may generate multiple interrupts    
*----------------------------------------------------------------------------*/
void configureB1withInterrupt() {
 
    /* Select GPIO and enable pull-up resistors and falling edge interrupt */
    PORTA->PCR[B1pin] &= ~(PORT_PCR_MUX_MASK | PORT_PCR_IRQC_MASK)  ;          
	PORTA->PCR[B1pin] |= PORT_PCR_MUX(1) | PORT_PCR_PS_MASK | PORT_PCR_PE_MASK  // PS : 1 means PULL UP and PE: Enable the PULL UP/DOWN. 
                         | PORT_PCR_IRQC(0x0A) ; // falling edge , IRQC number 10, meaning falling edge, correctly said. 
    // set as input
    PTA->PDDR &= ~B1MASK ;
 
    /* Enable Interrupts for port A*/
    NVIC_SetPriority(PORTA_IRQn, 128); // 0, 64, 128 or 192
    NVIC_ClearPendingIRQ(PORTA_IRQn);  // clear any pending interrupts
    NVIC_EnableIRQ(PORTA_IRQn);  
}

/*----------------------------------------------------------------------------
  Variables for communication
*----------------------------------------------------------------------------*/
volatile uint32_t pressedB5_ev ;  // set by interrupt handler
                                  //   Cleared by the task2


/*----------------------------------------------------------------------------
  task1RedGreen
  
  This task flashes the red and green LEDs. It behaviour is not important,
  but it is the execution of this code that we can measure.
  *----------------------------------------------------------------------------*/
#define RED_ON (0)
#define GREEN_ON (1)
#define FLASHTIME (100)

int stateTask1 ;  // this variable holds the current state
int task1Count ;

void initTask1() {
    stateTask1 = RED_ON;     // initialise the state 
    setRGB(RED, RGB_ON) ;    // turn the red LED on
    setRGB(GREEN, RGB_OFF) ; // turn the green LED off
    task1Count = FLASHTIME ; // set the flash time
}

void task1RedGreen() {
    if (task1Count > 0) task1Count-- ;
    switch (stateTask1) {
        case RED_ON:  // the state names are defined in the gpio.h file
            if (task1Count == 0) {      // RED on time completed
                setRGB(RED, RGB_OFF) ;  // turn red LED off
                setRGB(GREEN, RGB_ON) ; // turn green LED on
                stateTask1 = GREEN_ON ; // ... the new state
                task1Count = FLASHTIME ;
            }
            break ;

        case GREEN_ON:
            if (task1Count == 0) {        // GREEN on time completed
                setRGB(GREEN, RGB_OFF) ;  // turn green LED off
                setRGB(RED, RGB_ON) ;     // turn red LED on
                stateTask1 = RED_ON ;     // ... the new state
                task1Count = FLASHTIME ;
            }
            break ;
    }
}

/*----------------------------------------------------------------------------
 * Interrupt Handler GPIO A
 *    - Clear the pending request
 *    - Test the bit to see if it generated the interrupt 
  ---------------------------------------------------------------------------- */
void PORTA_IRQHandler(void) {  
    NVIC_ClearPendingIRQ(PORTA_IRQn);
    if ((PORTA->ISFR & B1MASK)) {
        // Add code to respond to interupt generated by B1
        PTE -> PTOR = MASK(OUT1);
           
       // Add PTA4 Oscilloscope probe to B1 button.
    }

    if ((PORTA->ISFR & B5MASK)) {
        // Add code to respond to interupt generated by B5
        pressedB5_ev = 1 ;
    }
    // Clear status flags 
    PORTA->ISFR = B1MASK | B5MASK ; 
}

/*----------------------------------------------------------------------------
  task2Blue

  This task toggles the state of the blue LED when the 'B5 pressed' event occurs
     * The task is coded with only a single state, so no state variable is needed
     * The code writes directly to the toggle output register (rather than the 
       provided API) as this is useful for the lab. 
*----------------------------------------------------------------------------*/
#define BLUE_ON (0)
#define BLUE_OFF (1)

void initTask2() {
    setRGB(BLUE, RGB_OFF) ;  // turn the LED off
}

void task2Blue() {
    // there is one case for each state
    // each case has the same structure
    if (pressedB5_ev) {            // B1 has been pressed
        pressedB5_ev = false ;     // acknowledge the event
        PTD->PTOR = MASK(BLUE_LED_POS) ;
    }
}


/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
    configureRGB() ;                 // configure RGB using GPIO
    configureButtons(B5MASK, true) ; // configure button B5 (with an interrupt)
    configureB1withInterrupt() ;     // Special configuration of B1 with interrupt
    configureOUT() ;                 // configure additional outputs
    initTask1() ;                    // initialise task1 state
    initTask2() ;                    // initialise task2 state
    Init_SysTick(1000) ;             // initialse SysTick every 1ms
    waitSysTickCounter(10) ;
    while (1) {                   // this runs for ever
        PTE -> PSOR = MASK(OUT2);  // start the signal for oscilloscope.
        task1RedGreen() ;
        task2Blue() ;
        PTE -> PCOR = MASK(OUT2);  // Clear the signal for oscilloscope.
            
        // delay
        waitSysTickCounter(10) ;  // cycle every 10 ms
        PTE -> PTOR = MASK(OUT1);
    }
}
