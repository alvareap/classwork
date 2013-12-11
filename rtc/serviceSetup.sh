#!/bin/bash
# By Alex Alvarez, alvarep@rose-hulman.edu
# This script creates a new directory if passed as the parameter, and places the rtc script and service files in those given directories. It also manipulates the service file such that the working directories change with the given directory parameter.
# If no paramter is given the working directory is used as the service location

if [ "$1" ]; then

  DIR=$1
  $(mkdir $1)
  

else

DIR=$(pwd)
  
fi


SED_ARG="-e 's@WorkingDirectory=.*@WorkingDirectory=$DIR@' rtc-ds1307.service" 

eval sed "$SED_ARG" > tmp.txt

mv tmp.txt rtc-ds1307.service



cp rtc-ds1307.service $DIR
cp clock_init.sh $DIR

systemctl enable $DIR/rtc-ds1307.service
