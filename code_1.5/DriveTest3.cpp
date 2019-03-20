#include "DriveTest3.h"
#include "cameraControl.h"

#define FORWARDSPEED 	 250
#define STRAFESPEED  	 250
#define PIVOTSPEED		 150
#define UP		 	 FORWARD
#define DOWN		 BACKWARD
#define LIFTSPEED	 250
#define LIFTTIME	 6950000
#define LOWERTIME	 6900000
#define PIVOTTIME	 3500000
#define TWISTSERVOPIN	18 
#define GRIPPERSERVOPIN 17
#define CAMERASERVOPIN	24
#define MIN_SERVO	 600
#define	MAX_SERVO	 2300
#define CAMERAUP	 1600
#define CAMERADOWN	 900
#define TWISTIN 	 2400
#define TWISTOUT 	 500
#define FORWARDTIME1 2600000
#define FORWARDTIME2 870000
#define FORWARDTIME3 6600000
#define ENCODERPINA	 5
#define ENCODERPINB  12
#define NSFACING	 0
#define EWFACING	 1
#define PIVOTTICKS	 1100
#define TICKSPERFOOT 1120


using namespace std;

Adafruit_MotorHAT  hat (0x61,1600,-1,-1);
Adafruit_MotorHAT  hat2(0x60,1600,-1,-1);

Adafruit_DCMotor& frontLeft  = hat.getDC(1);
Adafruit_DCMotor& frontRight = hat.getDC(2);
Adafruit_DCMotor& backLeft   = hat.getDC(3);
Adafruit_DCMotor& backRight  = hat.getDC(4);

Adafruit_DCMotor& liftMotor  = hat2.getDC(1);

position RobotPosition;
int fdArduino;
int fdJevois;
int pi;

void ctrl_c_handler(int s){
	cout << "Caught signal " << s << endl;
	frontLeft.run(RELEASE);	
	frontRight.run(RELEASE);
	backLeft.run(RELEASE);
	backRight.run(RELEASE);
	liftMotor.run(RELEASE);
	halt();
	haltClaw();
	camStreamOff(fdJevois);
	rebootCam(fdJevois);
	hat.resetAll();
	hat2.resetAll();
	gpioTerminate();
	exit(1);
}


int main(){
	
	signal(2, ctrl_c_handler);
	
	//if (gpioInitialise() < 0) return -1;
	
	//int fdArduino = -1; //DO NOT USE!!!!! -Adam & Morganmake
	cout << "Open Arduino" << endl;
	fdArduino = serialOpen("/dev/ttyACM0", 57600);
	cout << "fdArduino = " << fdArduino << endl;
	if (fdArduino < 0) {
		cout << "ERROR!!! Can't talk to Arduino!" << endl;
		return 2;
	}
	char ch;
	while(ch != '\n'){
		ch = (char)serialGetchar(fdArduino);
		cout << ch;
	}
	cout << "Arduino Open" << endl;
	
	cout << "Open Jevois" << endl;
	fdJevois = serialOpen("/dev/ttyACM1", 115200);
	//fdJevois = serialOpen("/dev/ttyACM0", 115200);
	//fdJevois = -1;
	
	cout << "fdJevois = " << fdJevois << endl;
	if (fdJevois < 0) {
		cout << "ERROR!!! Can't talk to jevois!" << endl;
		//serialClose(6);
		return 3;
	}
	cout << "Jevois Open" << endl;
	
	pi = pigpio_start(NULL,NULL); /* Connect to Pi. */
	
	double startX = 0;
	double startY = 0;
	char pause;
	
	RobotPosition.curPos.NS = startY*TICKSPERFOOT;
	RobotPosition.curPos.EW	= startX*TICKSPERFOOT;
	
	double endX,endY;
	
	cout << "X: ";
	cin >> endX;
	cout << "Y: ";
	cin >> endY;
	cout << "start(" << startX << "," << startY << ")-";
	cout << "end(" << endX << "," << endY << ")" << endl;
	
	int ticksX = (endX - startX) * TICKSPERFOOT;
	int ticksY = (endY - startY) * TICKSPERFOOT;
	
	cout << "ticksX = " << ticksX << endl;
	cout << "ticksY = " << ticksY << endl;
	
	cout << "Y to Continue" << endl;
	cin >> pause;
	
	
	setCameraSettings(fdJevois);
	activateOCR(fdJevois);
	printCamInfo(fdJevois);
	//ch = readBlock(fdJevois);
	ch = 'C';
	cout << "Rotate to " << ch << endl;
	closeClaw();
	//rotateToLoad(ch);	
		
	cameraUp();
	twistOut();
	openClaw();

	sleep(1);
	//lowerClaw();
	wait(NULL);
	goToPointNS(endX*TICKSPERFOOT,endY*TICKSPERFOOT);
	
	halt();
	
	camStreamOff(fdJevois);
	activateObjectDetect(fdJevois);
	printCamInfo(fdJevois);
	
	closeClaw();
	liftClaw();
	wait(NULL);
	//sleep(16);
	twistIn();
	sleep(1);
	cameraDown();
	openClaw();
	
	camStreamOff(fdJevois);
	activateOCR(fdJevois);
	printCamInfo(fdJevois);
	
	//rotateToLoad('A');
	
	//camStreamOff(fdJevois);
	
	rebootCam(fdJevois);
	serialClose(fdArduino);
	serialClose(fdJevois);
	pigpio_stop(pi);
	cout << "End of Program" << endl;
	return 0;
}

int checkEncoder(int stop){
		char ch = '0';
		char numString[10] = {'\0','\0','\0','\0','\0','\0','\0','\0','\0','\0'};
		int num = -1;
		int n = 0;
		cout << "Start Check Encoder: " << stop << endl; 
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
		}
		RobotPosition.curPos.arduino = num;
		cout << "End Check Encoder :" << num << endl; 
		return num;
}

int goForward(int distance,int facing){ //facing = 0 NS, 1 EW
	printf("Drive Forward!\n");
	//All motors forward
	frontLeft.run(FORWARD);	
	frontRight.run(FORWARD);
	backLeft.run(FORWARD);
	backRight.run(FORWARD);
	
	frontLeft.setSpeed(50);
	frontRight.setSpeed(50);
	backLeft.setSpeed(50);
	backRight.setSpeed(50);

	usleep(40000);
	
	int startPos,stopPos;
	if (facing == 0){
		startPos = RobotPosition.curPos.NS;
	} else {
		startPos = RobotPosition.curPos.EW;
	}
	
	startPos += RobotPosition.curPos.arduino;
	
	stopPos = startPos + distance;
	
	//All motors max speed
	frontLeft.setSpeed(FORWARDSPEED);
	frontRight.setSpeed(FORWARDSPEED-2);
	backLeft.setSpeed(FORWARDSPEED);
	backRight.setSpeed(FORWARDSPEED-2);
	
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << RobotPosition.curPos.NS << endl;
	cout << "Current EW = " << RobotPosition.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	
	if (facing == 0){
		cout << "North South" << endl;
		RobotPosition.curPos.NS += checkEncoder(stopPos) - startPos;
	} else {
		cout << "East West" <<endl;
		RobotPosition.curPos.EW += checkEncoder(stopPos) - startPos;
	}
	//halt();
	cout << "Start = " << startPos << endl;
	cout << "Current NS = " << RobotPosition.curPos.NS << endl;
	cout << "Current EW = " << RobotPosition.curPos.EW << endl;
	cout << "Stop = " << stopPos << endl;
	cout << "End Forward" << endl;
	return 0;
}

int strafeLeft(int distance){
	printf("Strafe Left!\n");
	//Motor directions for left
	frontLeft.run(BACKWARD);
	frontRight.run(FORWARD);
	backLeft.run(FORWARD);
	backRight.run(BACKWARD);
	//Go at strafing speed
	frontLeft.setSpeed(STRAFESPEED);
	frontRight.setSpeed(STRAFESPEED);
	backLeft.setSpeed(STRAFESPEED);
	backRight.setSpeed(STRAFESPEED);
	return 0;
}

int strafeRight(int distance){
	printf("Strafe Right!\n");
	//Motor direction for right
	frontLeft.run(FORWARD);
	frontRight.run(BACKWARD);
	backLeft.run(BACKWARD);
	backRight.run(FORWARD);
	//Go at strafing speed
	frontLeft.setSpeed(STRAFESPEED);
	frontRight.setSpeed(STRAFESPEED);
	backLeft.setSpeed(STRAFESPEED);
	backRight.setSpeed(STRAFESPEED);
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
	
	int startPos,stopPos;
	startPos = RobotPosition.curPos.NS;
	stopPos = startPos + (PIVOTTICKS*positions);
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

// Dillon 3/18
int turnToFace(int direction)
{
	// Check that direction value is {0,1,2,3}
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
int goToPointNS(int Xdest, int Ydest)
{
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

int goToPointEW(int Xdest, int Ydest)
{
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

int boardMothership(char letter)
{
	int stop;
	if(letter == 'C')
	{
		// strafe left
		stop = RobotPosition.curPos.EW + 2000;	//placeholder	
		frontLeft.run(BACKWARD);
		frontRight.run(FORWARD);
		backLeft.run(FORWARD);
		backRight.run(BACKWARD);
		checkEncoder(stop);
		halt();
		
		// drive forward
		stop = RobotPosition.curPos.EW + 2500;	//placeholder	
		frontLeft.run(FORWARD);	
		frontRight.run(FORWARD);
		backLeft.run(FORWARD);
		backRight.run(FORWARD);
		checkEncoder(stop);
		halt();
		
		// face ramp
		pivotRight(1);
	}
	
	else if(letter == 'D')
	{
		// strafe right
		stop = RobotPosition.curPos.EW + 2000;	//placeholder	
		frontLeft.run(FORWARD);
		frontRight.run(BACKWARD);
		backLeft.run(BACKWARD);
		backRight.run(FORWARD);
		checkEncoder(stop);
		halt();
		
		// drive forward
		stop = RobotPosition.curPos.EW + 2500;	//placeholder		
		frontLeft.run(FORWARD);	
		frontRight.run(FORWARD);
		backLeft.run(FORWARD);
		backRight.run(FORWARD);
		checkEncoder(stop);
		halt();
		
		// face ramp
		pivotLeft(1);
	}
	
	// get C and D on the right and left sides of the robot
	rotateToLoad('E');
	system("python StepperTwelfthCCW.py");
	
	// drive up ramp
	stop = RobotPosition.curPos.EW + 3000;		//placeholder	
	frontLeft.run(FORWARD);	
	frontRight.run(FORWARD);
	backLeft.run(FORWARD);
	backRight.run(FORWARD);
	checkEncoder(stop);
	halt();
	
	// dump blocks
	
	system("python StepperCCW.py");
	
	// drive forward to line up with B and E
	stop = RobotPosition.curPos.EW + 1500;		//placeholder	
	frontLeft.run(FORWARD);	
	frontRight.run(FORWARD);
	backLeft.run(FORWARD);
	backRight.run(FORWARD);
	checkEncoder(stop);
	halt();
	
	// dump blocks

	system("python StepperCCW.py");
	
	// drive forward to line up with A and F
	stop = RobotPosition.curPos.EW + 1500;		//placeholder	
	frontLeft.run(FORWARD);	
	frontRight.run(FORWARD);
	backLeft.run(FORWARD);
	backRight.run(FORWARD);
	checkEncoder(stop);
	halt();
	
	// dump blocks
	
	// finishing light
	
	// self destruct
	
	return 0;
}

// Dillon 3/19
int lookForBlock(int blockX, int blockY)
{
	int Ydisp = blockY - RobotPosition.curPos.NS;
	int Xdisp = blockX - RobotPosition.curPos.EW;
	switch(RobotPosition.curPos.facing)
	{
		// facing North to begin
		case 0: // block is north
				if(Xdisp>0 && Ydisp>0)
				{
					goToPointNS(blockX-1, blockY);
					turnToFace(1);
				}
				else if(Xdisp<0 && Ydisp>0)
				{
					goToPointNS(blockX+1, blockY);
					turnToFace(3);
				}
				else if(Xdisp==0 && Ydisp>0)
				{
					goToPointNS(blockX, blockY-1);
				}
				// block is south
				else if(Xdisp>0 && Ydisp<0)
				{
					goToPointEW(blockX, blockY+1);
					turnToFace(2);
				}
				else if(Xdisp<0 && Ydisp<0)
				{
					goToPointEW(blockX, blockY+1);
					turnToFace(2);
				}
				else if(Xdisp==0 && Ydisp<0)
				{
					goToPointEW(blockX, blockY+1);
				}
				// block is directly east or west
				else if(Xdisp>0 && Ydisp==0)
				{
					goToPointEW(blockX-1, blockY);
				}	
				else if(Xdisp<0 && Ydisp==0)
				{
					goToPointEW(blockX+1, blockY);
				}	
				else
					cout << "I'm facing North, but I don't know where to go" << endl;
				break;
				
		// facing East to begin		
		case 1: // block is north
				if(Xdisp>0 && Ydisp>0)
				{
					goToPointEW(blockX, blockY-1);
					turnToFace(0);
				}
				else if(Xdisp<0 && Ydisp>0)
				{
					goToPointNS(blockX, blockY+1);
					turnToFace(3);
				}
				else if(Xdisp==0 && Ydisp>0)
				{
					goToPointNS(blockX, blockY-1);
				}
				// block is south
				else if(Xdisp>0 && Ydisp<0)
				{
					goToPointEW(blockX, blockY+1);
					turnToFace(2);
				}
				else if(Xdisp<0 && Ydisp<0)
				{
					goToPointNS(blockX+1, blockY);
					turnToFace(3);
				}
				else if(Xdisp==0 && Ydisp<0)
				{
					goToPointEW(blockX, blockY+1);
				}
				// block is directly east or west
				else if(Xdisp>0 && Ydisp==0)
				{
					goToPointEW(blockX-1, blockY);
				}	
				else if(Xdisp<0 && Ydisp==0)
				{
					goToPointEW(blockX+1, blockY);
				}	
				else
					cout << "I'm facing East, but I don't know where to go" << endl;
				break;
				
		// facing South to begin
		case 2: // block is north
				if(Xdisp>0 && Ydisp>0)
				{
					goToPointEW(blockX, blockY-1);
					turnToFace(0);
				}
				else if(Xdisp<0 && Ydisp>0)
				{
					goToPointNS(blockX, blockY-1);
					turnToFace(0);
				}
				else if(Xdisp==0 && Ydisp>0)
				{
					goToPointNS(blockX, blockY-1);
				}
				// block is south
				else if(Xdisp>0 && Ydisp<0)
				{
					goToPointNS(blockX-1, blockY);
					turnToFace(1);
				}
				else if(Xdisp<0 && Ydisp<0)
				{
					goToPointNS(blockX+1, blockY);
					turnToFace(3);
				}
				else if(Xdisp==0 && Ydisp<0)
				{
					goToPointEW(blockX, blockY+1);
				}
				// block is directly east or west
				else if(Xdisp>0 && Ydisp==0)
				{
					goToPointEW(blockX-1, blockY);
				}	
				else if(Xdisp<0 && Ydisp==0)
				{
					goToPointEW(blockX+1, blockY);
				}	
				else
					cout << "I'm facing South, but I don't know where to go" << endl;
				break;
				
		// facing West to begin
		case 3: // block is north
				if(Xdisp>0 && Ydisp>0)
				{
					goToPointEW(blockX-1, blockY);
					turnToFace(1);
				}
				else if(Xdisp<0 && Ydisp>0)
				{
					goToPointEW(blockX, blockY-1);
					turnToFace(0);
				}
				else if(Xdisp==0 && Ydisp>0)
				{
					goToPointNS(blockX, blockY-1);
				}
				// block is south
				else if(Xdisp>0 && Ydisp<0)
				{
					goToPointNS(blockX-1, blockY);
					turnToFace(1);
				}
				else if(Xdisp<0 && Ydisp<0)
				{
					goToPointEW(blockX, blockY+1);
					turnToFace(2);
				}
				else if(Xdisp==0 && Ydisp<0)
				{
					goToPointEW(blockX, blockY+1);
				}
				// block is directly east or west
				else if(Xdisp>0 && Ydisp==0)
				{
					goToPointEW(blockX-1, blockY);
				}	
				else if(Xdisp<0 && Ydisp==0)
				{
					goToPointEW(blockX+1, blockY);
				}	
				else
					cout << "I'm facing West, but I don't know where to go" << endl;
				break;
	}
	return 0;
}

pid_t liftClaw(){
	pid_t pid;
	printf("Lift Claw!\n");
	if(RobotPosition.getLiftPos() == 0){
		printf("Lifting\n");
		liftMotor.run(UP);
		liftMotor.setSpeed(LIFTSPEED);
		RobotPosition.switchLiftPos();
		pid = fork();
		if (pid == 0){
			usleep(LIFTTIME);
			haltClaw();
			exit(0);
		}
	} else {
		printf("Claw already up!\n");
		return 1;
	}		
	return pid;
}

pid_t lowerClaw(){
	pid_t pid;
	printf("Lower Claw\n");
	if(RobotPosition.getLiftPos() == 1){
		printf("Lowering\n");
		liftMotor.run(DOWN);
		liftMotor.setSpeed(LIFTSPEED);
		RobotPosition.switchLiftPos();
		pid = fork();
		if (pid == 0){
			usleep(LOWERTIME);
			haltClaw();
			exit(0);
		}
	} else {
		printf("Claw already down!\n");
		return 1;
	}
	return pid;
}

int openClaw(){
	if(RobotPosition.getClawPos() == 1){
		printf("Open Claw\n");
		//gpioServo(GRIPPERSERVOPIN,MAX_SERVO);
		set_servo_pulsewidth(pi,GRIPPERSERVOPIN,MAX_SERVO);
		RobotPosition.switchClawPos();
	}
	return RobotPosition.getClawPos();
}

int closeClaw(){
	if(RobotPosition.getClawPos() == 0){
		printf("Close Claw\n");
		//gpioServo(GRIPPERSERVOPIN,MIN_SERVO);
		set_servo_pulsewidth(pi,GRIPPERSERVOPIN,MIN_SERVO);
		RobotPosition.switchClawPos();
	}
	return RobotPosition.getClawPos();
}

int cameraUp(){
	if(RobotPosition.getCameraPos() == 0){
		printf("Camera Up\n");
		set_servo_pulsewidth(pi,CAMERASERVOPIN,CAMERAUP);
		RobotPosition.switchCameraPos();
	}
	return RobotPosition.getCameraPos();
}

int cameraDown(){
	if(RobotPosition.getCameraPos() == 1){
		printf("Camera Down\n");
		set_servo_pulsewidth(pi,CAMERASERVOPIN,CAMERADOWN);
		RobotPosition.switchCameraPos();
	}
	return RobotPosition.getCameraPos();
}

int twistIn(){
	if(RobotPosition.getTwistPos() == 1){
		printf("Twist In\n");
		set_servo_pulsewidth(pi,TWISTSERVOPIN,TWISTIN);
		RobotPosition.switchTwistPos();
		usleep(100000);
	}
	return RobotPosition.getTwistPos();
}

int twistOut(){
	if(RobotPosition.getTwistPos() == 0){
		printf("Twist Out\n");
		set_servo_pulsewidth(pi,TWISTSERVOPIN,TWISTOUT);
		RobotPosition.switchTwistPos();
		usleep(100000);
	}
	return RobotPosition.getTwistPos();
}

int haltClaw(){
	printf("HALT CLAW!\n");
	liftMotor.setSpeed(0);
	return 0;
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
			system("python StepperClockwise.py");
			cout << RobotPosition.incLoadZone() << endl;
			cout << "Load Zone is now " << RobotPosition.getLoadZone() << endl;
		}
	} else if (dist < 0) {
		for(int i = 0; i > dist; i--){
			system("python StepperCCW.py");
			RobotPosition.decLoadZone();
			cout << "Load Zone is now " << RobotPosition.getLoadZone() << endl;
		}
	}
	cout << "Final Load Zone is now " << RobotPosition.getLoadZone() << endl;
	return RobotPosition.getLoadZone();
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
