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
#define MIN_SERVO	 500
#define	MAX_SERVO	 2500

using namespace std;

Adafruit_MotorHAT hat2(0x60,1600,-1,-1);

Adafruit_DCMotor& liftMotor  = hat2.getDC(1);

void ctrl_c_handler(int s){
	cout << "Caught signal " << s << endl;
	liftMotor.run(RELEASE);
	haltClaw();
	hat2.resetAll();
	exit(1);
}


int main(int argc, char* argv[]){
	signal(2, ctrl_c_handler);
	if (gpioInitialise() < 0) return -1;
	
	cout << "Hello World" << endl;
	cout << "Opening" << endl;
	if(RobotPosition.getClawPos() == 0)
		RobotPosition.switchClawPos();
	cout << "Old pos is " << RobotPosition.getClawPos() << endl;
	openClaw();
	usleep(1000000);

	return 0;
}

pid_t liftClaw(){
	pid_t pid;
	printf("Lift Claw!\n");
	if(RobotPosition.getLiftPos() == 0){
		printf("Lifting\n");
		liftMotor.run(UP);
		liftMotor.setSpeed(LIFTSPEED);
		RobotPosition.switchLiftPos();
		pid = fork();
		if (pid == 0){
			usleep(LIFTTIME);
			//haltClaw();
			exit(0);
		}
	} else {
		printf("Claw already up!\n");
		return 1;
	}		
	return pid;
}

pid_t lowerClaw(){
	pid_t pid;
	printf("Lower Claw\n");
	if(RobotPosition.getLiftPos() == 1){
		printf("Lowering\n");
		liftMotor.run(DOWN);
		liftMotor.setSpeed(LIFTSPEED);
		RobotPosition.switchLiftPos();
		pid = fork();
		if (pid == 0){
			usleep(LOWERTIME);
			//haltClaw();
			exit(0);
		}
	} else {
		printf("Claw already down!\n");
		return 1;
	}
	return pid;
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

int haltClaw(){
	printf("HALT CLAW!\n");
	liftMotor.setSpeed(0);
	return 0;
}

