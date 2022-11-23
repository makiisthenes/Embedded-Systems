/* ------------------------------------------
       ECS642/ECS714 Lab 5 Given Code
    
   Press B1 to use ADC to measure voltage
   When green LED is on, VR1 is measured
   When red LED is on, VR2 is measured
   Both measurements update 'scaled' variable    
  -------------------------------------------- */

#include <MKL25Z4.h>
#include <stdbool.h>
#include "../include/SysTick.h"
#include "../include/button.h"
#include "../include/rgb.h"
#include "../include/led.h"
#include "../include/adc.h"
#define PITCLOCK (10485760)


/* --------------------------------------
     Documentation
     =============
     This is a cyclic system with a cycle time of 10ms

     The file has a main function, with two tasks
        1. task1PollB1 polls button B1
        2. task2MeasureVR uses ADC to measure voltage on each button press
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

void initTask1PollB1() {
    b1State = BOPEN ;
    pressedB1_ev = false ; 
    b1BounceCount = 0 ;
}

void task1PollB1() {
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


/*----------------------------------------------------------------------------
   Task: task2MeasureVR

   This task use steh ADC to get the voltage 
*----------------------------------------------------------------------------*/

VR_t vrState ;        // Current state - which VR measured next

volatile uint16_t vr1 ;      // raw value - single ended
volatile uint16_t vr2 ;      // raw value - single ended
int scaledvr1 ; // voltage as a scaled integer - 3v shown as 300
int scaledvr2 ; 

void initTask2MeasureVR() {
    pressedB1_ev = false ; 
    vrState = VR1 ;
    setRGB(RED, RGB_OFF) ;
    setRGB(GREEN, RGB_ON) ;
}


void task2MeasureVR() {
    switch (vrState) {
        case VR1:
           vr1 = MeasureVR(VR1) ;
           scaledvr1 = (vr1 * 330) / 0xFFFF ;
                    // Max voltage is 3.3 v
                    // Using 16 bit accuracy, raw value is 0xFFFF
                    // Result represents voltage N.NN as integer NNN
           vrState = VR2 ;
           
					 
           break ;
        
        case VR2: 
            
						vr2 = MeasureVR(VR2) ;
						scaledvr2 = (vr2 * 330) / 0xFFFF ;  // this is in the lecturers.
						vrState = VR1 ;

            break ;
        
        default:
            break ; 
    }
}

/* For task 2, we need to intialise the PIT timer channel with interrupts
and calculate the load value in order to obtain 50Hz or 20 ms, using what we
learnt in lectures. */

void initPIT(){
	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK ;
	PIT->MCR &= ~PIT_MCR_MDIS_MASK ;
	PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK ;
  PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK ;  
	NVIC_SetPriority(PIT_IRQn, 128); // 0, 64, 128 or 192
  NVIC_ClearPendingIRQ(PIT_IRQn);  // clear any pending interrupts
  NVIC_EnableIRQ(PIT_IRQn);
	
	
	// setting ldval which we calculated timeout for. 
	// LDV = round(T * PITCLOCK)
	// 0.02 * 10485760-1 = 209714.2 
	PIT->CHANNEL[0].LDVAL = 104857 ;  // 100MHz/10ms, so we can measure at 50MHz/20ms for each potentiometer.
}


// We also need to deal with loading values into the LDVAL and starting the timer.


// Initialising a interrupt handler of the PIT to deal with the switching of the



/* Activity 3 */
void ledsOff(){
	setRGB(GREEN, 0);
	setRGB(RED, 0); 
	setRGB(BLUE, 0); 
	// I know ineffiecent but practical.
}

void shieldLEDSOff(){
	for (int i=0; i<5; i++) {
		ledOnOff(i, 0);
	}
}

void shieldLEDSOn(){
	for (int i=0; i<5; i++) {
		ledOnOff(i, 1);
	}
}



int v1max;
int v1min;

int v2max;
int v2min;





/*
int counter = 0;
static enum {ShieldsLEDsOn, ShieldsLEDsOff, FlashOff} task3state = ShieldsLEDsOn;


v1max = 298;
v1min = 0;

v2max = 298;
v2min = 0;


void task3(){
	switch (task3state){
		case ShieldsLEDsOn:
			
			setRGB(GREEN, 1);
			if (counter*(v1max-v1min) >= 350 * (scaledvr1-v1min) + 50 * (v1max-v1min)){
				counter = 0;
				ledsOff();
				// setRGB(GREEN, 1);
				task3state = ShieldsLEDsOff ;
			}
			else if (pressedB1_ev){
				pressedB1_ev = false;
				ledsOff();
				setRGB(RED, 1);
				task3state = FlashOff;	
			}
			break;
		
		
		case ShieldsLEDsOff:
			
			if (counter*(v2max-v2min) >= 350 * (scaledvr2-v2min) + 50 * (v2max-v2min)){
				counter = 0;
				ledsOff();
				setRGB(GREEN, 1);
				task3state = ShieldsLEDsOn;
			}
			else if (pressedB1_ev){
				pressedB1_ev = false;
				ledsOff();
				setRGB(RED, 1);
				task3state = FlashOff;
			}
			
			break;
		
		
		case FlashOff:
			
			if (pressedB1_ev){
				pressedB1_ev = false; 
				ledsOff();
				setRGB(GREEN, 1);
				task3state = ShieldsLEDsOn;
			}
			
			break;
	}
}





*/


/* Task 4 completed. */
int counter = 0;
static enum {MeasureMin, MeasureMax, ErrOn, ErrOff, ShieldLEDsOn, ShieldsLEDsOff, FlashOff} task4state = MeasureMin;
void task4(){
	switch (task4state){
		case MeasureMin:
			ledsOff();
			setRGB(GREEN, 1);
			
			if (pressedB1_ev){
				pressedB1_ev = false; // clear the signal. 
				v1min = scaledvr1;
				v2min = scaledvr2;
				ledsOff();
				setRGB(RED, 1);
				task4state = MeasureMax;
			}
	
			break;
		
		
		case MeasureMax:
			if (pressedB1_ev && scaledvr1 > v1min && scaledvr2 > v2min ){
				// check if constraints are realistic, such that max value is higher than both min values.
				pressedB1_ev = false; 
				v1max = scaledvr1;
				v2max = scaledvr2;
				counter = 0;
				ledsOff();
				setRGB(GREEN, 1);
				task4state = ShieldLEDsOn ; // Initialising of task 3 state model, copying here. 
			}
			else if (pressedB1_ev && (scaledvr1 <= v1min || scaledvr2 <= v2min)){
				// If this doesn't hold then scaledvr1/2 is lower than the min suggested.
				pressedB1_ev = false;
				ledsOff();
				setRGB(BLUE, 1);
				task4state = ErrOn;
				counter = 0;
			}
		
			break;
		
		
		case ErrOn:
			// condition needs 0.5 second transitions, we can counter value. calculate counter value for 0.5 seconds.
			// 100MHz PIT, meaning counter needs to be value of 50.
			if (counter >= 50){
				ledsOff();
				counter = 0;
				task4state = ErrOff;
			}
			break;
		
		
		case ErrOff:
			if (counter >= 50){
				ledsOff();
				setRGB(BLUE, 1);
				counter = 0;
				task4state = ErrOn;
			}
		
			break;
				
		
		case ShieldLEDsOn:
			setRGB(GREEN, 1);
			shieldLEDSOff();
			shieldLEDSOn();
		
			int measured_scaledvr1 = scaledvr1;
			if (measured_scaledvr1 < v1min) measured_scaledvr1 = v1min;
		  if (measured_scaledvr1 > v1max) measured_scaledvr1 = v1max;	
		
			
			if (counter*(v1max-v1min) >= 350 * (measured_scaledvr1-v1min) + 50 * (v1max-v1min) || counter*(v1max) >= 300 * v1max){
				counter = 0;
				// ledsOff();
				// setRGB(GREEN, 1);
				task4state = ShieldsLEDsOff ;
			}
			else if (pressedB1_ev){
				pressedB1_ev = false;
				ledsOff();
				setRGB(RED, 1);
				task4state = FlashOff;	
			}
			break;
		
		
		
		case ShieldsLEDsOff:
			// check whether scaled is within in the limits and fix them at boundaries.
			// can use scaledvr2 straight away.
			// 0setRGB(GREEN, 0);
			shieldLEDSOff();
			int measured_scaledvr2 = scaledvr2;
			if (measured_scaledvr2 < v2min) measured_scaledvr2 = v2min;
		  if (measured_scaledvr2 > v2max) measured_scaledvr2 = v2max; 
			
			if (counter*(v2max-v2min) >= 350 * (measured_scaledvr2-v2min) + 50 * (v2max-v2min) || counter*(v2max) >= 300 * v2max){
				counter = 0;
				ledsOff();
				setRGB(GREEN, 1);
				task4state = ShieldLEDsOn;
			}
			else if (pressedB1_ev){
				pressedB1_ev = false;
				ledsOff();
				setRGB(RED, 1);
				task4state = FlashOff;
			}
			
			break;
		
		
		case FlashOff:
			
			if (pressedB1_ev){
				pressedB1_ev = false; 
				ledsOff();
				setRGB(GREEN, 1);
				task4state = ShieldLEDsOn;
			}
			
			break;
	}
}








void PIT_IRQHandler(void) {
	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {
		PIT->CHANNEL[0].TFLG = PIT_TFLG_TIF_MASK ;
		// This will always be the case, as only one channel is enabled.
		counter = counter + 1;
		task2MeasureVR();
	}
}





/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {

    // Initialise peripherals
    configureButtons(B1MASK, false) ; // ConfigureButtons B1 for polling
    configureLEDs() ;                 // Configure shield LEDs
    configureRGB();                   // 
    
    // Initialise and calibrate ADC
    initADC() ; // Initialise ADC
    int calibrationFailed = ADC_Cal(ADC0) ; // calibrate the ADC 
    while (calibrationFailed) ; // block progress if calibration failed
    initADC() ; // Reinitialise ADC
    initVR1pin() ; // Not needed usually as default use, setting of MUX field.
    initVR2pin() ; // Not needed usually as default use, setting of MUX field.
		
		// task 2
		initPIT() ;
		PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK; // start the timer and interrupts.
	
    // initialse SysTick every 1 ms
    Init_SysTick(1000) ;  

    // Initialise tasks and cycle counter
    initTask1PollB1() ;  
    initTask2MeasureVR() ;
    waitSysTickCounter(10) ;  
    
    while (1) {      // this runs forever
        task1PollB1() ;    // Generate signals for a simulated button
				/* task3(); */
				
				task4();
			
        // delay
        waitSysTickCounter(10) ;  // cycle every 10 ms 
    }
}
