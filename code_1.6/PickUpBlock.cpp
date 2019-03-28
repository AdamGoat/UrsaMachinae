#include "DriveTest5.h"
#include "cameraControl.h"

using namespace std;

Adafruit_MotorHAT  hat2(0x60,1600,-1,-1);
Adafruit_DCMotor& liftMotor  = hat2.getDC(1);
position RobotPosition;
int pi;


void ctrl_c_handler(int s){
	cout << "Caught signal " << s << endl;
	
	liftMotor.run(RELEASE);
	
	haltClaw();
	hat2.resetAll();
	//gpioTerminate();
	gpio_write(pi, ARDUINORESETPIN, 0);
	usleep(5);
	gpio_write(pi, ARDUINORESETPIN, 1);
	pigpio_stop(pi);
	exit(1);
}

int main(){
	
	signal(2, ctrl_c_handler);
	
	char l = 'B';

	//Claw out
	twistOut();
	sleep(2);
	//Lower to grab block
	lowerClaw();
	sleep(2);
	//Grab block
	closeClaw();
	sleep(2);
	//Lift block
	liftClaw();
	sleep(2);
	//Rotate Lazy Morgan
	rotateToLoad(l);
	l++;
	sleep(2);
	//Claw in
	twistIn();
	sleep(2);
	//Drop Block
	openClaw();
	sleep(2);
	
}

void liftClaw(){
	printf("Lift Claw!\n");
	if(RobotPosition.getLiftPos() == 0){
		printf("Lifting\n");
		liftMotor.run(UP);
		liftMotor.setSpeed(LIFTSPEED);

		while(!gpio_read(pi, LIFTTOPSWITCH))
		{
			usleep(1000);
		}
		RobotPosition.switchLiftPos();
		haltClaw();
	} else {
		printf("Claw already up!\n");
		return;
	}	
	return;	
}

void lowerClaw(){
	printf("Lower Claw\n");
	if(RobotPosition.getLiftPos() == 1){
		printf("Lowering\n");
		liftMotor.run(DOWN);
		liftMotor.setSpeed(LIFTSPEED);

		while(!gpio_read(pi, LIFTBOTTOMSWITCH))
		{
			usleep(1000);
		}
		RobotPosition.switchLiftPos();
		haltClaw();
	} else {
		printf("Claw already down!\n");
		return;
	}
	return;
}

int openClaw(){
	if(RobotPosition.getClawPos() == 1 || true){
		printf("Open Claw\n");
		//gpioServo(GRIPPERSERVOPIN,MAX_SERVO);
		set_servo_pulsewidth(pi,GRIPPERSERVOPIN,MAX_SERVO);
		RobotPosition.switchClawPos();
	}
	return RobotPosition.getClawPos();
}

int closeClaw(){
	if(RobotPosition.getClawPos() == 0 || true){
		printf("Close Claw\n");
		//gpioServo(GRIPPERSERVOPIN,MIN_SERVO);
		set_servo_pulsewidth(pi,GRIPPERSERVOPIN,MIN_SERVO);
		RobotPosition.switchClawPos();
	}
	return RobotPosition.getClawPos();
}

int twistIn(){
	if(RobotPosition.getTwistPos() == 1 && RobotPosition.getLiftPos() == 1 && gpio_read(pi, LIFTTOPSWITCH)){
		printf("Twist In\n");
		set_servo_pulsewidth(pi,TWISTSERVOPIN,TWISTIN);
		RobotPosition.switchTwistPos();
		usleep(100000);
	} else {
		cout << "Cant twist in!" << endl;
	}
	return RobotPosition.getTwistPos();
}

int twistOut(){
	cameraUp();
	if(RobotPosition.getTwistPos() == 0 && gpio_read(pi, LIFTTOPSWITCH)){
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

void endProgram(){
	
	gpio_write(pi, ARDUINORESETPIN, 0);	
	usleep(5);
	gpio_write(pi, ARDUINORESETPIN, 1);
	pigpio_stop(pi);
	cout << "End of Program" << endl;
	return;
}

int cameraUp(){
	if(RobotPosition.getCameraPos() == 0 || true){
		printf("Camera Up\n");
		set_servo_pulsewidth(pi,CAMERASERVOPIN,CAMERAUP);
		RobotPosition.switchCameraPos();
	}
	return RobotPosition.getCameraPos();
}

int cameraDown(){
	if(RobotPosition.getCameraPos() == 1 || true){
		printf("Camera Down\n");
		set_servo_pulsewidth(pi,CAMERASERVOPIN,CAMERADOWN);
		RobotPosition.switchCameraPos();
	}
	return RobotPosition.getCameraPos();
}
