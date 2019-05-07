#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

static string load = "100011";
static string store = "101011";

class block{
	public:
		int valid;
		int history;
		int tag;
		int data;
		block(){
			valid = 0;
			history = 0;
			tag = 0;
			data = 0;
		}
};

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

int getByte(int rs, int offset){
	return rs + offset;

}

int getWord(int rs, int offset){
	return (rs + offset)/4;

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

int checkCache(block *cache, int tag){
	if (cache[0].valid == 1 && cache[0].tag == tag) return 0;
	else if (cache[1].valid == 1 && cache[1].tag == tag) return 1;
	else return -1;
}

int calcMemAddr(int tag, int set){
	return (tag * 8) + set;
}

int selectVictim(block *cache){
	if (cache[0].history == 1){
		cache[1].history = 1;
		cache[0].history = 0;
		return 1;

	}
	else{
		cache[0].history = 1;
		cache[1].history = 0;
		return 0;
	}
}

void displayMem(vector<int> mem){
	for(int i = 0; i <128; i++){
		cout<< "Mem Block " <<i<<": " << mem.at(i) << endl; 
	}

}

void displayReg(vector<int> reg){
	for (int i = 0; i < 8; i++){
		cout<< "Reg $s" <<i<<": " << reg.at(i) << endl; 
	}
}

void displayCache(block **cache){
	for(int i = 0; i < 8; i++){
		cout<<"Cache Index "<<i<< ": "<<"Block "<< 0<< ": "<< " | VALID = "<< cache[i][0].valid << " | HIST = "<< cache[i][0].history << " | TAG = "<< cache[i][0].tag << " | "<< "Block "<< 1<< ": "<< " | VALID = "<< cache[i][1].valid << " | HIST = "<< cache[i][1].history << " | TAG = "<< cache[i][1].tag << " |"<< endl;

	}
}

void runInstruction(block** cache, vector<int> registers, vector<int> mem, string instr){
	int op = decodeOP(instr);
	int rs = decodeRS(instr);
	int rt = decodeRT(instr);
	int offset = decodeOF(instr);
	int byteAddr = getByte(rs, offset);
	int wordAddr = getWord(rs, offset);
	int set = getSet(wordAddr);
	int tag = getTag(wordAddr);

	int res = checkCache(cache[set], tag); //will return block if hit, -1 if miss
	if (res == -1){ // miss
		if (op == 35){ // read miss
			int vic = selectVictim(cache[set]);
			if (cache[set][vic].valid==1){ //step 2
				int addr = (cache[set][vic].tag * 8) + set;
				mem.at(addr) = cache[set][vic].data;

				}
			cache[set][vic].data = mem.at(wordAddr);//step 3
			cache[set][vic].valid = 1;//step 4
			cache[set][vic].tag = tag;//step 4
			registers.at(rt) = cache[set][vic].data; //step 5

			}	
		else if (op == 43){ // write miss
			mem.at(wordAddr) = registers.at(rt);

		}
	
		else {
			cout<<"error within checkCache"<<endl;
		}
	}
	else if (res ==1 || res == 0){ //read or write hit
		if (op == 35){ // read hit
			registers.at(rt) = cache[set][res].data;
			cache[set][res].history = 1;
			cache[set][1-res].history = 0;
		}
		else if (op == 43){ // write hit
			cache[set][res].data = registers.at(rt);
			cache[set][res].history = 1;
			cache[set][1-res].history = 0;	

		}
		cout<<"Hit!"<<endl;
	}
	
}

int main(){
	cout<< "Main Running!" << endl;
	vector<string> instructions = getInstr();
	block cache[8][2];
	vector<int> registers(32);
	vector<int> mem(128);
   	for (int i = 0; i<instructions.size(); i++){
   		runInstruction(&cache, &registers, &mem, instructions.at(i));
   	}
   	displayReg(registers);
   	displayMem(mem);
   	displayCache(cache);

  	return 0;
}