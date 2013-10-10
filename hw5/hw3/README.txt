This directory contains project files for Homework 3 in ECE497 at Rose-Hulman Institute of Technology, Fall 2013.


In the folder Temp you will find the files related to reading from a temperature sensor over i2c

	tempSensor.c  --  top-level code, sets up temperature sensors, gpios

	gpio-int-test.c -- allows user to wait for logic 1 interrupt

	gpio-utils.c/h -- helper functions for gpio access

	myi2cget.c  --  allows C access to i2c devices
	i2c-dev.h -- helper functions for i2c access

	Makefile

In the folder Etch, you will find the following code with which to interface with an i2c led matrix

	etchSketch_arm -- arm executable
	
	etch.c	-- code to interface gpio's with i2c device, holds LED map array
	
	gpio-utils.c/h - functions to simplify gpio interfacing
	i2c-dev.h & i2cbusses.c/h -- functions to interface with i2c devices
	
	Makefile

by
Alex Alvarez
