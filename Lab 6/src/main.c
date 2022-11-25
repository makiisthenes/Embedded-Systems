
/*----------------------------------------------------------------------------
    Given code for Embedded Systems Lab 6
    
    This project illustrates the use of 
      - threads
      - event flags
      - a message queue

    The behaviour is: 
        - the green LED can be turned on or off using command entered on a terminal 
          emulator (see lab sheet) and send over the USB link
        - if the command are entered in the wrong order, the system enters an
          error state: the red LED flashes
        - to exit from the error state the 'reset' command must be entered
        - the system is initialised in the error state
        
    There are three threads
       t_serial: waits for input from the terminal; sends message to t_greenLED
       t_greenLED: turns the green LED on and off on receipt of the message. Enters
         an error state when the message is not appropriate
       t_redLED: flashes the red on and off when an error state is signalled by the
         t_greenLED
    
    Message queue: 
       * Message are put in the queue t_serial
       * Messages are read from the queue by t_greenLED

    Signal using event flags
       * A flag is set by t_greenLED
       * The thread t_redLED waits on the flag

 *---------------------------------------------------------------------------*/
 
#include "cmsis_os2.h"
#include "string.h"

#include <MKL25Z4.h>
#include <stdbool.h>
#include "gpio.h"
#include "serialPort.h"

#define RESET_EVT (1)
osEventFlagsId_t errorFlags ;       // id of the event flags
osMessageQueueId_t controlMsgQ ;    // id for the message queue


/*--------------------------------------------------------------
 *   Thread task3_t
 *     
 *--------------------------------------------------------------*/
 
 /*
 int faster_func(int set_delay){
	set_delay = set_delay + 500 % 4000 ;
	 if (set_delay == 0){
			set_delay = 500;
	 }
	 return set_delay;
 }
 */
 int modulo(int n, int p)
{
int r = n%p;
    if(((p > 0) && (r < 0)) || ((p < 0) && (r > 0)))
        r += p;
    return r;
}
 
 
const int timings[] = {500, 1000, 1500, 2000, 2500, 3000, 3500}; // in system ticks.
 
 
 
osThreadId_t task3_t;      /* id of thread to toggle green led */

// Green LED states
enum {RED_STATE, GREEN_STATE} task3State;

enum controlMsg_t {faster, slower} ;  // type for the messages

// We will measure based on 2 tick values.
uint32_t init_tick;  // for measuring ticks.
uint32_t measured_tick;  // for measuring ticks for green.

// we know that 1000 ticks = 1 second. 


void task3Thread (void *arg){
    int task3State = RED_STATE ;
	  int delay_index = 4; // 2 seconds.
		int delay;
    greenLEDOnOff(LED_OFF);
    enum controlMsg_t msg ;
    osStatus_t status ;   // returned by message queue get
    while (1) {
				
				// We can use osEventFlagsWait to wait the given time, if the timeout hasnt changed by the signal just carry on.
        // wait for message from queue
            switch (task3State) {
                case RED_STATE:
									  greenLEDOnOff(LED_OFF);
									  redLEDOnOff(LED_ON);
										init_tick = osKernelGetTickCount(); // we will obtain some tick value. 
										delay = timings[delay_index];
										while (1){
											status = osMessageQueueGet(controlMsgQ, &msg, NULL, delay);  // give a timeout of delay.
											if (status == osOK){
												if (msg == faster) {           
														delay_index = modulo(delay_index - 1,7);
														delay = timings[delay_index] ;
														measured_tick = osKernelGetTickCount();
													  if (delay <= (measured_tick - init_tick)){
															// as delay will have some new faster value, 2000, new value is 1500, however 1600 has passed. 	
															// 1500 - 1600 = -100 < 0 == 0.
															delay = 0; 
															init_tick = osKernelGetTickCount(); // so tick is updated for this state.
														}else{
															// 1500 - 1200 = 300 > 0 == 300.
															delay = (delay - (measured_tick - init_tick));// this is the ticks left for this system.
															// 
															// This will run again with the updated delay, and then timeout due to osErrorResource.
														}
														init_tick =  osKernelGetTickCount();
														
												}
												else if (msg == slower){
														delay_index = modulo(delay_index + 1, 7);
														delay = timings[delay_index] ;
														// 2000 - 1200 = 800. 
														measured_tick = osKernelGetTickCount();
														if (delay <= (measured_tick - init_tick)){
															delay = 0;
														}else{
															delay = delay - (measured_tick - init_tick); // theres is still enough ticks for this increase
														}
														init_tick =  osKernelGetTickCount();
														
												}
											}if (status == osErrorTimeout || status == osErrorResource){
												break; // if no new message within the period, we will break and go to next state.
											}
										}
										
										// We keep the delay and and then transition to the next state.
										task3State = GREEN_STATE;
                    break ;
										
                    
                case GREEN_STATE:
										redLEDOnOff(LED_OFF);
										greenLEDOnOff(LED_ON);
								    init_tick = osKernelGetTickCount();
										delay = timings[delay_index];
										while (1){
											status = osMessageQueueGet(controlMsgQ, &msg, NULL, delay);  // give a timeout of delay. 
											if (status == osOK){
												if (msg == faster) {            
														delay_index = modulo(delay_index-1,7);
														// Now instead of setting delay instantly, we check to see how many ticks has went by. 
														delay = timings[delay_index] ;
														measured_tick = osKernelGetTickCount();
													  if (delay <= (measured_tick - init_tick)){
															// as delay will have some new faster value, 2000, new value is 1500, however 1600 has passed. 	
															// 1500 - 1600 = -100 < 0 == 0.
															delay = 0; 
															
														}else{
															// 1500 - 1200 = 300 > 0 == 300.
															delay = delay - (measured_tick - init_tick);// this is the ticks left for this system.
															// 
															// This will run again with the updated delay, and then timeout due to osErrorResource.
														}
														init_tick = osKernelGetTickCount();; // so tick is restarted for this state.
												}
												else if (msg == slower){                      
														delay_index = modulo(delay_index + 1,7);
														delay = timings[delay_index];
														measured_tick = osKernelGetTickCount();
														if (delay <= (measured_tick - init_tick)){
																delay = 0;
														}else{
																delay = delay - (measured_tick - init_tick); // theres is still enough ticks for this increase
														}
														init_tick =  osKernelGetTickCount();
												}
											}if (status == osErrorTimeout || status == osErrorResource){
												break; // if no new message within the period, we will break and go to next state.
											}
										}
										task3State = RED_STATE;
										break ;        
        
			}
	}
}


/*------------------------------------------------------------
 *  Thread t_command
 *      Request user command
 *      
 *
 *------------------------------------------------------------*/
osThreadId_t t_command;        /* id of thread to receive command */

/* const */ char prompt[] = "Command: faster / slower >" ;
/* const */ char empty[] = "" ;

void commandThread (void *arg) {
    char response[7] ;  // buffer for response string
    enum controlMsg_t msg ;
    bool valid ;
    while (1) {
        //sendMsg(empty, CRLF) ;
        sendMsg(prompt, NOLINE) ;
        readLine(response, 6) ;  // 
  			valid = true ;
        if (strcmp(response, "faster") == 0) {
            msg = faster ;
        } else if (strcmp(response, "slower") == 0) {
            msg = slower ;
        }else valid = false ;
        
				
        if (valid) {
            osMessageQueuePut(controlMsgQ, &msg, 0, NULL);  // Put message in the queue.
        } else {
            sendMsg(response, NOLINE) ;
            sendMsg(" not recognised", CRLF) ;
        }
    }
}
/*
command thread, will take input from serial connection, and check different conditions for commands.
if command is valid we put a message in the controlMsgQ, queue. 

# This is obtained by the green thread by the message queue object.
status = osMessageQueueGet(controlMsgQ, &msg, NULL, osWaitForever);


*/




/*----------------------------------------------------------------------------
 * Application main
 *   Initialise I/O
 *   Initialise kernel
 *   Create threads
 *   Start kernel
 *---------------------------------------------------------------------------*/

int main (void) { 
    
    // System Initialization
    SystemCoreClockUpdate();

    // Initialise peripherals
    configureGPIOoutput();
    //configureGPIOinput();
    init_UART0(115200) ;

    // Initialize CMSIS-RTOS
    osKernelInitialize();
    
    // Create event flags
    errorFlags = osEventFlagsNew(NULL);
    
    // create message queue
    controlMsgQ = osMessageQueueNew(2, sizeof(enum controlMsg_t), NULL) ;

    // initialise serial port 
    initSerialPort() ;

    // Create  2 threads
    task3_t = osThreadNew(task3Thread, NULL, NULL); 
    
    t_command = osThreadNew(commandThread, NULL, NULL); 
    
    osKernelStart();    // Start thread execution - DOES NOT RETURN
    for (;;) {}         // Only executed when an error occurs
			
			
			// We basically in the main thread initialise 2 threads, these are:
			// -> we start with task3 thread, that runs the state machine until command changes.
			// -> t_command, this thread deals with command line over the uart connection. 
}
