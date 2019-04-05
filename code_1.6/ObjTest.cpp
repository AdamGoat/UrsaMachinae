#include "DriveTest6.h"

using namespace std;


int main(){
	signal(2, ctrl_c_handler);
	
	initilizePigpiod();	
	initilizeArduinoSerial();
	initilizeJevoisSerial();	
	
	double startX = 0;
	double startY = 0;
	char pause;
	
	RobotPosition.curPos.NS = startY*TICKSPERFOOT;
	RobotPosition.curPos.EW	= startX*TICKSPERFOOT;

	openClaw();
	//sleep(2);
	//cameraUp();
	//closeClaw();
	//sleep(2);
	//liftClaw();
	twistIn();
	//twistOut();
	cameraDown();
	waitForGreen();

	cameraDrive();
	activateObjectDetect(fdJevois);
	while(true){
		cout << camGetLine(fdJevois);
	}	
	endProgram();
	return 0;
}

