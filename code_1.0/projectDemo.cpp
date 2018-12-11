#include "projectDemo.h"

#define FORWARDSPEED 	 250
#define STRAFESPEED  	 250
#define PIVOTSPEED	 255
#define UP		 BACKWARD
#define DOWN		 FORWARD
#define LIFTSPEED	 250
#define LIFTTIME	 2650000
#define LOWERTIME	 1300000
#define PIVOTTIME	 3500000
#define PICTURETIME	 80000
#define ARDUINOPIN	 17   
#define SERVOPIN	 18 		//Dosn't change servo pin, just for record keeping
#define MIN_SERVO	 600
#define	MAX_SERVO	 1500
#define FORWARDTIME1 2600000
#define FORWARDTIME2 870000
#define FORWARDTIME3 6600000

using namespace std;

Adafruit_MotorHAT  hat(0x61, 1600,-1,-1);
Adafruit_MotorHAT hat2(0x60,1600,-1,-1);

Adafruit_DCMotor& frontLeft  = hat.getDC(1);
Adafruit_DCMotor& frontRight = hat.getDC(2);
Adafruit_DCMotor& backLeft   = hat.getDC(3);
Adafruit_DCMotor& backRight  = hat.getDC(4);

Adafruit_DCMotor& liftMotor  = hat2.getDC(1);

void ctrl_c_handler(int s){
	cout << "Caught signal " << s << endl;
	frontLeft.run(RELEASE);	
	frontRight.run(RELEASE);
	backLeft.run(RELEASE);
	backRight.run(RELEASE);
	liftMotor.run(RELEASE);
	halt();
	haltClaw();
	hat.resetAll();
	hat2.resetAll();
	gpioTerminate();
	exit(1);
}

//Called when gpio 18 changes level
void blockDetected(int gpio, int level, uint32_t tick){
	// 1 = rising edge
	if(level == 1){
		cout << "STOP!!!" << endl;
		//stop for 10 seconds
		halt();
		usleep(10000000);
	}
}


int main(int argc, char* argv[]){
	//set Ctrl_C action
	//struct sigaction ctrl_C;
	//ctrl_C.sa_handler = ctrl_c_handler;
	//ctrl_C.sa_flags = 0;
	//sigemptyset(&ctrl_C.sa_mask);
	signal(2, ctrl_c_handler);
	//sigaction(2,&ctrl_C,NULL);
	
	//int cycle = 0;
	//int input;
	
	int arduinoFD;
	char ardIn[1] = {'X'};
	char junk[8192];
	//clear file?
	if((arduinoFD = open("/dev/ttyACM0",O_RDONLY)) == -1){
		perror("BAD Arduino File\n");
		exit(2);
	}
	//arduinoFD = 0;
	cout << "arduinoFD = " << arduinoFD << endl;
	
	pthread_t pics;
	
	cout << "Hello World" << endl;
	
	//initialise pin 17 for pigpio
	if (gpioInitialise() < 0) return -1;
	/*gpioSetMode(ARDUINOPIN,PI_INPUT);
	gpioSetPullUpDown(ARDUINOPIN,PI_PUD_DOWN);
	//set alert function to trigger when pin 17 changes level
	gpioSetAlertFunc(ARDUINOPIN,blockDetected);*/
	
	//pid_t clawPID = 0;
	openClaw();
	goForward(100);
	cout << "Going Forward: " << ardIn[0] << endl;
	//lseek(arduinoFD,SEEK_END,0);
	read(arduinoFD,junk,sizeof(char)*8192);
	while(ardIn[0] != 'A'){
		//cout << "In Loop" << endl;
		read(arduinoFD,&(ardIn[0]),sizeof(char));
		//cout << "rd amt: " << rdAmt;
		cout <<  ": " << ardIn[0] << endl;
		usleep(10);
	}
	cout << "Last ardIn: " << ardIn[0] << endl;
	
	//usleep(FORWARDTIME1);
	halt();
	
	pthread_create(&pics,NULL,takePic,NULL);
	printf("Taking Picture!\n");
	usleep(PICTURETIME);

	lowerClaw();
	wait(NULL);
	
	goForward(100);
	usleep(FORWARDTIME2);
	halt();
	
	closeClaw();
	
	usleep(500000);
	/*clawPID = */liftClaw();
	wait(NULL);
	
	haltClaw();
	pivotRight(1);
	
	goForward(200);
	close(arduinoFD);
	if((arduinoFD = open("/dev/ttyACM0",O_RDONLY|O_TRUNC)) == -1){
		perror("BAD Arduino File\n");
		exit(2);
	}
	cout << "arduinoFD = " << arduinoFD << endl;
	ardIn[0] = '\0';
	read(arduinoFD,&junk,sizeof(char)*8192);
	junk[8191] = '\0';
	//cout << "junk:" <<  junk << endl;
	//lseek(arduinoFD,SEEK_END,0);
	while(ardIn[0] != 'A'){
		read(arduinoFD,&(ardIn[0]),sizeof(char));
		cout << ": " << ardIn[0] << endl;
		usleep(10);
	}
	cout << "Last ardIn: " << ardIn[0] << endl;
	//usleep(FORWARDTIME3);
	halt();
	usleep(500000);

	goForward(100);
	usleep(FORWARDTIME2);
	halt();

	
	pthread_join(pics,NULL);
	printf("Picture Done!\n");
	usleep(5000);
	
	openClaw();
	usleep(1000000);
	cout << "All Done!" << endl;
	ctrl_c_handler(-1);
	close(arduinoFD);


	return 0;
}

int goForward(int distance){
	printf("Drive Forward!\n");
	//All motors forward
	frontLeft.run(FORWARD);	
	frontRight.run(FORWARD);
	backLeft.run(FORWARD);
	backRight.run(FORWARD);
	//All motors max speed
	frontLeft.setSpeed(FORWARDSPEED);
	frontRight.setSpeed(FORWARDSPEED);
	backLeft.setSpeed(FORWARDSPEED);
	backRight.setSpeed(FORWARDSPEED);
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
	printf("Pivot Left\n!");
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
	//run for appropriate amount of time
	usleep(PIVOTTIME*positions);
	//stop
	halt();
	return 0;
}

int pivotRight(int positions){
	printf("Pivot Right\n");
	//Motor directions for pivot right
	frontLeft.run(FORWARD);
	frontRight.run(BACKWARD);
	backLeft.run(FORWARD);
	backRight.run(BACKWARD);
	//go at pivot speed
	frontLeft.setSpeed(PIVOTSPEED);
	frontRight.setSpeed(PIVOTSPEED);
	backLeft.setSpeed(PIVOTSPEED);
	backRight.setSpeed(PIVOTSPEED);
	//run for appropriate amount of time
	usleep(PIVOTTIME*positions);
	//stop
	halt();
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
			//haltClaw();
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
			//haltClaw();
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
		gpioServo(SERVOPIN,MAX_SERVO);
		RobotPosition.switchClawPos();
	}
	return RobotPosition.getClawPos();
}

int closeClaw(){
	if(RobotPosition.getClawPos() == 0){
		printf("Close Claw\n");
		gpioServo(SERVOPIN,MIN_SERVO);
		RobotPosition.switchClawPos();
	}
	return RobotPosition.getClawPos();
}

int haltClaw(){
	printf("HALT CLAW!\n");
	liftMotor.setSpeed(0);
	return 0;
}

	
int halt(){
	printf("HALT!\n");
	//stop all motors
	frontLeft.setSpeed(0);
	frontRight.setSpeed(0);
	backLeft.setSpeed(0);
	backRight.setSpeed(0);
	//liftMotor.setSpeed(0);
	return 0;
}

void* takePic(void* args){
	system("./pic.sh");
	return NULL;
}
	
	

