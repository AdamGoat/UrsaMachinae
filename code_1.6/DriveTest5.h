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

#define FORWARDSPEED 	 	250
#define STRAFESPEED  	 	250
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
#define	MAX_SERVO	 		2100
#define CAMERAUP	 		1600
#define CAMERADRIVE			1150
#define CAMERADOWN	 		900
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

void serialTrash(int fd);
int findBlock(int fd);
void cameraDrive();

char getBlock();
void findBlockInSquare();
void punchersUp();
void punchersDown();

void clockwiseSixth();
void counterClockwiseSixth();

//position RobotPosition;
