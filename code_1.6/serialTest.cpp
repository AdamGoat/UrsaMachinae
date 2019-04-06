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
	
	openClaw();
	liftClaw();
	twistIn();
	punchersDown();
	
	cameraDrive();
	
	waitForGreen();
	
	findBlockInSquare();
	//activateOCR(fdJevois);	
	camStreamOff(fdJevois);
	char letter = getBlock();
	
	liftClaw();
	twistIn();
	rotateToLoad(letter);
	sleep(2);
	openClaw();
	
	cameraDrive();
	
	endProgram();
	return 0;
}
