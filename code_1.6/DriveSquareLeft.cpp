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
	goForward(3*TICKSPERFOOT,0);
	pivotLeft(1);
	goForward(3*TICKSPERFOOT,1);
	pivotLeft(1);
	goForward(3*TICKSPERFOOT,0);
	pivotLeft(1);
	goForward(3*TICKSPERFOOT,1);
	return(0);
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
				//TODO disable for pivot
/*				if(serial_data_available(pi,fdJevois) > 0 && jevoisMode == JEVOISMODEOBJ && !turning){
					cout << camGetLine(fdJevois) << endl;
					evasiveManeuvers(curDestX,curDestY);				
				}
					
*/			}
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
/*				if(serial_data_available(pi,fdJevois) > 0 && jevoisMode == JEVOISMODEOBJ && !turning){
					cout << camGetLine(fdJevois) << endl;
					evasiveManeuvers(curDestX,curDestY);				
				}
					
*/			}
		}
		RobotPosition.curPos.arduino = num;
		cout << "End Check Encoder :" << num << endl; 
		return num;
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
