#ifndef SYSTICK_H
#define SYSTICK_H

#include <MKL25Z4.h>

// ECS642 Lab 1 header - provided version

// Function prototypes for cycle timing using SysTick

void Init_SysTick(uint32_t ticksPerSec) ;
void waitSysTickCounter(int ticks) ;

#endif
