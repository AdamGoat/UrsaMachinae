#include "projectDemo.h"

using namespace std;

Adafruit_MotorHAT  hat(0x61, 1600,-1,-1);
Adafruit_MotorHAT hat2(0x60,1600,-1,-1);

Adafruit_DCMotor& frontLeft  = hat.getDC(1);
Adafruit_DCMotor& frontRight = hat.getDC(2);
Adafruit_DCMotor& backLeft   = hat.getDC(3);
Adafruit_DCMotor& backRight  = hat.getDC(4);

Adafruit_DCMotor& liftMotor  = hat2.getDC(1);


int main(int argc, char* argv[]){
	cout << "Good Bye World" << endl;
	
	haltClaw();
	halt();
	return 0;
}


int haltClaw(){
	printf("HALT CLAW!\n");
	liftMotor.setSpeed(0);
	return 0;
}

	
int halt(){
	printf("HALT!\n");
	//stop all motors
	frontLeft.setSpeed(0);
	frontRight.setSpeed(0);
	backLeft.setSpeed(0);
	backRight.setSpeed(0);
	//liftMotor.setSpeed(0);
	return 0;
}
	
	

