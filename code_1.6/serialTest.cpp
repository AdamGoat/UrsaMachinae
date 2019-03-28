#include "DriveTest6.h"

using namespace std;

int main(){
	signal(2, ctrl_c_handler);
	
	double startX = 0;
	double startY = 0;
	
	RobotPosition.curPos.NS = startY*TICKSPERFOOT;
	RobotPosition.curPos.EW	= startX*TICKSPERFOOT;
	
	initilizePigpiod();	
	initilizeArduinoSerial();
	initilizeJevoisSerial();	
	
	closeClaw();
	liftClaw();
	twistIn();
	
	cameraDrive();
	
	findBlockInSquare();
	
	getBlock();
	
	liftClaw();
	twistIn();
	sleep(2);
	openClaw();
	
	endProgram();
	return 0;
}
