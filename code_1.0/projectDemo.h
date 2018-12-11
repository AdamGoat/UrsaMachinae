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

#include "Adafruit_MotorHAT.h"
#include <pigpiod_if2.h>
#include <pigpio.h>

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
		curPos.claw = 1;
		curPos.lift = 1;
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
void ctrl_c_handler(int s);
//Called when gpio 18 changes level
void blockDetected(int gpio, int level, uint32_t tick);
void* takePic(void* args);

position RobotPosition;
