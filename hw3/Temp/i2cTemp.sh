# This shell script iterfaces with i2c temperatures

ADDR=$1
I2CBUS=1; #This is actually bus 2 (0 based convention)

DEV=$(i2cdetect -y -r 1 | grep ${ADDR} ) 

if [ -n "${DEV}" ]; then
	echo "Device @ Address "$1" found"
else
	echo "No I2C device found at said address"
	exit
fi

# Temperature is held in register 0
TEMP=$(i2cget -y ${I2CBUS} 0x"$1" 0)


F_TEMP=$(( $TEMP *9/5 + 32  ))


echo "The Temperature @ I2C device "$1" is $F_TEMP F"

# sets TM to 1, i.e interrupt mode
i2cset -y ${I2CBUS} 0x"$1" 1 0x82 b

# Set lower threshold
i2cset -y ${I2CBUS} 0x"$1" 2 0x4b b
# Set higher threshold
i2cset -y ${I2CBUS} 0x"$1" 3 0x50 b 


echo "Done"
