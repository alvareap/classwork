These files are meant to be used with the DS1307 RTC from Adafruit.
By Alex Alvarez

see http://elinux.org/Adafruit:_RTC_DS1307#Bone_Usage


clock_init.sh:
------------------------------------------
interacts with the RTC, reading from the RTC if the host is out of date, and overwriting the system clock and overwriting the RTC if the host is not out of date, so as to account for adding +/- 2 seconds daily.

set "ln -sf /usr/share/zoneinfo/*/* /etc/localtime" as needed for your local timezone


serviceSetup.sh
------------------------------------------
This file manipulates the rtc-ds1307.service to run from a paramaterized directory. It copies over clock_init.sh, and rtc-ds1307.service to the given directory, and manipulates the variables inside of the service file to react accordingly.

In order to manually start the service execute:
systemctl start rtc-ds1306.service

and stop:
systemctl stop rtc-ds1306.service



The RTC setup information was taken from Adafruit's webpage:
http://learn.adafruit.com/adding-a-real-time-clock-to-beaglebone-black/set-rtc-time

