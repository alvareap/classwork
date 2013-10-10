#!/bin/bash
# This file sets up analog inputs for beagle


SLOTS=/sys/devices/bone_capemgr.*/slots
PINS=/sys/kernel/debug/pinctrl/44e10800.pinmux/pins
echo cape-bone-iio > $SLOTS

