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

	pair<double,double> locals[3];
	pair<double,double> blk1;
	pair<double,double> blk2;
	pair<double,double> blk3;
	
	blk1.first = -4;
	blk1.second = 3;
	blk2.first = 3;
	blk2.second = -4;
	blk3.first = 100;
	blk3.second = 100;

	locals[0] = blk1;
	locals[1] = blk2;
	locals[2] = blk3;

	numBlocks = 3;
	//openClaw();
	//sleep(2);
	closeClaw();
	//sleep(2);
	liftClaw();
	twistIn();
	cameraDown();
	/*char l = 'B';
	openClaw();
	sleep(2);	
	while (true){
		twistOut();
		sleep(2);
		lowerClaw();
		sleep(2);
		closeClaw();
		sleep(2);
		liftClaw();
		sleep(2);
		rotateToLoad(l);
		l++;
		sleep(2);
		twistIn();
		sleep(2);
	}*/

	for (int i = 0; i < numBlocks-1; i++){
		//activateObjectDetect(fdJevois);
		pair<double,double> result;
		result = chooseDest(locals);
		cout << "Next block @ (" << curBlockX << "," << curBlockY << ")" << endl;
		lookForBlock(curBlockX*TICKSPERFOOT,curBlockY*TICKSPERFOOT);
		halt();
		cout << "Block " << i << " found" <<endl;
		char letter = getBlock();
		cout << "Block is " << letter << endl;
		rotateToLoad(letter);
		liftClaw();
		twistIn();
		sleep(1);

		openClaw();
		sleep(2);
	}
	cout << "ALL BLOCKS FOUND!" << endl;
	halt();
	
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

