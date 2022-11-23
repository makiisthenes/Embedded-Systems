# Lab 3 Given Code 

The code:

  * Flashes RED and GREEN
  * Toggles BLUE when button B5 is pressed

Button B5 is handled by an interrupt (possible because it has hardware debounce).

The code also configures
  * Two further outputs, for use with the oscilloscope. They are unused in this version.
  * Button B1 with an interrupt handler. As there is no hardware debounce, multiple 
    interrupts may be generated. The handler is empty.