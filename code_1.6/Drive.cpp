#include "DriveTest5.h"
#include "cameraControl.h"

using namespace std;

//This stuff is still in DriveTest5
Adafruit_MotorHAT  hat (0x61,1600,-1,-1);
Adafruit_MotorHAT  hat2(0x60,1600,-1,-1);

Adafruit_DCMotor& frontLeft  = hat.getDC(1);
Adafruit_DCMotor& frontRight = hat.getDC(2);
Adafruit_DCMotor& backLeft   = hat.getDC(3);
Adafruit_DCMotor& backRight  = hat.getDC(4);

position RobotPosition;
int fdArduino;
int curBlockX;
int curBlockY;
int curDestX;
int curDestY;
int numBlocks;
bool turning;

void ctrl_c_handler(int s){
	cout << "Caught signal " << s << endl;
	frontLeft.run(RELEASE);	
	frontRight.run(RELEASE);
	backLeft.run(RELEASE);
	backRight.run(RELEASE);
	
	halt();
	
	
	serialClose(fdArduino);
	
	hat.resetAll();
	hat2.resetAll();
	//gpioTerminate();
	
	exit(1);
}

int main(){

	signal(2, ctrl_c_handler);
	initilizeArduinoSerial();
	double startX = 0;
	double startY = 0;
	
	RobotPosition.curPos.NS = startY*TICKSPERFOOT;
	RobotPosition.curPos.EW	= startX*TICKSPERFOOT;

	pair<double,double> locals[3];
	pair<double,double> blk1;
	pair<double,double> blk2;
	pair<double,double> blk3;
	
	blk1.first = 2;
	blk1.second = 2;
	blk2.first = -3;
	blk2.second = 1;
	blk3.first = -2;
	blk3.second = -4;

	locals[0] = blk1;
	locals[1] = blk2;
	locals[2] = blk3;

	numBlocks = 3;
	
	for (int i = 0; i < numBlocks-0; i++){
		//activateObjectDetect(fdJevois);
		pair<double,double> result;
		result = chooseDest(locals);
		cout << "Next block @ (" << curBlockX << "," << curBlockY << ")" << endl;
		lookForBlock(curBlockX*TICKSPERFOOT,curBlockY*TICKSPERFOOT);
		halt();
		cout << "Block " << i << " found" <<endl;
		//char letter = getBlock();
		//cout << "Block is " << letter << endl;
		//rotateToLoad(letter);
		//liftClaw();
		//twistIn();
		sleep(1);

		//openClaw();
		sleep(2);
	}
	cout << "ALL BLOCKS FOUND!" << endl;
	
	endProgram();
	
	return 0;	
}

int checkEncoder(int stop){
		char ch = '0';
		char numString[10] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
		int num;
		int n = 0;
		cout << "Start Check Encoder: " << RobotPosition.curPos.arduino << endl; 
		if(RobotPosition.curPos.arduino < stop){
			num = -100000;
			cout << "arduino < stop" << endl;
			while(num < stop){
				while(ch != '\n'){
					ch = (char)serialGetchar(fdArduino);
					numString[n] = ch;
					n++;
					//cout << ch;
				}
				numString[n] = '\0';
				n--;
				num = atoi(numString);
				//cout << num << endl;
				n = 0;
				ch = '0';
				////TODO disable for pivot
				//if(serial_data_available(pi,fdJevois) > 0 && jevoisMode == JEVOISMODEOBJ && !turning){
					//cout << camGetLine(fdJevois) << endl;
					//evasiveManeuvers(curDestX,curDestY);				
				//}
					
			}
               }else{
			num = 100000;
			cout << "arduino > stop" << endl;
			while(num > stop){
				while(ch != '\n'){
					ch = (char)serialGetchar(fdArduino);
					numString[n] = ch;
					n++;
					//cout << ch;
				}
				numString[n] = '\0';
				n--;
				num = atoi(numString);
				//cout << num << endl;
				n = 0;
				ch = '0';
				//TODO disable for pivot
				//if(serial_data_available(pi,fdJevois) > 0 && jevoisMode == JEVOISMODEOBJ && !turning){
					//cout << camGetLine(fdJevois) << endl;
					//evasiveManeuvers(curDestX,curDestY);				
				//}
					
			}
		}
		RobotPosition.curPos.arduino = num;
		cout << "End Check Encoder :" << num << endl; 
		return num;
}

int goForward(int distance,int facing){ //facing = 0 NS, 1 EW
	cout << "Drive Forward: " << distance/TICKSPERFOOT << endl;
	turning = false;
	//All motors forward
	frontLeft.run(FORWARD);	
	frontRight.run(FORWARD);
	backLeft.run(FORWARD);
	backRight.run(FORWARD);
	
	int startPos,stopPos,initVal;
	startPos = RobotPosition.curPos.arduino;
	
	frontLeft.setSpeed(50);
	frontRight.setSpeed(50);
	backLeft.setSpeed(50);
	backRight.setSpeed(50);

	usleep(40000);
	
	if (facing == 0){
		initVal = RobotPosition.curPos.NS;
	} else {
		initVal = RobotPosition.curPos.EW;
	}
	
	//distance = abs(distance);
	stopPos = startPos + abs(distance);
	
	//All motors max speed
	frontLeft.setSpeed(FORWARDSPEED+5);
	frontRight.setSpeed(FORWARDSPEED-9);
	backLeft.setSpeed(FORWARDSPEED+5);
	backRight.setSpeed(FORWARDSPEED-9);
	
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << RobotPosition.curPos.NS << endl;
	cout << "Current EW = " << RobotPosition.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	
	if (facing == 0){
		cout << "North South" << endl;
		RobotPosition.curPos.NS += (distance/abs(distance))* (checkEncoder(stopPos)- startPos);
	} else {
		cout << "East West" <<endl;
		RobotPosition.curPos.EW += (distance/abs(distance))* (checkEncoder(stopPos)- startPos);
	}
	//halt();
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << RobotPosition.curPos.NS << endl;
	cout << "Current EW = " << RobotPosition.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	cout << "Actual Stop = " << RobotPosition.curPos.arduino << endl;
	cout << "(" << RobotPosition.curPos.EW/TICKSPERFOOT << "," << RobotPosition.curPos.NS/TICKSPERFOOT << ")" << endl;
	halt();
	usleep(1000000);
	cout << "End Forward" << endl;
	return 0;
}

void strafe(int distance){
	cout << "Strafing" << endl;
	switch (RobotPosition.getFacing()){
		case 0:
			if(distance <= 0)
				strafeLeft(distance);
			else
				strafeRight(distance);
			break;
		case 1:
			if(distance <= 0)
				strafeRight(distance);
			else
				strafeLeft(distance);
			break;
		case 2:
			if(distance <= 0)
				strafeRight(distance);
			else
				strafeLeft(distance);
			break;
		case 3:
			if(distance <= 0)
				strafeLeft(distance);
			else
				strafeRight(distance);
			break;
		default:
			cout << "ERROR: Invalid Facing!" << endl;
			return;
		}
	cout << "End Strafing" << endl;
	return;
}

int strafeLeft(int distance){
	printf("Strafe Left!\n");
	//Motor directions for left
	frontLeft.run(BACKWARD);
	frontRight.run(FORWARD);
	backLeft.run(FORWARD);
	backRight.run(BACKWARD);
	
	turning = true;
	
	int startPos,stopPos;
	if (RobotPosition.getFacing()%2 == 0){ //Fliped for strafing
		cout << "Moving NS" << endl;
		startPos = RobotPosition.curPos.NS;
	} else {
		cout << "Moving EW" << endl;
		startPos = RobotPosition.curPos.EW;
	}
	startPos += RobotPosition.curPos.arduino;
	
	stopPos = startPos + (distance);
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << RobotPosition.curPos.NS << endl;
	cout << "Current EW = " << RobotPosition.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	
	//Go at strafing speed
	frontLeft.setSpeed(STRAFESPEED);
	frontRight.setSpeed(STRAFESPEED);
	backLeft.setSpeed(STRAFESPEED);
	backRight.setSpeed(STRAFESPEED);
	
	if (RobotPosition.getFacing()%2 == 0){
		cout << "North South" << endl;
		RobotPosition.curPos.NS += checkEncoder(stopPos) - startPos;
	} else {
		cout << "East West" <<endl;
		RobotPosition.curPos.EW += checkEncoder(stopPos) - startPos;
	}
	
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << RobotPosition.curPos.NS << endl;
	cout << "Current EW = " << RobotPosition.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	cout << "End Forward" << endl;
	
	return 0;
}

int strafeRight(int distance){
	printf("Strafe Right!\n");
	//Motor direction for right
	frontLeft.run(FORWARD);
	frontRight.run(BACKWARD);
	backLeft.run(BACKWARD);
	backRight.run(FORWARD);
	
	turning = true;
	
	int startPos,stopPos;
	/*if (RobotPosition.getFacing()%2 == 0){ //Fliped for strafing
		cout << "Moving NS" << endl;
		startPos = RobotPosition.curPos.NS;
	} else {
		cout << "Moving EW" << endl;
		startPos = RobotPosition.curPos.EW;
	}*/
	startPos = RobotPosition.curPos.arduino;
	
	stopPos = startPos + (distance) + 50;
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << RobotPosition.curPos.NS << endl;
	cout << "Current EW = " << RobotPosition.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	
	//Go at strafing speed
	frontLeft.setSpeed(STRAFESPEED);
	frontRight.setSpeed(STRAFESPEED);
	backLeft.setSpeed(STRAFESPEED);
	backRight.setSpeed(STRAFESPEED);
	
	if (RobotPosition.getFacing()%2 == 0){
		cout << "North South" << endl;
		RobotPosition.curPos.NS += checkEncoder(stopPos) - startPos;
	} else {
		cout << "East West" <<endl;
		RobotPosition.curPos.EW += checkEncoder(stopPos) - startPos;
	}
	
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << RobotPosition.curPos.NS << endl;
	cout << "Current EW = " << RobotPosition.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	cout << "End Forward" << endl;
	
	return 0;
}

int pivotLeft(int positions){
	printf("Pivot Left!\n");
	//Motor direction for pivot left
	frontLeft.run(BACKWARD);
	frontRight.run(FORWARD);
	backLeft.run(BACKWARD);
	backRight.run(FORWARD);
	//go at pivot speed
	frontLeft.setSpeed(PIVOTSPEED);
	frontRight.setSpeed(PIVOTSPEED);
	backLeft.setSpeed(PIVOTSPEED);
	backRight.setSpeed(PIVOTSPEED);
	
	turning = true;
	
	int startPos,stopPos;
	//startPos = RobotPosition.curPos.NS;
	startPos = RobotPosition.curPos.arduino;
	stopPos = startPos - (PIVOTTICKS*positions);
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << RobotPosition.curPos.NS << endl;
	cout << "Current EW = " << RobotPosition.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	checkEncoder(stopPos);
	/*while(stopPos > robotPosition.curPos.NS){
			usleep(10);
			//cout << "Stop = " << stopPos << endl;
	}*/
	//run for appropriate amount of time
	//usleep(PIVOTTIME*positions);
	//stop
	halt();
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << RobotPosition.curPos.NS << endl;
	cout << "Current EW = " << RobotPosition.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	//robotPosition.curPos.NS = startPos;
	for (int i = 0; i < positions; i++)
	{
		RobotPosition.changeFacingCCW();
	}
	return 0;
}

int pivotRight(int positions){
	printf("Pivot Right\n");
	//Motor directions for pivot right
	frontLeft.run(FORWARD);
	frontRight.run(BACKWARD);
	backLeft.run(FORWARD);
	backRight.run(BACKWARD);
	
	turning = true;

	int startPos,stopPos;
	startPos = RobotPosition.curPos.arduino;
	stopPos = startPos + (PIVOTTICKS*positions);
	
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << RobotPosition.curPos.NS << endl;
	cout << "Current EW = " << RobotPosition.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	
	//go at pivot speed
	frontLeft.setSpeed(PIVOTSPEED);
	frontRight.setSpeed(PIVOTSPEED);
	backLeft.setSpeed(PIVOTSPEED);
	backRight.setSpeed(PIVOTSPEED);
	
	checkEncoder(stopPos);

	halt();
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << RobotPosition.curPos.NS << endl;
	cout << "Current EW = " << RobotPosition.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	
	for (int i = 0; i < positions; i++)
	{
		RobotPosition.changeFacingCW();
	}
	cout << "End Pivot Right" << endl;
	return 0;
}

void evasiveManeuvers(int Xdest, int Ydest){
	cout << "Execute Evasive Maneuvers!" << endl;
	
	halt();
	
	int Ydisp = curBlockY - RobotPosition.curPos.NS;
	int Xdisp = curBlockX - RobotPosition.curPos.EW;
	
	cout << "Protocol: ";
	switch(RobotPosition.getFacing()){
		case 0: //Facing North currently
			cout << 'N';
			if (Ydisp > 0){
				if (Xdisp > 0){
					cout << 1 << endl;
					turnToFace(1);
				} else {
					cout << 2 << endl;
					turnToFace(3);
				}
			}else{
				cout << 3 << endl;
				turnToFace(2);
			}
			break;
		case 1: // Facing East currently
			cout << 'E';
			if(Xdisp > 0){
				if(Ydisp>0){
					cout << 1 << endl;
					turnToFace(0);
				} else {
					cout << 2 << endl;
					turnToFace(2);
				}
			} else {
				cout << 3 << endl;
				turnToFace(3);
			}
			break;
		case 2: // Facing South currently
			cout << 'S';
			if(Ydisp<0){
				if(Xdisp>0){
					cout << 1 << endl;
					turnToFace(1);
				}else{
					cout << 2 << endl;
					turnToFace(3);
				}
			}else{
				cout << 3 << endl;
				turnToFace(0);
			}
			break;
		case 3: // Facing West currently
			cout << 'W';
			if(Xdisp<0){
				if(Ydisp>0){
					cout << 1 << endl;
					turnToFace(0);
				}else{
					cout << 2 << endl;
					turnToFace(2);
				}
			}else{
				cout << 3 << endl;
				turnToFace(1);
			}
			break;
		default:
			cout << "Invalid Facing!" << endl;
			return;
		}
		
	
	cout << "Begin new path" << endl;
	lookForBlock(curBlockX,curBlockY);
	cout << "End evasive maneuvers" << endl;
	
		
	return;
}



void findBlockInSquare(){
	//activateBlockDetect(fdJevois);
	int diff = 0;
	do{
		if (diff < 0){
			strafeLeft(-diff*10);
		} else {
			strafeRight(diff*10);
		}
		//diff = findBlock(fdJevois);
	}while(abs(diff) > 5);
	return;
}

// Dillon 3/18
int turnToFace(int direction){
	// Check that direction value is {0,1,2,3} N E S W
	if(direction>3 || direction<0)
	{
		cout << "Invalid direction";
		return -1;
	}
	// Pivot robot
	if(RobotPosition.curPos.facing != direction)
	{
		if(abs(RobotPosition.curPos.facing - direction)==2)
			pivotRight(2);
		else if((RobotPosition.curPos.facing+1)%4 == direction)
			pivotRight(1);
		else
			pivotLeft(1);
	}
	return 0;
}

// Dillon 3/18 and 3/19 and probably other dates too
int goToPointNS(int Xdest, int Ydest){
	curDestX = Xdest;
	curDestY = Ydest;
	// NS
	if(Ydest != RobotPosition.curPos.NS)
	{
		turnToFace(2*(Ydest<RobotPosition.curPos.NS));
		goForward(Ydest-RobotPosition.curPos.NS, 0);
	}
	// EW
	if(Xdest != RobotPosition.curPos.EW)
	{
		turnToFace(1+2*(Xdest<RobotPosition.curPos.EW));
		goForward(Xdest-RobotPosition.curPos.EW, 1);
	}
	return 0;
}

int goToPointEW(int Xdest, int Ydest){
	curDestX = Xdest;
	curDestY = Ydest;
	// EW
	if(Xdest != RobotPosition.curPos.EW)
	{
		turnToFace(1+2*(Xdest<RobotPosition.curPos.EW));
		goForward(Xdest-RobotPosition.curPos.EW, 1);
	}
	
	// NS
	if(Ydest != RobotPosition.curPos.NS)
	{
		turnToFace(2*(Ydest<RobotPosition.curPos.NS));
		goForward(Ydest-RobotPosition.curPos.NS, 0);
	}
	return 0;
}

// 3/20
pair<double,double> chooseDest(pair<double,double> coordinates[]){
	double goDist=100000000;
	double dist;
	int index=0;
	for(int i=0;i<numBlocks;i++)
	{
		dist=sqrt(pow((coordinates[i].first*TICKSPERFOOT-RobotPosition.curPos.EW),2) + pow((coordinates[i].second*TICKSPERFOOT-RobotPosition.curPos.NS),2));
		cout << "Point (" << coordinates[i].first << "," << coordinates[i].second << ")";
		cout << " dist = " << dist << endl;
		if(dist<goDist)
		{
			cout << "New min dist: " << dist << " i = " << i << endl;
			goDist = dist;
			index = i;
		}
	}
	pair<double,double> sendCoordinates = coordinates[index];
	curBlockX = coordinates[index].first;
	curBlockY = coordinates[index].second;
	coordinates[index].first = 100;
	coordinates[index].second = 100;
	return sendCoordinates;
}



int halt(){
	printf("STOP! In the name of love\n");
	//stop all motors
	frontLeft.setSpeed(0);
	frontRight.setSpeed(0);
	backLeft.setSpeed(0);
	backRight.setSpeed(0);
	//liftMotor.setSpeed(0);
	return 0;
}

void endProgram(){
	halt();
	hat.resetAll();
	hat2.resetAll();
	cout << "End of Program" << endl;
	return;
}

int initilizeArduinoSerial(){
	//int fdArduino = -1; //DO NOT USE!!!!! -Adam & Morgan
	cout << "Open Arduino" << endl;
	fdArduino = serialOpen("/dev/ttyACM0", 57600);
	cout << "fdArduino = " << fdArduino << endl;
	if (fdArduino < 0) {
		cout << "ERROR!!! Can't talk to Arduino!" << endl;
		exit(2);
	}
	char ch;
	while(ch != '\n'){
		ch = (char)serialGetchar(fdArduino);
		cout << ch;
	}
	cout << "Arduino Open" << endl;
	return fdArduino;
}


// Dillon 3/19
int lookForBlock(int blockX, int blockY){
	//activateObjectDetect(fdJevois);
	int Ydisp = blockY - RobotPosition.curPos.NS;
	int Xdisp = blockX - RobotPosition.curPos.EW;
	cout << "Xdisp = " << Xdisp << endl;
	cout << "Ydisp = " << Ydisp << endl;
	cout << "Look for Block Case: ";
	switch(RobotPosition.curPos.facing)
	{
		// facing North to begin
		case 0: // block is north
			cout << "North: ";
				if(Xdisp>0 && Ydisp>0)
				{
					cout << 1 << endl;;
					goToPointNS(blockX-1*TICKSPERFOOT, blockY);
					turnToFace(1);
				}
				else if(Xdisp<0 && Ydisp>0)
				{
					cout << 2 << endl;
					goToPointNS(blockX+1*TICKSPERFOOT, blockY);
					turnToFace(3);
				}
				else if(Xdisp==0 && Ydisp>0)
				{
					cout << 3 << endl;
					goToPointNS(blockX, blockY-1*TICKSPERFOOT);
				}
				// block is south
				else if(Xdisp>0 && Ydisp<0)
				{
					cout << 4 << endl;
					goToPointEW(blockX, blockY+1*TICKSPERFOOT);
					turnToFace(2);
				}
				else if(Xdisp<0 && Ydisp<0)
				{
					cout << 5 << endl;
					goToPointEW(blockX, blockY+1*TICKSPERFOOT);
					turnToFace(2);
				}
				else if(Xdisp==0 && Ydisp<0)
				{
					cout << 6 << endl;
					goToPointEW(blockX, blockY+1*TICKSPERFOOT);
				}
				// block is directly east or west
				else if(Xdisp>0 && Ydisp==0)
				{
					cout << 7 << endl;
					goToPointEW(blockX-1*TICKSPERFOOT, blockY);
				}	
				else if(Xdisp<0 && Ydisp==0)
				{
					cout << 8 << endl;
					goToPointEW(blockX+1*TICKSPERFOOT, blockY);
				}	
				else
					cout << "I'm facing North, but I don't know where to go" << endl;
				break;
				
		// facing East to begin		
		case 1: // block is north
			cout << "East ";
				if(Xdisp>0 && Ydisp>0)
				{
					cout << 9 << endl;
					goToPointEW(blockX, blockY-1*TICKSPERFOOT);
					turnToFace(0);
				}
				else if(Xdisp<0 && Ydisp>0)
				{
					cout << 10 << endl;
					goToPointNS(blockX, blockY+1*TICKSPERFOOT);
					turnToFace(3);
				}
				else if(Xdisp==0 && Ydisp>0)
				{
					cout << 11 << endl;
					goToPointNS(blockX, blockY-1*TICKSPERFOOT);
				}
				// block is south
				else if(Xdisp>0 && Ydisp<0)
				{
					cout << 12 << endl;
					goToPointEW(blockX, blockY+1*TICKSPERFOOT);
					turnToFace(2);
				}
				else if(Xdisp<0 && Ydisp<0)
				{
					cout << 13 << endl;
					goToPointNS(blockX+1*TICKSPERFOOT, blockY);
					turnToFace(3);
				}
				else if(Xdisp==0 && Ydisp<0)
				{
					cout << 14 << endl;
					goToPointEW(blockX, blockY+1*TICKSPERFOOT);
				}
				// block is directly east or west
				else if(Xdisp>0 && Ydisp==0)
				{
					cout << 15 << endl;
					goToPointEW(blockX-1*TICKSPERFOOT, blockY);
				}	
				else if(Xdisp<0 && Ydisp==0)
				{
					cout << 16 << endl;
					goToPointEW(blockX+1*TICKSPERFOOT, blockY);
				}	
				else
					cout << "I'm facing East, but I don't know where to go" << endl;
				break;
				
		// facing South to begin
		case 2: // block is north
			cout << "South: ";
				if(Xdisp>0 && Ydisp>0)
				{
					cout << 17 << endl;
					goToPointEW(blockX, blockY-1*TICKSPERFOOT);
					turnToFace(0);
				}
				else if(Xdisp<0 && Ydisp>0)
				{
					cout << 18 << endl;
					goToPointNS(blockX, blockY-1*TICKSPERFOOT);
					turnToFace(0);
				}
				else if(Xdisp==0 && Ydisp>0)
				{
					cout << 19 << endl;
					goToPointNS(blockX, blockY-1*TICKSPERFOOT);
				}
				// block is south
				else if(Xdisp>0 && Ydisp<0)
				{
					cout << 20 << endl;
					goToPointNS(blockX-1*TICKSPERFOOT, blockY);
					turnToFace(1);
				}
				else if(Xdisp<0 && Ydisp<0)
				{
					cout << 21 << endl;
					goToPointNS(blockX+1*TICKSPERFOOT, blockY);
					turnToFace(3);
				}
				else if(Xdisp==0 && Ydisp<0)
				{
					cout << 22 << endl;
					goToPointEW(blockX, blockY+1*TICKSPERFOOT);
				}
				// block is directly east or west
				else if(Xdisp>0 && Ydisp==0)
				{
					cout << 23 << endl;
					goToPointEW(blockX-1*TICKSPERFOOT, blockY);
				}	
				else if(Xdisp<0 && Ydisp==0)
				{
					cout << 24 << endl;
					goToPointEW(blockX+1*TICKSPERFOOT, blockY);
				}	
				else
					cout << "I'm facing South, but I don't know where to go" << endl;
				break;
				
		// facing West to begin
		case 3: // block is north
			cout << "West: ";
				if(Xdisp>0 && Ydisp>0)
				{
					cout << 25 << endl;
					goToPointEW(blockX-1*TICKSPERFOOT, blockY);
					turnToFace(1);
				}
				else if(Xdisp<0 && Ydisp>0)
				{
					cout << 26 << endl;
					goToPointEW(blockX, blockY-1*TICKSPERFOOT);
					turnToFace(0);
				}
				else if(Xdisp==0 && Ydisp>0)
				{
					cout << 27 << endl;
					goToPointNS(blockX, blockY-1*TICKSPERFOOT);
				}
				// block is south
				else if(Xdisp>0 && Ydisp<0)
				{
					cout << 28 << endl;
					goToPointNS(blockX-1*TICKSPERFOOT, blockY);
					turnToFace(1);
				}
				else if(Xdisp<0 && Ydisp<0)
				{
					cout << 29 << endl;
					goToPointEW(blockX, blockY+1*TICKSPERFOOT);
					turnToFace(2);
				}
				else if(Xdisp==0 && Ydisp<0)
				{
					cout << 30 << endl;
					goToPointEW(blockX, blockY+1*TICKSPERFOOT);
				}
				// block is directly east or west
				else if(Xdisp>0 && Ydisp==0)
				{
					cout << 31 << endl;
					goToPointEW(blockX-1*TICKSPERFOOT, blockY);
				}	
				else if(Xdisp<0 && Ydisp==0)
				{
					cout << 32 << endl;
					goToPointEW(blockX+1*TICKSPERFOOT, blockY);
				}	
				else
					cout << "I'm facing West, but I don't know where to go" << endl;
				break;
		default:
			cout << "Invalid Facing!" << endl;
			return -1;
	}
	return 0;
}
