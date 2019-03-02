#include "DriveTest1.h"

#define FORWARDSPEED 	 110
#define STRAFESPEED  	 250
#define PIVOTSPEED		 100
#define UP		 	 BACKWARD
#define DOWN		 FORWARD
#define LIFTSPEED	 250
#define LIFTTIME	 2650000
#define LOWERTIME	 1300000
#define PIVOTTIME	 3500000
#define PICTURETIME	 80000
#define ARDUINOPIN	 17   
#define SERVOPIN	 18 		//Dosn't change servo pin, just for record keeping
#define MIN_SERVO	 600
#define	MAX_SERVO	 1500
#define FORWARDTIME1 2600000
#define FORWARDTIME2 870000
#define FORWARDTIME3 6600000
#define ENCODERPINA	 5
#define ENCODERPINB  12
#define NSFACING	 0
#define EWFACING	 1
#define PIVOTTICKS	 675000
#define TICKSPERFOOT 	 2000000


using namespace std;

Adafruit_MotorHAT  hat (0x61,1600,-1,-1);
Adafruit_MotorHAT  hat2(0x60,1600,-1,-1);

Adafruit_DCMotor& frontLeft  = hat.getDC(1);
Adafruit_DCMotor& frontRight = hat.getDC(2);
Adafruit_DCMotor& backLeft   = hat.getDC(3);
Adafruit_DCMotor& backRight  = hat.getDC(4);

Adafruit_DCMotor& liftMotor  = hat2.getDC(1);

position robotPos;

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

void encoderCallback(int pos){
	if(robotPos.curPos.facing % 2 == 0){
		robotPos.moveNS(pos);
		cout << "NS: " << robotPos.curPos.NS << endl;
	}else{
		robotPos.moveEW(pos);
		cout << "EW: " << robotPos.curPos.EW << endl;
	}
}

int main(){
	
	signal(2, ctrl_c_handler);
	
	if (gpioInitialise() < 0) return -1;
	
	int pi;
	RED_t *renc;
	RED_CB_t cb;
	cb = encoderCallback;
	
	
	pi = pigpio_start(NULL,NULL); /* Connect to Pi. */
	
	if (pi >= 0)
	{
		renc = RED(pi, ENCODERPINA, ENCODERPINB, RED_MODE_DETENT, cb);
	}
	
	
	double startX = 4.0;
	double startY = 4.0;
	char pause;
	
	double endX,endY;
	
	cout << "X: ";
	cin >> endX;
	cout << "Y: ";
	cin >> endY;
	cout << "start(" << startX << "," << startY << ")-";
	cout << "end(" << endX << "," << endY << ")" << endl;
	
	double ticksX = (endX - startX) * TICKSPERFOOT;
	double ticksY = (endY - startY) * TICKSPERFOOT;
	
	cout << "ticksX = " << ticksX << endl;
	cout << "ticksY = " << ticksY << endl;
	
	cout << "Y to Continue" << endl;
	cin >> pause;
	
	/*if(ticksY < 0){
		cout << "Go South" << endl;
		pivotRight(2);
		ticksY = -ticksY;
	}
	goForward(ticksY,NSFACING);
	
	sleep(1);
	cout << "Facing = " << robotPos.curPos.facing << endl;
	if(ticksX < 0){
		while(robotPos.curPos.facing != 3){
			cout << "Turn Right - West" << endl;
			pivotRight(1);
		}
		ticksX = -ticksX;
	} else {
		while(robotPos.curPos.facing != 1){
			cout << "Turn Right - East" << endl;
			pivotRight(1);
		}
	}
	goForward(ticksX,EWFACING);
	halt();*/
		
	
	
	/*goForward(10000+153+11, NSFACING);
	sleep(2);
	pivotLeft(2);
	sleep(2);*/
	/*pivotRight(1);
	sleep(5);
	pivotRight(2);
	sleep(5);
	pivotRight(1);*/

	goForward(27750*3*2000,NSFACING);

	return 0;
}

int goForward(int distance,int facing){ //facing = 0 NS, 1 EW
	printf("Drive Forward!\n");
	//All motors forward
	frontLeft.run(FORWARD);	
	frontRight.run(FORWARD);
	backLeft.run(FORWARD);
	backRight.run(FORWARD);
	int startPos,stopPos;
	if (facing == 0){
		startPos = robotPos.curPos.NS;
	} else {
		startPos = robotPos.curPos.EW;
	}
	stopPos = startPos + distance;
	//All motors max speed
	frontLeft.setSpeed(FORWARDSPEED);
	//frontRight.setSpeed(FORWARDSPEED);
	//backLeft.setSpeed(FORWARDSPEED);
	//backRight.setSpeed(FORWARDSPEED);
	
	cout << "Start = " << startPos << endl;
	cout << "Stop = " << stopPos << endl;
	if (facing == 0){
		while(stopPos > robotPos.curPos.NS);
			//usleep(10);
	} else {
		cout << "East West" <<endl;
		while(stopPos > robotPos.curPos.EW);
			//usleep(10);
	}
	halt();
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << robotPos.curPos.NS << endl;
	cout << "Current EW = " << robotPos.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	cout << "End Forward" << endl;
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

int pivotLeft(int positions){
	printf("Pivot Left!\n");
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
	
	int startPos,stopPos;
	startPos = robotPos.curPos.NS;
	stopPos = startPos + (PIVOTTICKS*positions);
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << robotPos.curPos.NS << endl;
	cout << "Current EW = " << robotPos.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	while(stopPos > robotPos.curPos.NS){
			usleep(10);
			//cout << "Stop = " << stopPos << endl;
	}
	//run for appropriate amount of time
	//usleep(PIVOTTIME*positions);
	//stop
	halt();
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << robotPos.curPos.NS << endl;
	cout << "Current EW = " << robotPos.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	robotPos.curPos.NS = startPos;
	for (int i = 0; i < positions; i++)
	{
		robotPos.changeFacingCCW();
	}
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
	
	int startPos,stopPos;
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << robotPos.curPos.NS << endl;
	cout << "Current EW = " << robotPos.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	if(robotPos.curPos.facing % 2 == 0){
		startPos = robotPos.curPos.NS;
		stopPos = startPos + (PIVOTTICKS*positions);
		while(stopPos > robotPos.curPos.NS){
				usleep(10);
				//cout << "Stop = " << stopPos << endl;
		}
	} else {
		startPos = robotPos.curPos.EW;
		stopPos = startPos + (PIVOTTICKS*positions);
		while(stopPos > robotPos.curPos.EW){
				usleep(10);
				//cout << "Stop = " << stopPos << endl;
		}
	}

	
	//run for appropriate amount of time
	//usleep(PIVOTTIME*positions);
	//stop
	halt();
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << robotPos.curPos.NS << endl;
	cout << "Current EW = " << robotPos.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	robotPos.curPos.NS = startPos;
	for (int i = 0; i < positions; i++)
	{
		robotPos.changeFacingCW();
	}
	cout << "End Pivot Right" << endl;
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
