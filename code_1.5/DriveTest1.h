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

#include "/home/pi/Documents/UrsaMachinae/code_1.5/Adafruit_MotorHAT.h"
#include <pigpiod_if2.h>
#include <pigpio.h>
#include  "RED.h"

//enum Directions {N, E, S, W};

struct pos{
	double NS;
	double EW;
	//Directions facing;
	int facing;
	double ang;
	int claw;
	int lift;
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

position RobotPosition;
