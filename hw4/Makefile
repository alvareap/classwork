
CC := gcc
CFLAGS := -g

# .x rule

all: main step

step : lightFollower.o gpio.h
	$(CC) $(CFLAGS) $^  


main: lightFollower.c gpio.h
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	rm lightFollower.o gpio.h.gch
