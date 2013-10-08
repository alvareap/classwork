TOOLS_CFLAGS	:= -Wstrict-prototypes -Wshadow -Wpointer-arith -Wcast-qual \
		   -Wcast-align -Wwrite-strings -Wnested-externs -Winline \
		   -W -Wundef -Wmissing-prototypes
#
# Programs
#
all:	gpio-int-test myi2cget tempSensor

gpio-int-test:  gpio-int-test.o gpio-utils.o
	$(CC) $(LDFLAGS) -o $@ $^


myi2cget:   myi2cget.o
	$(CC) $(LDFLAGS) -o $@ $^

tempSensor: tempSensor.o
	$(CC) $(LDFLAGS) -o $@ $^
#
# Objects
#

%.o: %.c
	$(CC) $(CFLAGS) $(TOOLS_CFLAGS) -c $< -o $@

clean:
	rm gpio-int-test.o gpio-utils.o myi2cget myi2cget.o tempSensor.o
