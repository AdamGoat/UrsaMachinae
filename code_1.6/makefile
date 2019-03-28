all:
	#g++ -std=c++11 -c *.cpp
	#ar cr AdafruitDCMotorHAT.a Adafruit_MotorHAT.o PWM.o
	g++ -o DriveTest5 DriveTest5.cpp AdafruitDCMotorHAT.a -lwiringPi -lpthread -lcrypt -lpigpio -lrt -lpigpiod_if2 -g
	g++ -o Drive Drive.cpp AdafruitDCMotorHAT.a -lwiringPi -lpthread -lcrypt -lpigpio -lrt -lpigpiod_if2 -g
	g++ -o PickUpBlock PickUpBlock.cpp AdafruitDCMotorHAT.a -lwiringPi -lpthread -lcrypt -lpigpio -lrt -lpigpiod_if2 -g
	g++ -o DriveForwardAndBack DriveForwardAndBack.cpp AdafruitDCMotorHAT.a -lwiringPi -lpthread -lcrypt -lpigpio -lrt -lpigpiod_if2 -g
	g++ -o serialTest serialTest.cpp AdafruitDCMotorHAT.a -lwiringPi -lpthread -lcrypt -lpigpio -lrt -lpigpiod_if2 -g
	g++ -o LazyMorganTest LazyMorganTest.cpp AdafruitDCMotorHAT.a -lwiringPi -lpthread -lcrypt -lpigpio -lrt -lpigpiod_if2 -g
