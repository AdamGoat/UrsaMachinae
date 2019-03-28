#include "DriveTest5.h"
#include "cameraControl.h"

using namespace std;

int main(){
	
}

int boardMothership(char letter){
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
	punchersUp();
	sleep(1);
	punchersDown();
	usleep(500000);
	
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
	punchersUp();
	sleep(1);
	punchersDown();
	usleep(500000);

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
	punchersUp();
	sleep(1);
	punchersDown();
	usleep(500000);
	
	// finishing light
	
	// self destruct
	
	return 0;
}
