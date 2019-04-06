#include "DriveTest6.h"

using namespace std;

int main(){
	
	initilizePigpiod();	
	while(true){
		cout << "Light On" << endl;	
		gpio_write(pi,FINISHLEDPIN,1);
		sleep(5);
		cout << "Light Off" << endl;
		gpio_write(pi,FINISHLEDPIN,0);
		sleep(5);
	}
	cout << "Done" << endl;
	
	return 0;
}
