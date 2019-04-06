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

	readJson();
	
	openClaw();
	//sleep(2);
	//cameraUp();
	//closeClaw();
	//sleep(2);
	liftClaw();
	twistIn();
	punchersDown();
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
		rotateToLoad(letter);
		liftClaw();
		twistIn();
		sleep(1);

		openClaw();
		sleep(2);
	}
	cout << "ALL BLOCKS FOUND!" << endl;
	halt();
	
	if(motherX > 10){
		//Due to the fact that the mothership is a piece of shit we aren't gonna find that motherfucker
		//Peace out 
		cout << "Not trying for MS" << endl;
		endProgram();
		return 0;
	}
	lookForBlock(motherX*TICKSPERFOOT,motherY*TICKSPERFOOT);
	turnToFace(motherFacing);
	examineMothership();
		
	endProgram();
	return 0;
}

