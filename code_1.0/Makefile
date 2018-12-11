all:
	g++ -std=c++11 -c *.cpp
	ar cr AdafruitDCMotorHAT.a Adafruit_MotorHAT.o PWM.o
	g++ -std=c++11 -o Drive Drive.cpp AdafruitDCMotorHAT.a -lwiringPi -lpthread -lcrypt -lpigpio -lrt -lpigpiod_if2 -g
	g++ -std=c++11 -o clawDown clawDown.cpp AdafruitDCMotorHAT.a -lwiringPi -lpigpio -lrt -lpigpiod_if2
	g++ -std=c++11 -o clawUp clawUp.cpp AdafruitDCMotorHAT.a -lwiringPi -lpigpio -lrt -lpigpiod_if2
	g++ -std=c++11 -o clawOpen clawOpen.cpp AdafruitDCMotorHAT.a -lwiringPi -lpigpio -lrt -lpigpiod_if2
	g++ -std=c++11 -o clawClose clawClose.cpp AdafruitDCMotorHAT.a -lwiringPi -lpigpio -lrt -lpigpiod_if2
	g++ -std=c++11 -o halt halt.cpp AdafruitDCMotorHAT.a -lwiringPi -lpigpio -lrt -lpigpiod_if2
	g++ -std=c++11 -o Demo projectDemo.cpp AdafruitDCMotorHAT.a -lwiringPi -lpthread -lcrypt -lpigpio -lrt -lpigpiod_if2 -g


clean:
	rm -rf StepperMotorTest *.a *.o
	rm -rf StepperMotorTest
	rm -rf DCMotorTest *.a *.o
	rm -rf Drive.o Drive
