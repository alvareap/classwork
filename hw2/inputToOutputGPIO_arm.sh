#!/bin/sh
# From http://wh1t3s.com/2009/05/14/reading-beagleboard-gpio/
#
# Read a GPIO input

GPIO_I=$1
GPIO_O=$2

cleanup() { # Release the GPIO port
  echo $GPIO_I > /sys/class/gpio/unexport
  echo $GPIO_O > /sys/class/gpio/unexport
  echo ""
  echo ""
  exit
}

# Open the GPIO ports
#
echo "$GPIO_I" > /sys/class/gpio/export
echo "in" > /sys/class/gpio/gpio${GPIO_I}/direction

# LED Ouput
echo "$GPIO_O" > /sys/class/gpio/export
echo "out" > /sys/class/gpio/gpio${GPIO_O}/direction


trap cleanup SIGINT # call cleanup on Ctrl-C

THIS_VALUE=`cat /sys/class/gpio/gpio${GPIO_I}/value`
LAST_VALUE=0
LED_STATUS=`cat /sys/class/gpio/gpio${GPIO_O}/value`
BUTTON_PRESS=0
NEWLINE=0

# Read forever

while [ "1" = "1" ]; do
  # next three lines detect state transition

  if [ "1" = "$THIS_VALUE" ]; then
	  if [ "1" = "$LAST_VALUE" ]; then
		  LED_STATUS=$((LED_STATUS^1))
		  echo "${LED_STATUS}" > /sys/class/gpio/gpio${GPIO_O}/value
		BUTTON_PRESS=1
	  fi 
  fi
 

  # sleep for a while
  if [ "${BUTTON_PRESS}" -eq "1" ]; then
	   sleep 0.2
	   BUTTON_PRESS=0
  fi

  sleep 0.05

  LAST_VALUE=$THIS_VALUE
  THIS_VALUE=`cat /sys/class/gpio/gpio${GPIO_I}/value`

done

cleanup # call the cleanup routine

