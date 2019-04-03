#include "DriveTest6.h"

using namespace std;

int main(){
	
	initilizePigpiod();	
	signal(2, ctrl_c_handler);
	
	int blockNum = 4; 
	int turned = 0; 
	//while(turned < blockNum)
//	{
		char letter;
		cameraUp();
		punchersDown();
		twistIn();
		
		cout << "Rotate to letter: " << endl;
		cin >> letter;
		rotateToLoad(letter);
		cout << "be prepared to lower" << endl;
		cout<< "button is " << gpio_read(pi, LIFTTOPSWITCH) << endl;
		twistOut();
		openClaw();
		cout << "claw coming down" << endl;
		usleep(1000000);
		lowerClaw();
		cout<< "button is " << gpio_read(pi, LIFTBOTTOMSWITCH) << endl;
		usleep(1500000);
		closeClaw();
		usleep(2000000);
		liftClaw();
		twistIn();
		usleep(1000000);
		openClaw();
		usleep(1000000);
		set_servo_pulsewidth(pi,TWISTSERVOPIN,0);
		set_servo_pulsewidth(pi,GRIPPERSERVOPIN,0);
		turned ++;
	//}

	/*sleep(1);
	cout << "Entering Punchers" << endl;
	punchersDown();
	punchersUp();
	sleep(1);
	punchersDown();
	*/
	//endProgram();

	return 0;
}
