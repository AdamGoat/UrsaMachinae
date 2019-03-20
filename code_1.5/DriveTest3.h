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

#include "/home/pi/Documents/UrsaMachinae/code_1.5/Adafruit_MotorHAT.h"
#include <pigpiod_if2.h>
#include <pigpio.h>
#include  "RED.h"

#include <wiringSerial.h>

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
};

class position{
	public:
	pos curPos;
	position(){
		curPos.NS = 0;
		curPos.EW = 0;
		curPos.ang = 0;
		curPos.claw = 0;
		curPos.lift = 1;
		curPos.facing = 0;
		curPos.camera = 0;
		curPos.twist = 0;
		curPos.loadZone = 'A';
		curPos.dumpZone = 'A';
		curPos.arduino = 0;
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
pid_t liftClaw();
pid_t lowerClaw();
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


//position RobotPosition;
