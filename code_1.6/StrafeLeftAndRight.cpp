#include "DriveTest6.h"

using namespace std;

int main(){
	signal(2, ctrl_c_handler);
	initilizePigpiod();
	initilizeArduinoSerial();
	double feet;
	cin >> feet;
	waitForGreen();
	strafeRight(feet*STRAFETICKSPERFOOT);
/*	system("../code_1.0/halt");
	halt();
	sleep(2);
	strafeRight(feet*STRAFETICKSPERFOOT);
	halt();
*/	endProgram();
	return(0);
}
