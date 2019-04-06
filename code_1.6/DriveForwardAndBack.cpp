#include "DriveTest6.h"

using namespace std;

//This stuff is still in DriveTest5
/*Adafruit_MotorHAT  hat (0x61,1600,-1,-1);
Adafruit_MotorHAT  hat2(0x60,1600,-1,-1);

Adafruit_DCMotor& frontLeft  = hat.getDC(1);
Adafruit_DCMotor& frontRight = hat.getDC(2);
Adafruit_DCMotor& backLeft   = hat.getDC(3);
Adafruit_DCMotor& backRight  = hat.getDC(4);

position RobotPosition;
int fdArduino;
int curBlockX;
int curBlockY;
int curDestX;
int curDestY;
int numBlocks;
bool turning;*/

/*void ctrl_c_handler(int s){
	cout << "Caught signal " << s << endl;
	frontLeft.run(RELEASE);	
	frontRight.run(RELEASE);
	backLeft.run(RELEASE);
	backRight.run(RELEASE);
	
	halt();
	
	
	serialClose(fdArduino);
	
	hat.resetAll();
	hat2.resetAll();
	//gpioTerminate();
	
	exit(1);
}*/

int main(){
	initilizePigpiod();
	initilizeArduinoSerial();
	double feet;
	cin >> feet;
	waitForGreen();
	goForward(feet*TICKSPERFOOT,0);
	//pivotLeft(2);
	//goForward(feet*TICKSPERFOOT,0);
	return(0);
}
