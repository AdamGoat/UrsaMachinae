#include "projectDemo.h"

#define FORWARDSPEED 	 250
#define STRAFESPEED  	 250
#define PIVOTSPEED	 250
#define UP		 BACKWARD
#define DOWN		 FORWARD
#define LIFTSPEED	 150
#define LIFTTIME	 100000
#define LOWERTIME	 1000000
#define ARDUINOPIN	 17
#define SERVOPIN	 18 		//Dosn't change servo pin, just for record keeping
#define MIN_SERVO	 800
#define	MAX_SERVO	 1200

using namespace std;

void ctrl_c_handler(int s){
	cout << "Caught signal " << s << endl;
//	liftMotor.run(RELEASE);
//	haltClaw();
//	hat2.resetAll();
	exit(1);
}

int main(int argc, char* argv[]){

/*	signal(2, ctrl_c_handler);
	if (gpioInitialise() < 0) return -1;
	
	cout << "Hello World" << endl;
	cout << "Opening" << endl;
	if(RobotPosition.getClawPos() == 0)
		RobotPosition.switchClawPos();
	cout << "Old pos is " << RobotPosition.getClawPos() << endl;
	openClaw();
	usleep(1000000);
*/
	gpioInitialise();
	
	string LMcommand = "sudo python /home/pi/Documents/UrsaMachinae/Adafruit-Motor-HAT-Python-Library/examples/StepperClockwise.py";
	
	system(LMcommand.c_str());
	
	sleep(1);
	closeClaw();
	sleep(1);
	openClaw();
	sleep(1);
	
	system(LMcommand.c_str());
	

	return 0;
}


int openClaw(){
	if(RobotPosition.getClawPos() == 1){
		printf("Open Claw\n");
		gpioServo(SERVOPIN,MAX_SERVO);
		RobotPosition.switchClawPos();
	}
	return RobotPosition.getClawPos();
}

int closeClaw(){
	if(RobotPosition.getClawPos() == 0){
		printf("Close Claw\n");
		gpioServo(SERVOPIN,MIN_SERVO);
		RobotPosition.switchClawPos();
	}
	return RobotPosition.getClawPos();
}
