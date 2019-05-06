#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

static string load = "100011";
static string store = "101011";


vector<string> getInstr(){
	cout<< "getInstr" << endl;
	vector<string> instructions;
    ifstream input("instructions.txt");
    if(!input.is_open()){
        cout << "File Not Found.\n\n";
    }else{
        string lineRead;
        while(getline(input, lineRead)){
        	instructions.push_back(lineRead);
        }
    }
	return instructions;
}

int decodeOP(string instr){
	string op  = instr.substr(0,6);
	cout<<op<<endl;
	if (op == load) return 35;
	else if (op == store) return 43;
	else return 69;
}

int decodeRS(string instr){
	string rs = instr.substr(6,5);
	return stoi(rs, nullptr, 2);
}

int decodeRT(string instr){
	string rt = instr.substr(11,5);
	return stoi(rt, nullptr, 2);
}

int decodeOF(string instr){
	string offset = instr.substr(16,16);
	cout<<offset<<endl;
	return stoi(offset, nullptr, 2);
}

int getTag(int wordAddr){
	return wordAddr/8;
}

int getSet(int wordAddr){
	return wordAddr%8;
}

void printList(vector<string> instructions){
	cout<<"printList"<<endl;
	for(int i = 0; i < instructions.size(); i++){
		cout<<"printList for loop"<<endl;
		cout<< i << " is the number!" << endl;
		cout<< instructions.at(i)<< endl;
	}
	cout<<"Done!" <<endl;
}

int main(){
	cout<< "Main Running!" << endl;
	vector<string> instructions = getInstr();
   	for(int i = 0; i < 24; i++){
   	cout<< decodeOF(instructions.at(i)) << endl;
   	}
   	cin.ignore();
  	return 0;
}