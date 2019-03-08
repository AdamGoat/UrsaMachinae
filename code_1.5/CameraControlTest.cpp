#include "stdio.h"
#include <iostream>
#include <signal.h>
#include <unistd.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <string>

#include "/home/pi/Documents/UrsaMachinae/code_1.5/Adafruit_MotorHAT.h"
#include <pigpiod_if2.h>
#include <pigpio.h>
#include  "RED.h"

#include <wiringSerial.h>

using namespace std;

int main(int argc, char* argv[]){
	
	int fd = serialOpen("/dev/ttyACM1", 115200);
	if (fd < 0) {
		cout << "ERROR!!! Can't talk to jevois!" << endl;
		return 1;
	}
	
	serialPuts(fd,"setmapping2 YUYV 320 240 30 ME TesseractOCR4\n\0");
	
	serialPuts(fd,"setcam autoexp 0\n\0");
	serialPuts(fd,"setcam autogain 0\n\0");
	serialPuts(fd,"setcam autowb 0\n\0");
	
	serialPuts(fd,"setcam redbal 107\n\0");
	serialPuts(fd,"setcam bluebal 206\n\0");
	serialPuts(fd,"setcam gain 30\n\0");
	serialPuts(fd,"setcam absexp 314\n\0");
	serialPuts(fd,"info\n\0");
	
	serialPuts(fd,"setpar serout USB\n\0");
	serialPuts(fd,"setpar serlog USB\n\0");
	
	serialPuts(fd,"streamon\n\0");
	
	sleep(2);
	serialPuts(fd,"streamoff\n\0");
	
	serialPuts(fd,"setmapping2 YUYV 320 240 30 JeVois ObjectDetect\n\0");
	//serialPuts(fd,"setmapping 48\n\0");
	serialPuts(fd,"info\n\0");
	serialPuts(fd,"setpar serstyle Normal\n\0");
	sleep(1);
	serialPuts(fd,"streamon\n\0");
	
	
	
	while(serialDataAvail(fd) >0 || true){
		cout << (char)serialGetchar(fd);
	}
	cout << "\nEOD" << endl;	
	
	return 0;
}
