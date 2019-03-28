#include "stdio.h"
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <string>
#include <math.h>

#include "/home/pi/Documents/UrsaMachinae/code_1.6/Adafruit_MotorHAT.h"
#include <pigpiod_if2.h>
#include <pigpio.h>

#include <wiringSerial.h>

#include "cameraControl.h"

#define FORWARDSPEED 	 	250
#define STRAFESPEED  	 	150
#define PIVOTSPEED		 	150
#define UP		 	 		FORWARD
#define DOWN		 		BACKWARD
#define LIFTSPEED	 		250
#define LIFTTIME	 		6950000
#define LOWERTIME	 		6900000
#define PIVOTTIME	 		3500000
#define TWISTSERVOPIN		18 
#define GRIPPERSERVOPIN 	17
#define CAMERASERVOPIN		24
#define	LIFTTOPSWITCH		21
#define LIFTBOTTOMSWITCH	19
#define MIN_SERVO	 		700
#define	MAX_SERVO	 		2150
#define CAMERAUP	 		1600
#define CAMERADRIVE			1150
#define CAMERADOWN	 		950
#define TWISTIN 	 		554
#define TWISTOUT 	 		2250
#define NSFACING	 		0
#define EWFACING	 		1
#define PIVOTTICKS	 		1125
#define TICKSPERFOOT 		1143
#define	DOUGETICKS	 		TICKSPERFOOT/2

#define LEFTPUNCHERPIN		27
#define RIGHTPUNCHERPIN		22
#define LEFTPUNCHERUP		600
#define LEFTPUNCHERDOWN		2200
#define RIGHTPUNCHERUP		2200
#define RIGHTPUNCHERDOWN	600

#define ARDUINORESETPIN		4

//TODO Define edges to [-4,3]
#define BOARDEDGENORTH		8*TICKSPERFOOT
#define BOARDEDGESOUTH		0
#define BOARDEDGEEAST		8*TICKSPERFOOT 
#define BOARDEDGEWEST		0

//enum Directions {N, E, S, W};

using namespace std;



struct pos{
	double NS;
	double EW;
	//Directions facing;
	int facing;
	double ang;
	int claw;
	int lift;
	int camera;
	int twist;
	char loadZone;
	char dumpZone;
	int arduino;
	int puncher;
	int stepperOffset;
};

class position{
	public:
	pos curPos;
	position(){
		curPos.NS = 0;
		curPos.EW = 0;
		curPos.ang = 0;
		curPos.claw = 1;
		curPos.lift = 0;
		curPos.facing = 0;
		curPos.camera = 1;
		curPos.twist = 1;
		curPos.loadZone = 'A';
		curPos.dumpZone = 'A';
		curPos.arduino = 000;
		curPos.puncher = 0;
		curPos.stepperOffset = 0;
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
	pos changeFacingCW(){
		++(curPos.facing) %= 4;
		return curPos;
	}
	pos changeFacingCCW(){
		curPos.facing--;
		if (curPos.facing < 0)
			curPos.facing = 3;
		return curPos;
	}
	int getFacing(){
		return curPos.facing;
	}
	int getPuncher(){
		return curPos.puncher;
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
	int getCameraPos(){
		// 0 = down
		// 1 = up
		// 2 = drive
		return curPos.camera;
	}
	int switchCameraPos(){
		curPos.camera++;
		curPos.camera %= 2;
		return curPos.camera;
	}
	int getTwistPos(){
		// 0 = in
		// 1 = out
		return curPos.twist;
	}
	int switchTwistPos(){
		curPos.twist++;
		curPos.twist %= 2;
		return curPos.twist;
	}
	int switchPuncherPos(){
		curPos.puncher++;
		curPos.puncher %= 2;
		return curPos.puncher;
	}
	char getLoadZone(){
		//A = A 
		//B = B
		//C = C
		//D = F
		//E = E
		//F = D
		return curPos.loadZone;
	}
	char getDumpZone(){
		//A = AF
		//B = BE
		//C = CD
		return curPos.dumpZone;
	}
	char incLoadZone(){
		curPos.loadZone++;
		cout << "Inc load to " << curPos.loadZone << endl;
		if (curPos.loadZone > 'F'){
			cout << "IncLoad wrap " << curPos.loadZone << endl;
			curPos.loadZone = 'A';
		}
		return curPos.loadZone;
	}
	char decLoadZone(){
		curPos.loadZone--;
		if (curPos.loadZone < 'A')
			curPos.loadZone ='F';
		return curPos.loadZone;
	}
	int getArduinoValue(){
		return curPos.arduino;
	}
};

int goForward(int distance,int facing);
int strafeLeft(int distance);
int strafeRight(int distance);
int pivotLeft(int positions);
int pivotRight(int positions);
void liftClaw();
void lowerClaw();
int openClaw();
int closeClaw();
int haltClaw();
int halt();
void ctrl_c_handler(int s);
int checkEncoder(int stop);
int cameraUp();
int cameraDown();
int twistIn();
int twistOut();
char rotateToLoad(char load);
void switchToDump();
char rotateToDump(char dump);
int turnToFace(int direction);
int goToPointNS(int Xdest, int Ydest);
int goToPointEW(int Xdest, int Ydest);
pair<double,double> chooseDest(pair<double,double> coordinates[]);
void evasiveManeuvers(int Xdest, int Ydest);
void strafe(int distance);
int lookForBlock(int blockX, int blockY);
int initilizeArduinoSerial();
int initilizeJevoisSerial();
int initilizePigpiod();
void endProgram();

//void serialTrash(int fd);
int findBlock(int fd);
void cameraDrive();

char getBlock();
void findBlockInSquare();
void punchersUp();
void punchersDown();

void clockwiseSixth();
void counterClockwiseSixth();

Adafruit_MotorHAT  hat (0x61,1600,-1,-1);
Adafruit_MotorHAT  hat2(0x60,1600,-1,-1);

Adafruit_DCMotor& frontLeft  = hat.getDC(1);
Adafruit_DCMotor& frontRight = hat.getDC(2);
Adafruit_DCMotor& backLeft   = hat.getDC(3);
Adafruit_DCMotor& backRight  = hat.getDC(4);

Adafruit_DCMotor& liftMotor  = hat2.getDC(1);

position RobotPosition;
int fdArduino;
int fdJevois;
int pi;
int curBlockX;
int curBlockY;
int curDestX;
int curDestY;
int numBlocks;
bool turning;

int checkEncoder(int stop){
		char ch = '0';
		char numString[10] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
		int num;
		int n = 0;
		cout << "Start Check Encoder: " << RobotPosition.curPos.arduino << endl; 
		if(RobotPosition.curPos.arduino < stop){
			num = -100000;
			cout << "arduino < stop" << endl;
			while(num < stop){
				while(ch != '\n'){
					ch = (char)serialGetchar(fdArduino);
					numString[n] = ch;
					n++;
					//cout << ch;
				}
				numString[n] = '\0';
				n--;
				num = atoi(numString);
				//cout << num << endl;
				n = 0;
				ch = '0';
				//TODO disable for pivot
				if(serial_data_available(pi,fdJevois) > 0 && jevoisMode == JEVOISMODEOBJ && !turning){
					cout << camGetLine(fdJevois) << endl;
					evasiveManeuvers(curDestX,curDestY);				
				}
					
			}
               }else{
			num = 100000;
			cout << "arduino > stop" << endl;
			while(num > stop){
				while(ch != '\n'){
					ch = (char)serialGetchar(fdArduino);
					numString[n] = ch;
					n++;
					//cout << ch;
				}
				numString[n] = '\0';
				n--;
				num = atoi(numString);
				//cout << num << endl;
				n = 0;
				ch = '0';
				//TODO disable for pivot
				if(serial_data_available(pi,fdJevois) > 0 && jevoisMode == JEVOISMODEOBJ && !turning){
					cout << camGetLine(fdJevois) << endl;
					evasiveManeuvers(curDestX,curDestY);				
				}
					
			}
		}
		RobotPosition.curPos.arduino = num;
		cout << "End Check Encoder :" << num << endl; 
		return num;
}

int goForward(int distance,int facing){ //facing = 0 NS, 1 EW
	cout << "Drive Forward: " << distance/TICKSPERFOOT << endl;
	turning = false;
	//All motors forward
	frontLeft.run(FORWARD);	
	frontRight.run(FORWARD);
	backLeft.run(FORWARD);
	backRight.run(FORWARD);
	
	int startPos,stopPos,initVal;
	startPos = RobotPosition.curPos.arduino;
	
	frontLeft.setSpeed(50);
	frontRight.setSpeed(50);
	backLeft.setSpeed(50);
	backRight.setSpeed(50);

	usleep(40000);
	
	if (facing == 0){
		initVal = RobotPosition.curPos.NS;
	} else {
		initVal = RobotPosition.curPos.EW;
	}
	
	//distance = abs(distance);
	stopPos = startPos + abs(distance);
	
	//All motors max speed
	//frontLeft.setSpeed(FORWARDSPEED+5);
	//frontRight.setSpeed(FORWARDSPEED-7);
	//backLeft.setSpeed(FORWARDSPEED+5);
	//backRight.setSpeed(FORWARDSPEED-7);
	frontLeft.setSpeed(FORWARDSPEED+4);
	frontRight.setSpeed(FORWARDSPEED-2.1);
	backLeft.setSpeed(FORWARDSPEED-5);
	backRight.setSpeed(FORWARDSPEED-2.1);

	
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << RobotPosition.curPos.NS << endl;
	cout << "Current EW = " << RobotPosition.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	
	if (facing == 0){
		cout << "North South" << endl;
		RobotPosition.curPos.NS += (distance/abs(distance))* (checkEncoder(stopPos)- startPos);
	} else {
		cout << "East West" <<endl;
		RobotPosition.curPos.EW += (distance/abs(distance))* (checkEncoder(stopPos)- startPos);
	}
	//halt();
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << RobotPosition.curPos.NS << endl;
	cout << "Current EW = " << RobotPosition.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	cout << "Actual Stop = " << RobotPosition.curPos.arduino << endl;
	cout << "(" << RobotPosition.curPos.EW/TICKSPERFOOT << "," << RobotPosition.curPos.NS/TICKSPERFOOT << ")" << endl;
	halt();
	usleep(1000000);
	cout << "End Forward" << endl;
	return 0;
}

void strafe(int distance){
	cout << "Strafing" << endl;
	switch (RobotPosition.getFacing()){
		case 0:
			if(distance <= 0)
				strafeLeft(distance);
			else
				strafeRight(distance);
			break;
		case 1:
			if(distance <= 0)
				strafeRight(distance);
			else
				strafeLeft(distance);
			break;
		case 2:
			if(distance <= 0)
				strafeRight(distance);
			else
				strafeLeft(distance);
			break;
		case 3:
			if(distance <= 0)
				strafeLeft(distance);
			else
				strafeRight(distance);
			break;
		default:
			cout << "ERROR: Invalid Facing!" << endl;
			return;
		}
	cout << "End Strafing" << endl;
	return;
}

int strafeLeft(int distance){
	printf("Strafe Left!\n");
	//Motor directions for left
	frontLeft.run(BACKWARD);
	frontRight.run(FORWARD);
	backLeft.run(FORWARD);
	backRight.run(BACKWARD);
	
	turning = true;
	
	int startPos,stopPos;
	if (RobotPosition.getFacing()%2 == 0){ //Fliped for strafing
		cout << "Moving NS" << endl;
		startPos = RobotPosition.curPos.NS;
	} else {
		cout << "Moving EW" << endl;
		startPos = RobotPosition.curPos.EW;
	}
	startPos += RobotPosition.curPos.arduino;
	
	stopPos = startPos + (distance);
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << RobotPosition.curPos.NS << endl;
	cout << "Current EW = " << RobotPosition.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	
	//Go at strafing speed
	frontLeft.setSpeed(STRAFESPEED);
	frontRight.setSpeed(STRAFESPEED);
	backLeft.setSpeed(STRAFESPEED);
	backRight.setSpeed(STRAFESPEED);
	
	if (RobotPosition.getFacing()%2 == 0){
		cout << "North South" << endl;
		RobotPosition.curPos.NS += checkEncoder(stopPos) - startPos;
	} else {
		cout << "East West" <<endl;
		RobotPosition.curPos.EW += checkEncoder(stopPos) - startPos;
	}
	
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << RobotPosition.curPos.NS << endl;
	cout << "Current EW = " << RobotPosition.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	cout << "End Forward" << endl;
	
	return 0;
}

int strafeRight(int distance){
	printf("Strafe Right!\n");
	//Motor direction for right
	frontLeft.run(FORWARD);
	frontRight.run(BACKWARD);
	backLeft.run(BACKWARD);
	backRight.run(FORWARD);
	
	turning = true;
	
	int startPos,stopPos;
	/*if (RobotPosition.getFacing()%2 == 0){ //Fliped for strafing
		cout << "Moving NS" << endl;
		startPos = RobotPosition.curPos.NS;
	} else {
		cout << "Moving EW" << endl;
		startPos = RobotPosition.curPos.EW;
	}*/
	startPos = RobotPosition.curPos.arduino;
	
	stopPos = startPos + (distance) /*+ 50*/;
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << RobotPosition.curPos.NS << endl;
	cout << "Current EW = " << RobotPosition.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	
	//Go at strafing speed
	frontLeft.setSpeed(STRAFESPEED);
	frontRight.setSpeed(STRAFESPEED);
	backLeft.setSpeed(STRAFESPEED);
	backRight.setSpeed(STRAFESPEED);
	
	if (RobotPosition.getFacing()%2 == 0){
		cout << "North South" << endl;
		RobotPosition.curPos.NS += checkEncoder(stopPos) - startPos;
	} else {
		cout << "East West" <<endl;
		RobotPosition.curPos.EW += checkEncoder(stopPos) - startPos;
	}
	
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << RobotPosition.curPos.NS << endl;
	cout << "Current EW = " << RobotPosition.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	cout << "End Forward" << endl;
	
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
	
	turning = true;
	
	int startPos,stopPos;
	//startPos = RobotPosition.curPos.NS;
	startPos = RobotPosition.curPos.arduino;
	stopPos = startPos - (PIVOTTICKS*positions);
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << RobotPosition.curPos.NS << endl;
	cout << "Current EW = " << RobotPosition.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	checkEncoder(stopPos);
	/*while(stopPos > robotPosition.curPos.NS){
			usleep(10);
			//cout << "Stop = " << stopPos << endl;
	}*/
	//run for appropriate amount of time
	//usleep(PIVOTTIME*positions);
	//stop
	halt();
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << RobotPosition.curPos.NS << endl;
	cout << "Current EW = " << RobotPosition.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	//robotPosition.curPos.NS = startPos;
	for (int i = 0; i < positions; i++)
	{
		RobotPosition.changeFacingCCW();
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
	
	turning = true;

	int startPos,stopPos;
	startPos = RobotPosition.curPos.arduino;
	stopPos = startPos + (PIVOTTICKS*positions);
	
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << RobotPosition.curPos.NS << endl;
	cout << "Current EW = " << RobotPosition.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	
	//go at pivot speed
	frontLeft.setSpeed(PIVOTSPEED);
	frontRight.setSpeed(PIVOTSPEED);
	backLeft.setSpeed(PIVOTSPEED);
	backRight.setSpeed(PIVOTSPEED);
	
	checkEncoder(stopPos);

	halt();
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << RobotPosition.curPos.NS << endl;
	cout << "Current EW = " << RobotPosition.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	
	for (int i = 0; i < positions; i++)
	{
		RobotPosition.changeFacingCW();
	}
	cout << "End Pivot Right" << endl;
	return 0;
}

void evasiveManeuvers(int Xdest, int Ydest){
	cout << "Execute Evasive Maneuvers!" << endl;
	
	halt();
	
	int Ydisp = curBlockY - RobotPosition.curPos.NS;
	int Xdisp = curBlockX - RobotPosition.curPos.EW;
	
	cout << "Protocol: ";
	switch(RobotPosition.getFacing()){
		case 0: //Facing North currently
			cout << 'N';
			if (Ydisp > 0){
				if (Xdisp > 0){
					cout << 1 << endl;
					turnToFace(1);
				} else {
					cout << 2 << endl;
					turnToFace(3);
				}
			}else{
				cout << 3 << endl;
				turnToFace(2);
			}
			break;
		case 1: // Facing East currently
			cout << 'E';
			if(Xdisp > 0){
				if(Ydisp>0){
					cout << 1 << endl;
					turnToFace(0);
				} else {
					cout << 2 << endl;
					turnToFace(2);
				}
			} else {
				cout << 3 << endl;
				turnToFace(3);
			}
			break;
		case 2: // Facing South currently
			cout << 'S';
			if(Ydisp<0){
				if(Xdisp>0){
					cout << 1 << endl;
					turnToFace(1);
				}else{
					cout << 2 << endl;
					turnToFace(3);
				}
			}else{
				cout << 3 << endl;
				turnToFace(0);
			}
			break;
		case 3: // Facing West currently
			cout << 'W';
			if(Xdisp<0){
				if(Ydisp>0){
					cout << 1 << endl;
					turnToFace(0);
				}else{
					cout << 2 << endl;
					turnToFace(2);
				}
			}else{
				cout << 3 << endl;
				turnToFace(1);
			}
			break;
		default:
			cout << "Invalid Facing!" << endl;
			return;
		}
		
	
	cout << "Begin new path" << endl;
	lookForBlock(curBlockX,curBlockY);
	cout << "End evasive maneuvers" << endl;
	
		
	return;
}

char getBlock(){
	cout<< "I will now pick up the block" << endl;
	//findBlockInSquare();
	if (RobotPosition.getFacing()>1){
		goForward(-.9*TICKSPERFOOT,(RobotPosition.getFacing()%2));
	} else {
		goForward(.9*TICKSPERFOOT,(RobotPosition.getFacing()%2));
	}

	halt();
	cameraDown();
	//activateOCR(fdJevois);
	//char letter = readBlock(fdJevois);
	char letter;
	cin >> letter;
	cameraUp();
	twistOut();
	openClaw();
	lowerClaw();
	closeClaw();
	sleep(1);
		cout << "Block " << letter << " is mine!" << endl;
	return letter;
}

int findBlock(int fd){
	//serialTrash(fd);
	//fd = initilizeJevoisSerial();
	activateBlockDetect(fd);
	string num = camGetLine(fd);
	cout << "Block at " << num << endl;
	camStreamOff(fd);
	//serial_close(pi,fd);
	int dist = atoi(num.c_str());
	return dist;
}

void findBlockInSquare(){
	//activateBlockDetect(fdJevois);
	//serial_close(pi,fdJevois);
	int diff = 0;
	do{
		if (diff < 0){
			cout << "Strafe Left to Block " << diff*1 << endl;
			strafeLeft((int)(-diff*1));
		} else if ( diff > 0) {
			cout << "Strafe Right to Block " << -diff*1 << endl;
			strafeRight((int)(-diff*1));
		} 
		halt();
		diff = findBlock(fdJevois);
		//serialTrash(fdJevois);
	}while(abs(diff) > 10);
	return;
}

void punchersDown(){
	if(RobotPosition.getPuncher() == 1){
		printf("Punchers Down\n");
		set_servo_pulsewidth(pi,LEFTPUNCHERPIN,LEFTPUNCHERDOWN);
		set_servo_pulsewidth(pi,RIGHTPUNCHERPIN,RIGHTPUNCHERDOWN);
		RobotPosition.switchPuncherPos();
	}	
	return;
}

void punchersUp(){
	if(RobotPosition.getPuncher() == 0){
		printf("Punchers Up\n");
		set_servo_pulsewidth(pi,LEFTPUNCHERPIN,LEFTPUNCHERUP);
		set_servo_pulsewidth(pi,RIGHTPUNCHERPIN,RIGHTPUNCHERUP);
		RobotPosition.switchPuncherPos();
	}	
	return;
}

// Dillon 3/18
int turnToFace(int direction){
	// Check that direction value is {0,1,2,3} N E S W
	if(direction>3 || direction<0)
	{
		cout << "Invalid direction";
		return -1;
	}
	// Pivot robot
	if(RobotPosition.curPos.facing != direction)
	{
		if(abs(RobotPosition.curPos.facing - direction)==2)
			pivotRight(2);
		else if((RobotPosition.curPos.facing+1)%4 == direction)
			pivotRight(1);
		else
			pivotLeft(1);
	}
	return 0;
}

// Dillon 3/18 and 3/19 and probably other dates too
int goToPointNS(int Xdest, int Ydest){
	curDestX = Xdest;
	curDestY = Ydest;
	// NS
	if(Ydest != RobotPosition.curPos.NS)
	{
		turnToFace(2*(Ydest<RobotPosition.curPos.NS));
		goForward(Ydest-RobotPosition.curPos.NS, 0);
	}
	// EW
	if(Xdest != RobotPosition.curPos.EW)
	{
		turnToFace(1+2*(Xdest<RobotPosition.curPos.EW));
		goForward(Xdest-RobotPosition.curPos.EW, 1);
	}
	return 0;
}

int goToPointEW(int Xdest, int Ydest){
	curDestX = Xdest;
	curDestY = Ydest;
	// EW
	if(Xdest != RobotPosition.curPos.EW)
	{
		turnToFace(1+2*(Xdest<RobotPosition.curPos.EW));
		goForward(Xdest-RobotPosition.curPos.EW, 1);
	}
	
	// NS
	if(Ydest != RobotPosition.curPos.NS)
	{
		turnToFace(2*(Ydest<RobotPosition.curPos.NS));
		goForward(Ydest-RobotPosition.curPos.NS, 0);
	}
	return 0;
}

int boardMothership(char letter){
	int stop;
	if(letter == 'C')
	{
		// strafe left
		stop = RobotPosition.curPos.EW + 2000;	//placeholder	
		frontLeft.run(BACKWARD);
		frontRight.run(FORWARD);
		backLeft.run(FORWARD);
		backRight.run(BACKWARD);
		checkEncoder(stop);
		halt();
		
		// drive forward
		stop = RobotPosition.curPos.EW + 2500;	//placeholder	
		frontLeft.run(FORWARD);	
		frontRight.run(FORWARD);
		backLeft.run(FORWARD);
		backRight.run(FORWARD);
		checkEncoder(stop);
		halt();
		
		// face ramp
		pivotRight(1);
	}
	
	else if(letter == 'D')
	{
		// strafe right
		stop = RobotPosition.curPos.EW + 2000;	//placeholder	
		frontLeft.run(FORWARD);
		frontRight.run(BACKWARD);
		backLeft.run(BACKWARD);
		backRight.run(FORWARD);
		checkEncoder(stop);
		halt();
		
		// drive forward
		stop = RobotPosition.curPos.EW + 2500;	//placeholder		
		frontLeft.run(FORWARD);	
		frontRight.run(FORWARD);
		backLeft.run(FORWARD);
		backRight.run(FORWARD);
		checkEncoder(stop);
		halt();
		
		// face ramp
		pivotLeft(1);
	}
	
	// get C and D on the right and left sides of the robot
	rotateToLoad('E');
	system("python StepperTwelfthCCW.py");
	
	// drive up ramp
	stop = RobotPosition.curPos.EW + 3000;		//placeholder	
	frontLeft.run(FORWARD);	
	frontRight.run(FORWARD);
	backLeft.run(FORWARD);
	backRight.run(FORWARD);
	checkEncoder(stop);
	halt();
	
	// dump blocks
	punchersUp();
	sleep(1);
	punchersDown();
	usleep(500000);
	
	system("python StepperCCW.py");
	
	// drive forward to line up with B and E
	stop = RobotPosition.curPos.EW + 1500;		//placeholder	
	frontLeft.run(FORWARD);	
	frontRight.run(FORWARD);
	backLeft.run(FORWARD);
	backRight.run(FORWARD);
	checkEncoder(stop);
	halt();
	
	// dump blocks
	punchersUp();
	sleep(1);
	punchersDown();
	usleep(500000);

	system("python StepperCCW.py");
	
	// drive forward to line up with A and F
	stop = RobotPosition.curPos.EW + 1500;		//placeholder	
	frontLeft.run(FORWARD);	
	frontRight.run(FORWARD);
	backLeft.run(FORWARD);
	backRight.run(FORWARD);
	checkEncoder(stop);
	halt();
	
	// dump blocks
	punchersUp();
	sleep(1);
	punchersDown();
	usleep(500000);
	
	// finishing light
	
	// self destruct
	
	return 0;
}

// Dillon 3/19
int lookForBlock(int blockX, int blockY){
	//activateObjectDetect(fdJevois);
	int Ydisp = blockY - RobotPosition.curPos.NS;
	int Xdisp = blockX - RobotPosition.curPos.EW;
	cout << "Xdisp = " << Xdisp << endl;
	cout << "Ydisp = " << Ydisp << endl;
	cout << "Look for Block Case: ";
	switch(RobotPosition.curPos.facing)
	{
		// facing North to begin
		case 0: // block is north
			cout << "North: ";
				if(Xdisp>0 && Ydisp>0)
				{
					cout << 1 << endl;;
					goToPointNS(blockX-1*TICKSPERFOOT, blockY);
					turnToFace(1);
				}
				else if(Xdisp<0 && Ydisp>0)
				{
					cout << 2 << endl;
					goToPointNS(blockX+1*TICKSPERFOOT, blockY);
					turnToFace(3);
				}
				else if(Xdisp==0 && Ydisp>0)
				{
					cout << 3 << endl;
					goToPointNS(blockX, blockY-1*TICKSPERFOOT);
				}
				// block is south
				else if(Xdisp>0 && Ydisp<0)
				{
					cout << 4 << endl;
					goToPointEW(blockX, blockY+1*TICKSPERFOOT);
					turnToFace(2);
				}
				else if(Xdisp<0 && Ydisp<0)
				{
					cout << 5 << endl;
					goToPointEW(blockX, blockY+1*TICKSPERFOOT);
					turnToFace(2);
				}
				else if(Xdisp==0 && Ydisp<0)
				{
					cout << 6 << endl;
					goToPointEW(blockX, blockY+1*TICKSPERFOOT);
				}
				// block is directly east or west
				else if(Xdisp>0 && Ydisp==0)
				{
					cout << 7 << endl;
					goToPointEW(blockX-1*TICKSPERFOOT, blockY);
				}	
				else if(Xdisp<0 && Ydisp==0)
				{
					cout << 8 << endl;
					goToPointEW(blockX+1*TICKSPERFOOT, blockY);
				}	
				else
					cout << "I'm facing North, but I don't know where to go" << endl;
				break;
				
		// facing East to begin		
		case 1: // block is north
			cout << "East ";
				if(Xdisp>0 && Ydisp>0)
				{
					cout << 9 << endl;
					goToPointEW(blockX, blockY-1*TICKSPERFOOT);
					turnToFace(0);
				}
				else if(Xdisp<0 && Ydisp>0)
				{
					cout << 10 << endl;
					goToPointNS(blockX, blockY+1*TICKSPERFOOT);
					turnToFace(3);
				}
				else if(Xdisp==0 && Ydisp>0)
				{
					cout << 11 << endl;
					goToPointNS(blockX, blockY-1*TICKSPERFOOT);
				}
				// block is south
				else if(Xdisp>0 && Ydisp<0)
				{
					cout << 12 << endl;
					goToPointEW(blockX, blockY+1*TICKSPERFOOT);
					turnToFace(2);
				}
				else if(Xdisp<0 && Ydisp<0)
				{
					cout << 13 << endl;
					goToPointNS(blockX+1*TICKSPERFOOT, blockY);
					turnToFace(3);
				}
				else if(Xdisp==0 && Ydisp<0)
				{
					cout << 14 << endl;
					goToPointEW(blockX, blockY+1*TICKSPERFOOT);
				}
				// block is directly east or west
				else if(Xdisp>0 && Ydisp==0)
				{
					cout << 15 << endl;
					goToPointEW(blockX-1*TICKSPERFOOT, blockY);
				}	
				else if(Xdisp<0 && Ydisp==0)
				{
					cout << 16 << endl;
					goToPointEW(blockX+1*TICKSPERFOOT, blockY);
				}	
				else
					cout << "I'm facing East, but I don't know where to go" << endl;
				break;
				
		// facing South to begin
		case 2: // block is north
			cout << "South: ";
				if(Xdisp>0 && Ydisp>0)
				{
					cout << 17 << endl;
					goToPointEW(blockX, blockY-1*TICKSPERFOOT);
					turnToFace(0);
				}
				else if(Xdisp<0 && Ydisp>0)
				{
					cout << 18 << endl;
					goToPointNS(blockX, blockY-1*TICKSPERFOOT);
					turnToFace(0);
				}
				else if(Xdisp==0 && Ydisp>0)
				{
					cout << 19 << endl;
					goToPointNS(blockX, blockY-1*TICKSPERFOOT);
				}
				// block is south
				else if(Xdisp>0 && Ydisp<0)
				{
					cout << 20 << endl;
					goToPointNS(blockX-1*TICKSPERFOOT, blockY);
					turnToFace(1);
				}
				else if(Xdisp<0 && Ydisp<0)
				{
					cout << 21 << endl;
					goToPointNS(blockX+1*TICKSPERFOOT, blockY);
					turnToFace(3);
				}
				else if(Xdisp==0 && Ydisp<0)
				{
					cout << 22 << endl;
					goToPointEW(blockX, blockY+1*TICKSPERFOOT);
				}
				// block is directly east or west
				else if(Xdisp>0 && Ydisp==0)
				{
					cout << 23 << endl;
					goToPointEW(blockX-1*TICKSPERFOOT, blockY);
				}	
				else if(Xdisp<0 && Ydisp==0)
				{
					cout << 24 << endl;
					goToPointEW(blockX+1*TICKSPERFOOT, blockY);
				}	
				else
					cout << "I'm facing South, but I don't know where to go" << endl;
				break;
				
		// facing West to begin
		case 3: // block is north
			cout << "West: ";
				if(Xdisp>0 && Ydisp>0)
				{
					cout << 25 << endl;
					goToPointEW(blockX-1*TICKSPERFOOT, blockY);
					turnToFace(1);
				}
				else if(Xdisp<0 && Ydisp>0)
				{
					cout << 26 << endl;
					goToPointEW(blockX, blockY-1*TICKSPERFOOT);
					turnToFace(0);
				}
				else if(Xdisp==0 && Ydisp>0)
				{
					cout << 27 << endl;
					goToPointNS(blockX, blockY-1*TICKSPERFOOT);
				}
				// block is south
				else if(Xdisp>0 && Ydisp<0)
				{
					cout << 28 << endl;
					goToPointNS(blockX-1*TICKSPERFOOT, blockY);
					turnToFace(1);
				}
				else if(Xdisp<0 && Ydisp<0)
				{
					cout << 29 << endl;
					goToPointEW(blockX, blockY+1*TICKSPERFOOT);
					turnToFace(2);
				}
				else if(Xdisp==0 && Ydisp<0)
				{
					cout << 30 << endl;
					goToPointEW(blockX, blockY+1*TICKSPERFOOT);
				}
				// block is directly east or west
				else if(Xdisp>0 && Ydisp==0)
				{
					cout << 31 << endl;
					goToPointEW(blockX-1*TICKSPERFOOT, blockY);
				}	
				else if(Xdisp<0 && Ydisp==0)
				{
					cout << 32 << endl;
					goToPointEW(blockX+1*TICKSPERFOOT, blockY);
				}	
				else
					cout << "I'm facing West, but I don't know where to go" << endl;
				break;
		default:
			cout << "Invalid Facing!" << endl;
			return -1;
	}
	return 0;
}

// 3/20
pair<double,double> chooseDest(pair<double,double> coordinates[]){
	double goDist=100000000;
	double dist;
	int index=0;
	for(int i=0;i<numBlocks;i++)
	{
		dist=sqrt(pow((coordinates[i].first*TICKSPERFOOT-RobotPosition.curPos.EW),2) + pow((coordinates[i].second*TICKSPERFOOT-RobotPosition.curPos.NS),2));
		cout << "Point (" << coordinates[i].first << "," << coordinates[i].second << ")";
		cout << " dist = " << dist << endl;
		if(dist<goDist)
		{
			cout << "New min dist: " << dist << " i = " << i << endl;
			goDist = dist;
			index = i;
		}
	}
	pair<double,double> sendCoordinates = coordinates[index];
	curBlockX = coordinates[index].first;
	curBlockY = coordinates[index].second;
	coordinates[index].first = 100;
	coordinates[index].second = 100;
	return sendCoordinates;
}

void liftClaw(){
	printf("Lift Claw!\n");
	//cout << "Lift Top Button: " << gpio_read(pi, LIFTTOPSWITCH) << endl;
	if(/*RobotPosition.getLiftPos() == 0 || */!gpio_read(pi, LIFTTOPSWITCH)){
		printf("Lifting!\n");
		liftMotor.run(UP);
		liftMotor.setSpeed(LIFTSPEED);
		//cout << "Lift Top Button Pre: " << gpio_read(pi, LIFTTOPSWITCH) << endl;
		while(gpio_read(pi, LIFTTOPSWITCH) == 0)
		{
			//cout << "Wait for top button" << endl;
			//cout << "Lift Top Button: " << gpio_read(pi, LIFTTOPSWITCH) << endl;
			usleep(1000);
		}
		//cout << "Lift Top Button Post: " << gpio_read(pi, LIFTTOPSWITCH) << endl;
		RobotPosition.switchLiftPos();
		haltClaw();
	} else {
		printf("Claw already up!\n");
		return;
	}	
	return;	
}

void lowerClaw(){
	printf("Lower Claw!\n");
	//cout << "Lift Bottom Button: " << gpio_read(pi, LIFTBOTTOMSWITCH) << endl;
	if(/*RobotPosition.getLiftPos() == 1 || */!gpio_read(pi, LIFTBOTTOMSWITCH)){
		printf("Lowering\n");
		liftMotor.run(DOWN);
		liftMotor.setSpeed(LIFTSPEED);
		//cout << "Lift Bottom Button Pre: " << gpio_read(pi, LIFTBOTTOMSWITCH) << endl;
		while(!gpio_read(pi, LIFTBOTTOMSWITCH))
		{
			//cout << "Lift Bottom Button: " << gpio_read(pi, LIFTBOTTOMSWITCH) << endl;
			usleep(1000);
		}
		//cout << "Lift Bottom Button Post: " << gpio_read(pi, LIFTBOTTOMSWITCH) << endl;
		RobotPosition.switchLiftPos();
		haltClaw();
	} else {
		printf("Claw already down!\n");
		return;
	}
	return;
}

int openClaw(){
	if(RobotPosition.getClawPos() == 1 || true){
		printf("Open Claw\n");
		//gpioServo(GRIPPERSERVOPIN,MAX_SERVO);
		set_servo_pulsewidth(pi,GRIPPERSERVOPIN,MAX_SERVO);
		RobotPosition.switchClawPos();
	}
	return RobotPosition.getClawPos();
}

int closeClaw(){
	if(RobotPosition.getClawPos() == 0 || true){
		printf("Close Claw\n");
		//gpioServo(GRIPPERSERVOPIN,MIN_SERVO);
		set_servo_pulsewidth(pi,GRIPPERSERVOPIN,MIN_SERVO);
		RobotPosition.switchClawPos();
	}
	return RobotPosition.getClawPos();
}

int cameraUp(){
	if(RobotPosition.getCameraPos() == 0 || true){
		printf("Camera Up\n");
		set_servo_pulsewidth(pi,CAMERASERVOPIN,CAMERAUP);
		RobotPosition.switchCameraPos();
	}
	return RobotPosition.getCameraPos();
}

int cameraDown(){
	if(RobotPosition.getCameraPos() == 1 || true){
		printf("Camera Down\n");
		set_servo_pulsewidth(pi,CAMERASERVOPIN,CAMERADOWN);
		RobotPosition.switchCameraPos();
	}
	return RobotPosition.getCameraPos();
}

void cameraDrive(){
	if(RobotPosition.getCameraPos() == 0 || true){
		printf("Camera Drive\n");
		set_servo_pulsewidth(pi,CAMERASERVOPIN,CAMERADRIVE);
		//RobotPosition.switchCameraPos();
	}
	return;
}

int twistIn(){
	if(/*RobotPosition.getTwistPos() == 1 && */gpio_read(pi, LIFTTOPSWITCH)){
		printf("Twist In\n");
		set_servo_pulsewidth(pi,TWISTSERVOPIN,TWISTIN);
		RobotPosition.switchTwistPos();
		usleep(100000);
	} else {
		cout << "Cant twist in!" << endl;
	}
	return RobotPosition.getTwistPos();
}

int twistOut(){
	cameraUp();
	if(/*RobotPosition.getTwistPos() == 0 && RobotPosition.getLiftPos() == 1 && */gpio_read(pi, LIFTTOPSWITCH)){
		printf("Twist Out\n");
		set_servo_pulsewidth(pi,TWISTSERVOPIN,TWISTOUT);
		RobotPosition.switchTwistPos();
		usleep(100000);
	}
	return RobotPosition.getTwistPos();
}

int haltClaw(){
	printf("HALT CLAW!\n");
	liftMotor.setSpeed(0);
	return 0;
}

void clockwiseSixth(){
	switch(RobotPosition.curPos.stepperOffset)
	{
		case -2:
			system("python StepperClockwiseExtra.py");
			RobotPosition.curPos.stepperOffset = 1;
			break;
			
		case -1:
			system("python StepperClockwise.py");
			RobotPosition.curPos.stepperOffset = 0;
			break;
			
		case 0:
			system("python StepperClockwise.py");
			RobotPosition.curPos.stepperOffset = 1;
			break;
		
		case 1:
			system("python StepperClockwise.py");
			RobotPosition.curPos.stepperOffset = 2;
			break;
			
		case 2:
			system("python StepperClockwiseExtra.py");
			RobotPosition.curPos.stepperOffset = 0;
			break;
	}	
}

void counterClockwiseSixth(){
	switch(RobotPosition.curPos.stepperOffset)
	{
		case -2:
			system("python StepperCCWExtra.py");
			RobotPosition.curPos.stepperOffset = 0;
			break;
			
		case -1:
			system("python StepperCCW.py");
			RobotPosition.curPos.stepperOffset = -2;
			break;
			
		case 0:
			system("python StepperCCW.py");
			RobotPosition.curPos.stepperOffset = -1;
			break;
		
		case 1:
			system("python StepperCCW.py");
			RobotPosition.curPos.stepperOffset = 0;
			break;
			
		case 2:
			system("python StepperCCWExtra.py");
			RobotPosition.curPos.stepperOffset = -1;
			break;
	}	
}
char rotateToLoad(char load){
	char cur = RobotPosition.getLoadZone();
	if (load == 'D'){
		cout << "Changing D to F" << endl;
		load = 'F';
	} else if (load == 'F') {
		load = 'D';
		cout << "Changing F to D" << endl;
	}
	int dist = load - cur;
	if (dist >= 3){
		cout << "shorter CCW" << endl;
		dist -= 6;
	} else if (dist <= -3){
		cout << "shorter CW" << endl;
		dist += 6;
	}
	cout << "Rotating " << dist << " spots" << endl;
	if (dist > 0){
		for(int i = 0; i < dist; i++){
//			system("python StepperClockwise.py");  // Should be taken care of by clockwiseSixth()
			clockwiseSixth();
			cout << RobotPosition.incLoadZone() << endl;
			cout << "Load Zone is now " << RobotPosition.getLoadZone() << endl;
		}
	} else if (dist < 0) {
		for(int i = 0; i > dist; i--){
//			system("python StepperCCW.py");		// Should be taken care of by counterClockwiseSixth()
			counterClockwiseSixth();
			RobotPosition.decLoadZone();
			cout << "Load Zone is now " << RobotPosition.getLoadZone() << endl;
		}
	}
	cout << "Final Load Zone is now " << RobotPosition.getLoadZone() << endl;
	return RobotPosition.getLoadZone();
}



/*void serialTrash(int fd){
	cout << "pi = " << pi << endl;
	int bytes = serial_data_available(pi,fd);
	cout << "dumping " << bytes << " bytes of data from " << fd << endl;
	if (bytes > 0){
		serial_read(pi,fd,NULL,bytes);
	} else {
		cout << PI_BAD_HANDLE << endl;
		cout << pigpio_error(bytes) << endl;
	}
	return;
}*/

int initilizeArduinoSerial(){
	//int fdArduino = -1; //DO NOT USE!!!!! -Adam & Morgan
	cout << "Open Arduino" << endl;
	fdArduino = serialOpen("/dev/ttyACM0", 57600);
	cout << "fdArduino = " << fdArduino << endl;
	if (fdArduino < 0) {
		cout << "ERROR!!! Can't talk to Arduino!" << endl;
		exit(2);
	}
	char ch;
	while(ch != '\n'){
		ch = (char)serialGetchar(fdArduino);
		cout << ch;
	}
	cout << "Arduino Open" << endl;
	return fdArduino;
}

int initilizeJevoisSerial(){
	cout << "Open Jevois" << endl;
	fdJevois = serialOpen("/dev/ttyACM1", 115200);
	//fdJevois = serialOpen("/dev/ttyACM0", 115200);
	//fdJevois = -1;
	
	cout << "fdJevois = " << fdJevois << endl;
	if (fdJevois < 0) {
		cout << "ERROR!!! Can't talk to jevois!" << endl;
		//serialClose(6);
		exit(3);
	}
	cout << "Jevois Open" << endl;
	setCameraSettings(fdJevois);
	streamOn = false;
	return fdJevois;
}

int initilizePigpiod(){
	cout << "Start pigpiod" << endl;
	pi = pigpio_start(NULL,NULL); /* Connect to Pi. */
	cout << "pi = " << pi << endl;
	
	set_mode(pi, LIFTTOPSWITCH, PI_INPUT);
	set_mode(pi, LIFTBOTTOMSWITCH, PI_INPUT);
	set_mode(pi, ARDUINORESETPIN, PI_OUTPUT);
	gpio_write(pi, ARDUINORESETPIN, 1);
	set_pull_up_down(pi, LIFTTOPSWITCH, PI_PUD_DOWN);
	set_pull_up_down(pi, LIFTBOTTOMSWITCH, PI_PUD_DOWN);
	return pi;
}

int halt(){
	printf("STOP! In the name of love\n");
	//stop all motors
	frontLeft.setSpeed(0);
	frontRight.setSpeed(0);
	backLeft.setSpeed(0);
	backRight.setSpeed(0);
	liftMotor.setSpeed(0);
	return 0;
}

void endProgram(){
	rebootCam(fdJevois);
	serialClose(fdArduino);
	serialClose(fdJevois);
	gpio_write(pi, ARDUINORESETPIN, 0);	
	usleep(5);
	gpio_write(pi, ARDUINORESETPIN, 1);
	pigpio_stop(pi);
	cout << "End of Program" << endl;
	return;
}

void ctrl_c_handler(int s){
	cout << "Caught signal " << s << endl;
	frontLeft.run(RELEASE);	
	frontRight.run(RELEASE);
	backLeft.run(RELEASE);
	backRight.run(RELEASE);
	liftMotor.run(RELEASE);
	halt();
	haltClaw();
	camStreamOff(fdJevois);
	rebootCam(fdJevois);
	serialClose(fdArduino);
	serialClose(fdJevois);
	hat.resetAll();
	hat2.resetAll();
	//gpioTerminate();
	gpio_write(pi, ARDUINORESETPIN, 0);
	usleep(5);
	gpio_write(pi, ARDUINORESETPIN, 1);
	pigpio_stop(pi);
	exit(1);
}
