This is homework 2 for Alexander Alvarez
It consists of several files


etch.c:
	the C file with the etch a sketch program
	uses ncurses to refresh screen
	a 'q' keyboard input erases the screen

etchASketch_x86:
	x86 compiled version of etch.c

inputOutputGPIO_arm:
	This arm-compiled executable reads the first parameter as the GPIO number for an input, and the second as the GPIO number for the output. When the input goes to logic 0, the output it toggle, and remains toggled until the input (button) is pressed again or is held down if held down for more then 2ms
