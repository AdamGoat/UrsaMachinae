if [ -e "/home/pi/Documents/UrsaMachinae/cords.json" ]; then
	echo "file already exists"
	echo "Run Drive Code"

else
	echo "Waiting for flashdrive"
	cd `findmnt -o TARGET -p -f -n |head -1`
	cp ./m* ~pi/Documents/UrsaMachinae/cords.json
	echo "File copied"
	cd ~pi/Documents/UrsaMachinae/code_1.6
	./jsonTest
	echo "Done"
	./confirmBlink
fi
