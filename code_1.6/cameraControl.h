#include <wiringSerial.h>
#include <string>

#define JEVOISMODEOFF	0
#define JEVOISMODEOCR	1
#define	JEVOISMODEBLK	2
#define JEVOISMODEOBJ	3

int jevoisMode;
bool streamOn;
bool msFound;

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
int pingCam(int fd);
char readObject(int fd);
char readObject(string line);
string readMsObject(int fd);
string camGetLineObjDetect(int fd);
char readMsSlot(int fd);

int pingCam(int fd){
	int amt = 0;
	//cout << "Ping" << endl;
	serialPuts(fd,"ping\n\0");
	string line = camGetLine(fd);
	//cout << line << endl;
	//cout << "cmp " <<line.find("ALIVE") << endl;
	while(line.find("ALIVE") != 0){
		amt++;
		if(line.find("ms") != string::npos)
		{
			cout << "Mothership detected" << endl;
			msFound = true;
			readObject(line);
		}			
		line = camGetLine(fd);
		cout << line;
		//cout << "cmp " <<line.find("ALIVE") << endl;
	}
	//cout << line;
	//cout << "End Ping" << endl;
	camGetLine(fd);
	return amt;
}

char readObject(string line){
	cout << "Reading Object String" << endl;
	cout << "Object Info: " << line << endl;
	char obj = (line.c_str())[line.find_first_of(' ')+1];
	cout << "Object rtn = " << obj << endl;
	return obj;
}

char readObject(int fd){
	cout << "Reading Object" << endl;
	string line = camGetLine(fd);
	camStreamOff(fd);
	cout << "Object Info: " << line << endl;
	char obj = (line.c_str())[line.find_first_of(' ')+1];
	pingCam(fd);
	return obj;
}
char readMsSlot(int fd){
	cout << "Reading MS Slot" << endl;
	string line = camGetLine(fd);
	camStreamOff(fd);
	cout << "MS slot Info: " << line << endl;
	char obj = (line.c_str())[line.find_first_of('_')+1];
	pingCam(fd);
	return obj;
}

int readObjectX(int fd){
	cout << "Reading for Object X Pos" << endl;
	string line = camGetLine(fd);
	camStreamOff(fd);
	cout << "Object Info: " << line << endl;
	string xLoc = line.substr(line.find("jpg")+4,4);
	cout << "xLoc first pass: " << xLoc << endl;
	xLoc = xLoc.substr(0,xLoc.find_first_of(' '));
	cout << "xLoc final string: " << xLoc << endl;
	return atoi(xLoc.c_str());
}

string readMsObject(int fd){
	string rtn;

	cout << "Reading MS Object" << endl;
	string line = camGetLine(fd);
	camStreamOff(fd);
	cout << "Object Info: " << line << endl;
	char obj = (line.c_str())[line.find_first_of(' ')+1];
	pingCam(fd);
	if(obj != 'm'){
		rtn = "ERR";
		return rtn;
	}
	int len = line.find(".jpg") - (line.find_first_of('_') + 1);
	rtn = line.substr(line.find_first_of(' ')+4,len);
	cout << "Derived Info: " << rtn << endl;
	return rtn;
}

char readBlock(int fd){
	cout << "Reading Block" << endl;
	string line = camGetLine(fd);
	cout << "Block = " << line;
	if ((line[0]) < 'A' || (line[0]) > 'F'){
		return (char)(line[1]);
	} else {
		return (char)(line[0]);
	} 
}

void rebootCam(int fd){
	cout << "Restart Camera" << endl;
	serialPuts(fd,"restart\n\0");
	cout << camGetLine(fd) << endl;
	return;
}

void camStreamOff(int fd){
	if(streamOn){
		cout << "Stream off: " << endl;
		jevoisMode = JEVOISMODEOFF;
		serialPuts(fd,"streamoff\n\0");
		cout << "Call Ping" << endl;
		pingCam(fd);
		//cout << camGetLine(fd) << endl;
	} else {
		cout << "Stream already off" << endl;
	}
	return;
}

void activateOCR(int fd){
	camStreamOff(fd);
	jevoisMode = JEVOISMODEOCR;
	cout << "Activate Camera Tesseract4{" << endl;
	serialPuts(fd,"setmapping2 YUYV 320 240 30 ME TesseractOCR4\n\0");
	serialPuts(fd,"setpar serout USB\n\0");
	//serialPuts(fd,"setpar serlog USB\n\0");
	serialPuts(fd,"streamon\n\0");
	printChunkCam(fd,3);
	cout << "}" << endl;
	streamOn = true;
	return;
}

void activateObjectDetect(int fd){
	camStreamOff(fd);
	jevoisMode = JEVOISMODEOBJ;
	cout << "Active Object Detector{" << endl;
	serialPuts(fd,"setmapping2 YUYV 320 240 30 JeVois ObjectDetect\n\0");
	serialPuts(fd,"setpar serstyle Normal\n\0");
	serialPuts(fd,"setpar serout USB\n\0");
	serialPuts(fd,"setpar serlimit 1\n\0");
	serialPuts(fd,"setpar goodpts 5...100\n\0");
	serialPuts(fd,"streamon\n\0");
	//cout << "messages sent" << endl;
	printChunkCam(fd,6);
	cout << "}" << endl;
	streamOn = true;
	return;
}

void activateBlockDetect(int fd){
	camStreamOff(fd);
	jevoisMode = JEVOISMODEBLK;
	cout << "Active White Detector{" << endl;
	serialPuts(fd,"setmapping2 YUYV 320 240 30 ME WhiteTracker\n\0");
	serialPuts(fd,"setpar serout USB\n\0");
	//serialPuts(fd,"setpar serlimit 1\n\0");
	serialPuts(fd,"streamon\n\0");
	printChunkCam(fd,3);
	cout << "}" << endl;
	streamOn = true;
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
	//cout << "start getLine" << endl;
	string rtn;
	char ch = '\0';
	//cout << "line:{";
	while(ch != '\n'){
		ch = (char)serialGetchar(fd);
		if(ch >= ' ' && ch <= 'z'){
			rtn += ch;
			//cout << ch;
		}
	}
	rtn += '\n';
	//cout << "}" << endl;
	return rtn;
}

string camGetLineObjDetect(int fd){	
	int tries=0;
	string rtn;
	char ch = '\0';
	while(ch != '\n' && tries<3000){
		if(serialDataAvail(fd)>0){
			ch = (char)serialGetchar(fd);
			if(ch >= ' ' && ch <= 'z'){
				rtn += ch;
			}
		}
		tries++;
		usleep(1000);
	}
	if(tries>=3000)
		return "error";
	rtn += '\n';
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
	return;
}
