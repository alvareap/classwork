
CC := gcc
CFLAGS := -g

# .x rule

etch : etch.o i2cbusses.o gpio-utils.o
	$(CC) $(CFLAGS) $^  


main.o : etch.c i2cbusses.h gpio-utils.h
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	rm etch.o i2cbusses.o gpio-utils.o
