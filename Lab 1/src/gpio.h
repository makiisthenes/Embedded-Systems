#ifndef GPIO_H
#define GPIO_H

// ECS714P/ECS642U Lab 1 header - provided version

// Create a bit mask (32 bits) with only bit x set
#define MASK(x) (1UL << (x))

// Freedom KL25Z LEDs pin numbers
#define RED_LED_POS (18)		// on port B
#define GREEN_LED_POS (19)	// on port B
#define BLUE_LED_POS (1)		// on port D

// Symbols for constants
#define OFF 0
#define ON 1
#define ONPERIOD 100 // time in 10ms units
#define OFFPERIOD 100 // time in 10ms units

// States

// These states are simply numbers, that we need to use to distinguise between the different cases in the lab work. Do we really need all of these states. 
#define WHITEOFF 0
#define WHITEON 1

#define YELLOWON 2
#define YELLOWOFF 3 

#define REDOFF 4
#define REDON 5

#define MAGENTAON 6
#define MAGENTAOFF 7

#define BLUEOFF 8
#define BLUEON 9

#define CYANOFF 10
#define CYANON 11

#define GREENOFF 12
#define GREENON 13


#endif
