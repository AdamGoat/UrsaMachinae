#include <fstream>
#include <iostream>

using namespace std;

int main(){
	ifstream mars("../cords.json");
	
	if(!(mars.is_open())){
		cout << "File not open" << endl;
		return 1;
	}
	
	string line = "init\n";
	int size;
	int xCords[6];
	int yCords[6];
	
	getline(mars,line);
	//cout << line << endl;
	cout << "Find size" << endl;
	while(line.find("size") > 100){
		getline(mars,line);
		//cout << line << endl;
	}
	line = line.substr(line.find_last_of(',')-1,1);
	size = atoi(line.c_str());
	cout << "size = " << size << endl;
	
	cout << "Find x coords" << endl;
	while(line.find("x coords") > 100){
		getline(mars,line);
		cout << line << endl;
	}
	line = line.substr(line.find_first_of('[')+1,size*2-1);
	cout << "Xlist is " << line << endl;
	
	for (int i = 0; i < size; i++){
		xCords[i] = atoi(&((line.c_str())[i*2]));
		cout << xCords[i] << endl;
	}
	
	cout << "Find y coords" << endl;
	while(line.find("y coords") > 100){
		getline(mars,line);
		cout << line << endl;
	}
	line = line.substr(line.find_first_of('[')+1,size*2-1);
	cout << "Ylist is " << line << endl;
	
	for (int i = 0; i < size; i++){
		yCords[i] = atoi(&((line.c_str())[i*2]));
		cout << yCords[i] << endl;
	}
	
	pair<double,double> locals[size];
	
	for(int i = 0; i < size; i++){
		locals[i].first=xCords[i]-4;
		locals[i].second=yCords[i]-4;
		
		cout << "locals[" << i << "] = (";
		cout << locals[i].first << "," << locals[i].second;
		cout << ")" << endl;
	}
	
	mars.close();
}
