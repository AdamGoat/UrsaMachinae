#include "stdio.h"
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "Adafruit_MotorHAT.h"
#include <pigpiod_if2.h>
#include <pigpio.h>

#define FORWARDSPEED 250
#define STRAFESPEED  250
#define PIVOTSPEED	 250
#define UP			 BACKWARD
#define DOWN		 FORWARD
#define LIFTSPEED	 250
#define LIFTTIME	 1200000
#define LOWERTIME	 1000000
#define ARDUINOPIN	 17
#define SERVOPIN	 18 		//Dosn't change servo pin, just for record keeping
#define MIN_SERVO	 500
#define	MAX_SERVO	 1500

using namespace std;

Adafruit_MotorHAT  hat(0x61, 1600,-1,-1);
Adafruit_MotorHAT hat2(0x60,1600,-1,-1);

Adafruit_DCMotor& frontLeft  = hat.getDC(1);
Adafruit_DCMotor& frontRight = hat.getDC(2);
Adafruit_DCMotor& backLeft   = hat.getDC(3);
Adafruit_DCMotor& backRight  = hat.getDC(4);

Adafruit_DCMotor& liftMotor  = hat2.getDC(1);

struct pos{
	double NS;
	double EW;
	double ang;
	int claw;
	int lift;
};

class position{
	pos curPos;
	public:
	position(){
		curPos.NS = 0;
		curPos.EW = 0;
		curPos.ang = 0;
		curPos.claw = 0;
		curPos.lift = 0;
	}
	pos getCurrentPos(){
		return curPos;
	}
	pos moveNS(double change){
		curPos.NS += change;
		return curPos;
	}
	pos moveEW(double change){
		curPos.EW += change;
		return curPos;
	}
	pos changeAng(double change){
		curPos.ang += change;
		return curPos;
	}
	int getLiftPos(){
		// 0 = down
		// 1 = up
		return curPos.lift;
	}
	int switchLiftPos(){
		curPos.lift++;
		curPos.lift %= 2;
		return curPos.lift;
	}
	int getClawPos(){
		// 0 = open
		// 1 = close
		return curPos.claw;
	}
	int switchClawPos(){
		curPos.claw++;
		curPos.claw %= 2;
		return curPos.claw;
	}
};

int goForward(int distance);
int strafeLeft(int distance);
int strafeRight(int distance);
int pivotLeft(int positions);
int pivotRight(int positions);
pid_t liftClaw();
pid_t lowerClaw();
int openClaw();
int closeClaw();
int haltClaw();
int halt();

void ctrl_c_handler(int s){
	cout << "Caught signal " << s << endl;
	frontLeft.run(RELEASE);	
	frontRight.run(RELEASE);
	backLeft.run(RELEASE);
	backRight.run(RELEASE);
	liftMotor.run(RELEASE);
	halt();
	haltClaw();
	hat.resetAll();
	hat2.resetAll();
	gpioTerminate();
	exit(1);
}

//Called when gpio 18 changes level
void blockDetected(int gpio, int level, uint32_t tick){
	// 1 = rising edge
	if(level == 1){
		cout << "STOP!!!" << endl;
		//stop for 10 seconds
		halt();
		usleep(10000000);
	}
}

position RobotPosition;

int main(int argc, char* argv[]){
	//set Ctrl_C action
	//struct sigaction ctrl_C;
	//ctrl_C.sa_handler = ctrl_c_handler;
	//ctrl_C.sa_flags = 0;
	//sigemptyset(&ctrl_C.sa_mask);
	signal(2, ctrl_c_handler);
	//sigaction(2,&ctrl_C,NULL);
	
	int cycle = 0;
	//int input;
	
	cout << "Hello World" << endl;
	
	//initialise pin 17 for pigpio
	if (gpioInitialise() < 0) return -1;
	gpioSetMode(ARDUINOPIN,PI_INPUT);
	gpioSetPullUpDown(ARDUINOPIN,PI_PUD_DOWN);
	//set alert function to trigger when pin 17 changes level
	gpioSetAlertFunc(ARDUINOPIN,blockDetected);
	
	pid_t clawPID = 0;
	
	
	
	//int fd;
	/*char in[10];
	char buff[1];
	int i = 0;
	buff[0] = '\0';
	fd = open("/dev/ttyACM0", O_RDONLY);*/
	while(true){
		//usleep(100000);
		//cin >> input;
		//getline(cin,in);
				
		/*buff[0] = '\0';
		while(buff[0] != '\n'){
			read(fd,buff,sizeof(char));
			in[i]=buff[0];
			i++;
		}
		in[i-1] = '\0';
		i = 0;
		cout << "in = " <<  in << endl;
		input = atoi(in);

		cout << "input = " << input << endl;
		if(input > 6){*/
		
		//cycle through various drive commands
			switch(cycle % 6){
				case 0:
					printf("Forward!\n");
					goForward(100);
					break;
				case 1:
					printf("Close, Left, Lift\n");
					halt();
					closeClaw();
					usleep(1000000);
					strafeLeft(100);
					clawPID = liftClaw();
					break;
				case 2:
					printf("Right and Down\n");
					strafeRight(100);
					break;
				case 3:
					printf("Open and Turn Left\n");
					openClaw();
					usleep(1000000);
					pivotLeft(1);
					clawPID = lowerClaw();
					break;
				case 4:
					printf("Turn Right\n");
					pivotRight(1);
					break;
				case 5:
					printf("Halt!\n");
					haltClaw();
					halt();
					break;
			}
			if (clawPID != 0){
				printf("Waiting\n");
				waitpid(clawPID,NULL,0);
				haltClaw();
				clawPID = 0;
			}
			usleep(2000000);
			printf("Next\n");
			
			cycle++;
		//}
	}	


	return 0;
}

int goForward(int distance){
	printf("Drive Forward!\n");
	//All motors forward
	frontLeft.run(FORWARD);	
	frontRight.run(FORWARD);
	backLeft.run(FORWARD);
	backRight.run(FORWARD);
	//All motors max speed
	frontLeft.setSpeed(FORWARDSPEED);
	frontRight.setSpeed(FORWARDSPEED);
	backLeft.setSpeed(FORWARDSPEED);
	backRight.setSpeed(FORWARDSPEED);
	return 0;
}
int strafeLeft(int distance){
	printf("Strafe Left!\n");
	//Motor directions for left
	frontLeft.run(BACKWARD);
	frontRight.run(FORWARD);
	backLeft.run(FORWARD);
	backRight.run(BACKWARD);
	//Go at strafing speed
	frontLeft.setSpeed(STRAFESPEED);
	frontRight.setSpeed(STRAFESPEED);
	backLeft.setSpeed(STRAFESPEED);
	backRight.setSpeed(STRAFESPEED);
	return 0;
}

int strafeRight(int distance){
	printf("Strafe Right!\n");
	//Motor direction for right
	frontLeft.run(FORWARD);
	frontRight.run(BACKWARD);
	backLeft.run(BACKWARD);
	backRight.run(FORWARD);
	//Go at strafing speed
	frontLeft.setSpeed(STRAFESPEED);
	frontRight.setSpeed(STRAFESPEED);
	backLeft.setSpeed(STRAFESPEED);
	backRight.setSpeed(STRAFESPEED);
	return 0;
}

int pivotLeft(int positions){
	printf("Pivot Left\n!");
	//Motor direction for pivot left
	frontLeft.run(BACKWARD);
	frontRight.run(FORWARD);
	backLeft.run(BACKWARD);
	backRight.run(FORWARD);
	//go at pivot speed
	frontLeft.setSpeed(PIVOTSPEED);
	frontRight.setSpeed(PIVOTSPEED);
	backLeft.setSpeed(PIVOTSPEED);
	backRight.setSpeed(PIVOTSPEED);
	//run for appropriate amount of time
	usleep(3000000*positions);
	//stop
	halt();
	return 0;
}

int pivotRight(int positions){
	printf("Pivot Right\n");
	//Motor directions for pivot right
	frontLeft.run(FORWARD);
	frontRight.run(BACKWARD);
	backLeft.run(FORWARD);
	backRight.run(BACKWARD);
	//go at pivot speed
	frontLeft.setSpeed(PIVOTSPEED);
	frontRight.setSpeed(PIVOTSPEED);
	backLeft.setSpeed(PIVOTSPEED);
	backRight.setSpeed(PIVOTSPEED);
	//run for appropriate amount of time
	usleep(3000000*positions);
	//stop
	halt();
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
	
	

