#include <wiringSerial.h>
#include <string>

using namespace std;

void activateOCR(int fd);
void activateObjectDetect(int fd);
void setCameraSettings(int fd);
string camGetLine(int fd);
void printCamInfo(int fd);
void printChunkCam(int fd, int lines);
void camStreamOff(int fd);
void rebootCam(int fd);
char readBlock(int fd);

char readBlock(int fd){
	cout << "Reading Block" << endl;
	string line = camGetLine(fd);
	cout << "Block = " << line;
	return (char)(line[0]); 
}

void rebootCam(int fd){
	serialPuts(fd,"restart\n\0");
	return;
}

void camStreamOff(int fd){
	serialPuts(fd,"streamoff\n\0");
	cout << camGetLine(fd);
	return;
}

void activateOCR(int fd){
	cout << "Activate Camera Tesseract4{" << endl;
	serialPuts(fd,"setmapping2 YUYV 320 240 30 ME TesseractOCR4\n\0");
	serialPuts(fd,"setpar serout USB\n\0");
	//serialPuts(fd,"setpar serlog USB\n\0");
	serialPuts(fd,"streamon\n\0");
	printChunkCam(fd,3);
	cout << "}" << endl;
	return;
}

void activateObjectDetect(int fd){
	serialPuts(fd,"setmapping2 YUYV 320 240 30 JeVois ObjectDetect\n\0");
	serialPuts(fd,"setpar serstyle Normal\n\0");
	serialPuts(fd,"setpar serout USB\n\0");
	serialPuts(fd,"streamon\n\0");
	printChunkCam(fd,4);
	return;
}

void setCameraSettings(int fd){
	cout << "Camera settings{" << endl;
	serialPuts(fd,"setcam autoexp 0\n\0");
	serialPuts(fd,"setcam autogain 0\n\0");
	serialPuts(fd,"setcam autowb 0\n\0");
	
	serialPuts(fd,"setcam redbal 107\n\0");
	serialPuts(fd,"setcam bluebal 206\n\0");
	serialPuts(fd,"setcam gain 30\n\0");
	serialPuts(fd,"setcam absexp 314\n\0");
	printChunkCam(fd,7);
	cout << "}" << endl;
	return;
}

string camGetLine(int fd){
	string rtn;
	char ch = '\0';
	cout << "line:{";
	while(ch != '\n'){
		ch = (char)serialGetchar(fd);
		cout << ch;
		rtn += ch;
	}
	cout << "}" << endl;
	return rtn;
}

void printCamInfo(int fd){
	cout << "Camera Info" << endl;
	serialPuts(fd,"info\n\0");
	printChunkCam(fd,6);
	return;
}

void printChunkCam(int fd,int lines){
	for(int i = 0; i < lines; i++){
		cout << camGetLine(fd);
	}
	cout << endl;
	return;
}