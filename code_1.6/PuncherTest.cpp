#include "DriveTest6.h"

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
int pi;


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
	initilizePigpiod();	
	
	// get C and D on the right and left sides of the robot
	/*rotateToLoad('E');
	system("python StepperTwelfthCCW.py");
	
	sleep(0.5);
	
	*/// dump blocks 1
	punchersUp();
	sleep(3);
	punchersDown();
	usleep(500000);
	/*
	counterClockwiseSixth();
	
	sleep(0.5);
	// dump blocks 2
	punchersUp();
	sleep(3);
	punchersDown();
	usleep(500000);
	
	counterClockwiseSixth();
	
	sleep(0.5);
	// dump blocks 3
	punchersUp();
	sleep(3);
	punchersDown();
	usleep(500000);
*/
	return 0;
}

void punchersDown(){
	if(RobotPosition.getPuncher() == 1){
		printf("Punchers Down\n");
		set_servo_pulsewidth(pi,LEFTPUNCHERPIN,LEFTPUNCHERDOWN);
		set_servo_pulsewidth(pi,RIGHTPUNCHERPIN,RIGHTPUNCHERDOWN);
		RobotPosition.switchPuncherPos();
	}	
	return;
}

void punchersUp(){
	if(RobotPosition.getPuncher() == 0){
		printf("Punchers Up\n");
		set_servo_pulsewidth(pi,LEFTPUNCHERPIN,LEFTPUNCHERUP);
		set_servo_pulsewidth(pi,RIGHTPUNCHERPIN,RIGHTPUNCHERUP);
		RobotPosition.switchPuncherPos();
	}	
	return;
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

int initilizePigpiod(){
	cout << "Start pigpiod" << endl;
	pi = pigpio_start(NULL,NULL); /* Connect to Pi. */
	
/*	set_mode(pi, LIFTTOPSWITCH, PI_INPUT);
	set_mode(pi, LIFTBOTTOMSWITCH, PI_INPUT);
	set_mode(pi, ARDUINORESETPIN, PI_OUTPUT);
	gpio_write(pi, ARDUINORESETPIN, 1);
	set_pull_up_down(pi, LIFTTOPSWITCH, PI_PUD_DOWN);
	set_pull_up_down(pi, LIFTBOTTOMSWITCH, PI_PUD_DOWN);
*/	return pi;
}
