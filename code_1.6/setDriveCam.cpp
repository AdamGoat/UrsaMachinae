#include "DriveTest6.h"

using namespace std;

int main(){
	
	initilizePigpiod();	
	signal(2, ctrl_c_handler);
	
	cameraDrive();
	sleep(1);
	cout << "Done" << endl;
	
	return 0;
}
