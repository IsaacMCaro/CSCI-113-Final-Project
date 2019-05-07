#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

//class for block object 
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

//global variable declarations
static string load = "100011";
static string store = "101011"; 
block cache[8][2];
vector<int> registers(32);
vector<int> mem(128);

// initializes memory blocks with index + 5
void initMem(){
	for (int i = 0; i < mem.size(); i++){
		mem.at(i) = i+5;

	}

}
//initializes registers with 0 value
void initRegisters(){
	for (int i = 0; i < registers.size(); i++){
		registers.at(i) = 0;

	}
}
//returns vector of instructions from input file
vector<string> getInstr(){
	//cout<< "getInstr" << endl;
	vector<string> instructions;
    ifstream input("/Users/isaacmcaro/Documents/GitHub/CSCI-113-Final-Project/instructions.txt");
    if(!input.is_open()){
        cout << "File Not Found.\n";
    }else{
        string lineRead;
        while(getline(input, lineRead)){
        	instructions.push_back(lineRead);
        }
    }
	return instructions;
}
//decode opcode from string
int decodeOP(string instr){
	string op  = instr.substr(0,6);
	if (op == load) return 35;
	else if (op == store) return 43;
	else return 69;
}
//decodes rs from string 
int decodeRS(string instr){
	string rs = instr.substr(6,5);
	return stoi(rs, nullptr, 2);
}
//decodes rt from string
int decodeRT(string instr){
	string rt = instr.substr(11,5);
	return stoi(rt, nullptr, 2);
}
//decode offset from string 
int decodeOF(string instr){
	string offset = instr.substr(16,16);
	return stoi(offset, nullptr, 2);
}
//caclculates tag based on word address
int getTag(int wordAddr){
	return wordAddr/8;
}
//caclculates set#(index) based on word address
int getSet(int wordAddr){
	return wordAddr%8;
}
//calculates byte address
int getByte(int rs, int offset){
	return rs + offset;

}
//calculates word adress
int getWord(int rs, int offset){
	return (rs + offset)/4;

}
//function for printing vector of instructions
void printList(vector<string> instructions){
	cout<<"printList"<<endl;
	for(int i = 0; i < instructions.size(); i++){
		cout<<"printList for loop"<<endl;
		cout<< i << " is the number!" << endl;
		cout<< instructions.at(i)<< endl;
	}
	cout<<"Done!" <<endl;
}

// returns the cache block from pair that matches set and tag of instruction
int checkCache(int set, int tag){
	if (cache[set][0].valid == 1 && cache[set][0].tag == tag) return 0;
	else if (cache[set][1].valid == 1 && cache[set][1].tag == tag) return 1;
	else return -1;
}
// calculates memory address of cache block(used in read miss)
int calcMemAddr(int tag, int set){
	return (tag * 8) + set;
}

//selects victim block based on history bit and then resets the history bits of the other block
int selectVictim(int set){
	if (cache[set][0].history == 1){
		cache[set][1].history = 1;
		cache[set][0].history = 0;
		return 1;

	}
	else{
		cache[set][0].history = 1;
		cache[set][1].history = 0;
		return 0;
	}
}

//displays memory and Register values in 8 bit binary
string memRegDisp(int dec){
	string bits  = "00000000";
    for (int pos = 7; pos >= 0; --pos)
    {
        if (dec % 2) 
            bits.at(pos) = '1';
        dec /= 2;
    }
    return bits;
}
//Displays data in 32 bit binary 
string dataBin(int dec){
	string bits  = "00000000000000000000000000000000";
    for (int pos = 31; pos >= 0; --pos)
    {
        if (dec % 2) 
            bits.at(pos) = '1';
        dec /= 2;
    }
    return bits;
}

//Displays tag value in 4 bit Binary 
string tagBin(int dec){
	string bits  = "0000";
    for (int pos = 3; pos >= 0; --pos)
    {
        if (dec % 2) 
            bits.at(pos) = '1';
        dec /= 2;
    }
    return bits;

}

//Displays Memory
void displayMem(){
	for(int i = 0; i <mem.size(); i++){
		cout<< "Mem Block " <<i<<": " << memRegDisp(mem.at(i)) << endl; 
	}

}

//Displays Registers
void displayReg(){
	for (int i = 16; i < 24; i++){
		cout<< "Reg $s" <<i-16<<": " << memRegDisp(registers.at(i)) << endl; 
	}
}

//Displays Cache
void displayCache(){
	for(int i = 0; i < 8; i++){
		cout<<"Set "<<i<< ": "<<"Block "<< 0<< ": "<< " | VALID = "<< cache[i][0].valid<< " | HIST = "<< cache[i][0].history << " | TAG = "<< tagBin(cache[i][0].tag) << " | DATA = "<< dataBin(cache[i][0].data) << " | | "<< "Block "<< 1<< ": "<< " | VALID = "<< cache[i][1].valid << " | HIST = "<< cache[i][1].history << " | TAG = "<< tagBin(cache[i][1].tag) <<  " | DATA = "<< dataBin(cache[i][1].data) <<" |"<< endl;

	}
}

//Runs Instruction
void runInstruction(string instr){
	int op = decodeOP(instr);
	int rs = decodeRS(instr);
	int rt = decodeRT(instr);
	int offset = decodeOF(instr);
	int byteAddr = getByte(rs, offset);
	int wordAddr = getWord(rs, offset);
	int set = getSet(wordAddr);
	int tag = getTag(wordAddr);

	int res = checkCache(set, tag); //will return block (0,1) if hit, -1 if miss
	if (res == -1){ // miss
		if (op == 35){ // read miss
			int vic = selectVictim(set);
			if (cache[set][vic].valid==1){ //step 2
				int addr = (cache[set][vic].tag * 8) + set;
				mem.at(addr) = cache[set][vic].data;

				}
			cache[set][vic].data = mem.at(wordAddr);//step 3
			cache[set][vic].valid = 1;//step 4
			cache[set][vic].tag = tag;//step 4
			registers.at(rt) = cache[set][vic].data; //step 5
			cout<<"Read Miss!"<<endl;

			}	
		else if (op == 43){ // write miss
			mem.at(wordAddr) = registers.at(rt);
			cout<<"Write Miss!"<<endl;

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
			cout<<"Read Hit!"<<endl;
		}
		else if (op == 43){ // write hit
			cache[set][res].data = registers.at(rt);
			cache[set][res].history = 1;
			cache[set][1-res].history = 0;	
			cout<<"Write Hit!"<<endl;
		}
	}
	else cout<<"Error running Instruction"<<endl;
	
}

int main(){
	vector<string> instructions = getInstr();
	initMem();
	initRegisters();
   	for (int i = 0; i<instructions.size(); i++){
   		runInstruction(instructions.at(i));
   	}
   	displayMem();
   	displayReg();
   	displayCache();

  	return 0;
}