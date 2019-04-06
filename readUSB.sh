#!/bin/bash
if [ -e "/home/pi/Documents/UrsaMachinae/cords.json" ]; then
	echo "file already exists"
	echo "Run Drive Code" > /home/pi/Documents/UrsaMachinae/bootout
	cd /home/pi/Documents/UrsaMachinae/code_1.6
	/home/pi/Documents/UrsaMachinae/code_1.6/DriveCode1 >./bootReport
else
	echo "Waiting for flashdrive" > /home/pi/Documents/UrsaMachinae/bootout
	cd `findmnt -o TARGET -p -f -n -t vfat |head -1`
	echo "Found Drive: " > /home/pi/Documents/UrsaMachinae/bootout1
	echo $PWD > /home/pi/Documents/UrsaMachinae/bootout3
	cp ./m* ~pi/Documents/UrsaMachinae/cords.json
	echo "File copied"
	cd ~pi/Documents/UrsaMachinae/code_1.6
	./jsonTest
	echo "Done"
	./confirmBlink
fi
