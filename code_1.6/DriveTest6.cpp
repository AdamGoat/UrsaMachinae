#include "DriveTest6.h"


//#define FORWARDSPEED 	 	250
//#define STRAFESPEED  	 	250
//#define PIVOTSPEED		 	150
//#define UP		 	 		FORWARD
//#define DOWN		 		BACKWARD
//#define LIFTSPEED	 		250
//#define LIFTTIME	 		6950000
//#define LOWERTIME	 		6900000
//#define PIVOTTIME	 		3500000
//#define TWISTSERVOPIN		18 
//#define GRIPPERSERVOPIN 	17
//#define CAMERASERVOPIN		24
//#define	LIFTTOPSWITCH		21
//#define LIFTBOTTOMSWITCH	19
//#define MIN_SERVO	 		700
//#define	MAX_SERVO	 		2050
//#define CAMERAUP	 		1600
//#define CAMERADOWN	 		900
//#define TWISTIN 	 		554
//#define TWISTOUT 	 		2250
//#define NSFACING	 		0
//#define EWFACING	 		1
//#define PIVOTTICKS	 		1090
//#define TICKSPERFOOT 		1160
//#define	DOUGETICKS	 		TICKSPERFOOT/2

//#define LEFTPUNCHERPIN		27
//#define RIGHTPUNCHERPIN		22
//#define LEFTPUNCHERUP		600
//#define LEFTPUNCHERDOWN		2200
//#define RIGHTPUNCHERUP		2200
//#define RIGHTPUNCHERDOWN	600

//#define ARDUINORESETPIN		4

////TODO Define edges to [-4,3]
//#define BOARDEDGENORTH		8*TICKSPERFOOT
//#define BOARDEDGESOUTH		0
//#define BOARDEDGEEAST		8*TICKSPERFOOT 
//#define BOARDEDGEWEST		0

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

	readJson();
	
	openClaw();
	//sleep(2);
	//cameraUp();
	//closeClaw();
	//sleep(2);
	liftClaw();
	twistIn();
	punchersDown();
	//twistOut();
	cameraDown();
	waitForGreen();

	cameraDrive();
	for (int i = 0; i < numBlocks-0; i++){
		activateObjectDetect(fdJevois);
		pair<double,double> result;
		result = chooseDest(locals);
		cout << "Next block @ (" << curBlockX << "," << curBlockY << ")" << endl;
		cameraDrive();
		lookForBlock(curBlockX*TICKSPERFOOT,curBlockY*TICKSPERFOOT);
		halt();
		findBlockInSquare();
		cout << "Block " << i << " found" <<endl;
		char letter = getBlock();
		cout << "Block is " << letter << endl;
		rotateToLo ad(letter);
		liftClaw();
		twistIn();
		sleep(1);

		openClaw();
		sleep(2);
	}
	cout << "ALL BLOCKS FOUND!" << endl;
	halt();
	
	/*if(motherX > 10){
		lookForMothership();
	}*/	
	//lookForBlock(motherX*TICKSPERFOOT,motherY*TICKSPERFOOT);
	//turnToFace(motherFacing);
	//boardMothership('C');
	examineMothership();
	
	/*cout << "Dump Blocks" << endl;
		twistIn();
		sleep(1);

		openClaw();*/
		//sleep(2);
	//}
	//cout << "ALL BLOCKS FOUND!" << endl;
	//halt();
	
	/*cout << "Dump Blocks" << endl;
	rotateToLoad('E');
	system("python StepperTwelfthCCW.py");
	for (int i = 0; i < 3; i++){
		punchersUp();
		sleep(1);
		punchersDown();
		usleep(500000);
		system("python StepperCCW.py");
	}*/
		
	endProgram();
	return 0;
}

