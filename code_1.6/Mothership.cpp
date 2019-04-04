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

	goForward(1.5*TICKSPERFOOT);
	punchersDown();
	usleep(500000);
	servoOff(LEFTPUNCHERPIN);
	servoOff(RIGHTPUNCHERPIN);
	rotateToLoad('A');
	system("python StepperTwelfthCW.py");
	//CD
	punchersUp();
	usleep(1000000);
	punchersDown();
	goForward(.25*TICKSPERFOOT);
	counterClockwiseSixth();
	//EB
	punchersUp();
	usleep(1000000);
	punchersDown();
	goForward(.25*TICKSPERFOOT);
	//stepperCCWExtra();
	system("python StepperCCWExtra.py");
	//AF
	punchersUp();
	usleep(500000);
	punchersDown();

	cameraDrive();
}


