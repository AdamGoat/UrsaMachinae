#include "DriveTest5.h"

using namespace std;

Adafruit_MotorHAT  hat (0x61,1600,-1,-1);
Adafruit_MotorHAT  hat2(0x60,1600,-1,-1);

/*
Adafruit_DCMotor& frontLeft  = hat.getDC(1);
Adafruit_DCMotor& frontRight = hat.getDC(2);
Adafruit_DCMotor& backLeft   = hat.getDC(3);
Adafruit_DCMotor& backRight  = hat.getDC(4);

Adafruit_DCMotor& liftMotor  = hat2.getDC(1);
*/

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
/*	frontLeft.run(RELEASE);	
	frontRight.run(RELEASE);
	backLeft.run(RELEASE);
	backRight.run(RELEASE);
	
	halt();
	
	
	serialClose(fdArduino);
*/	
	hat.resetAll();
	hat2.resetAll();
	//gpioTerminate();
	
	exit(1);
}


int main(){
	char letter;
	cout << "Rotate to letter: " << endl;
	cin >> letter;
	rotateToLoad(letter);

	return 0;
}

void clockwiseSixth(){
	switch(RobotPosition.curPos.stepperOffset)
	{
		case -2:
			system("python StepperClockwiseExtra.py");
			RobotPosition.curPos.stepperOffset = 1;
			break;
			
		case -1:
			system("python StepperClockwise.py");
			RobotPosition.curPos.stepperOffset = 0;
			break;
			
		case 0:
			system("python StepperClockwise.py");
			RobotPosition.curPos.stepperOffset = 1;
			break;
		
		case 1:
			system("python StepperClockwise.py");
			RobotPosition.curPos.stepperOffset = 2;
			break;
			
		case 2:
			system("python StepperClockwiseExtra.py");
			RobotPosition.curPos.stepperOffset = 0;
			break;
	}	
}

void counterClockwiseSixth(){
	switch(RobotPosition.curPos.stepperOffset)
	{
		case -2:
			system("python StepperCCWExtra.py");
			RobotPosition.curPos.stepperOffset = 0;
			break;
			
		case -1:
			system("python StepperCCW.py");
			RobotPosition.curPos.stepperOffset = -2;
			break;
			
		case 0:
			system("python StepperCCW.py");
			RobotPosition.curPos.stepperOffset = -1;
			break;
		
		case 1:
			system("python StepperCCW.py");
			RobotPosition.curPos.stepperOffset = 0;
			break;
			
		case 2:
			system("python StepperCCWExtra.py");
			RobotPosition.curPos.stepperOffset = -1;
			break;
	}	
}

char rotateToLoad(char load){
	char cur = RobotPosition.getLoadZone();
	if (load == 'D'){
		cout << "Changing D to F" << endl;
		load = 'F';
	} else if (load == 'F') {
		load = 'D';
		cout << "Changing F to D" << endl;
	}
	int dist = load - cur;
	if (dist >= 3){
		cout << "shorter CCW" << endl;
		dist -= 6;
	} else if (dist <= -3){
		cout << "shorter CW" << endl;
		dist += 6;
	}
	cout << "Rotating " << dist << " spots" << endl;
	if (dist > 0){
		for(int i = 0; i < dist; i++){
//			system("python StepperClockwise.py");  // Should be taken care of by clockwiseSixth()
			clockwiseSixth();
			cout << RobotPosition.incLoadZone() << endl;
			cout << "Load Zone is now " << RobotPosition.getLoadZone() << endl;
		}
	} else if (dist < 0) {
		for(int i = 0; i > dist; i--){
//			system("python StepperCCW.py");		// Should be taken care of by counterClockwiseSixth()
			counterClockwiseSixth();
			RobotPosition.decLoadZone();
			cout << "Load Zone is now " << RobotPosition.getLoadZone() << endl;
		}
	}
	cout << "Final Load Zone is now " << RobotPosition.getLoadZone() << endl;
	return RobotPosition.getLoadZone();
}
