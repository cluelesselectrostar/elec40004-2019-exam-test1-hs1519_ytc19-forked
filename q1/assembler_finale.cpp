#include "mu0.hpp"
#include <iostream>
#include <map>
#include <sstream>
#include <algorithm>

using namespace std;

struct instruction {
	int address;
	string label;
	string content; //need to translate operand to hex during cout.

	void clear() {
		address = 0;
		label.clear();
		content.clear();
	}
};

string op_convert(const string &inp) {
	string opc;
			if(inp=="LDA"){
				opc="0";
			}
			if(inp=="STA"){
				opc="1";
			}
			if(inp=="ADD"){
				opc="2";
			}
			if(inp=="SUB"){
				opc="3";
			}
			if(inp=="JMP"){
				opc="4";
			}
			if(inp=="JGE"){
				opc="5";
			}
			if(inp=="JNE"){
				opc="6";
			}
			if(inp=="STP"){
				opc="7";
				//opr="000";
			}
			if(inp=="INP"){
				opc="8";
				//opr="000";
			}
			if(inp=="OUT"){
				opc="9";
				//opr="000";
			}
	return opc;
}

string signed_dec_to_hex (int num, int digits) {
	stringstream ss; 
    ss << hex << num; 
    string res = ss.str(); 
	while (res.size() < digits) {
		res.insert(0,"0");
	}
	return res;
}

int main()
{
	string inp;
	instruction current;
	vector<instruction> prog;
	int i=0; //address

	string opcode; 
	string operand;
	string content;

	map<string, int> label_map; //map of labels to addresses

	string out; //output string (to be modified)
	string tag; // key for finding label in map
	string is_label; //to determine size of label; if not 0, then it is a label.
	char out_opc; //opcode of output

	int tag_match; //output stage: integer address corresponding to label
	string tag_match_trans; // tag match translated to string


	//////////////////// INPUT LOOP ////////////////////
	while(1){
		cin >> inp;

		if(cin.fail()){
			break;
		}

		/*
		2 cases:
		1) label + content
		2) content (instruction + label/data)
		*/

		current.address = i;

		if(mu0_is_label_decl(inp)) { // CASE 1: label + content

			inp.pop_back(); //remove colon from label.
			current.label = inp;
			cin >> inp; //moving on to content.

			if(mu0_is_data(inp)) {
				current.content = signed_dec_to_hex(stoi(inp),4); //content has to be 4 digits max
			} else { //is instruction
				opcode = op_convert(inp);

				if (opcode == "7" || opcode == "8" || opcode == "9") { //no operand for STP, INP, OUT;
					current.content = opcode + "000"; //concatenate with trailing zeroes.
				} else {
					cin >> inp; //moving on to operand

					if(mu0_is_data(inp)){ //operand
						operand = signed_dec_to_hex(stoi(inp),3); //operand has to be 3 digits only
						current.content = opcode + operand;
					} else { //label
						current.content = opcode + inp; //content is now operand + label	
					}	
				}
			}
			
			label_map.insert({current.label, current.address});

		} else if(mu0_is_instruction(inp)){ //CASE 2: opcode + (and) label/data

			opcode = op_convert(inp);

			if (opcode == "7" || opcode == "8" || opcode == "9") { //no operand for STP, INP, OUT;
				current.content = opcode + "000";
			} else {
				cin >> inp;

				if(mu0_is_data(inp)){
					operand = signed_dec_to_hex(stoi(inp),3); //operand has to be 3 digits only.
					current.content = opcode + operand;
				} else {
					current.content = opcode + inp; //content is now operand + label	
				}	
			}

		} else {
			exit(1);
		}

		//push back entry into vector, clear instruction, and increment address for next instruction.
		prog.push_back(current);
		current.clear();
		i+=1;
	}

	//////////////////// OUTPUT STAGE ////////////////////

	//now print and match maps.
	for (int i=0; i<prog.size(); i++) {
		out = prog[i].content;
		is_label = prog[i].label;
		out_opc = out[0];

		//skip the following if statement, if it is a normal instruction (opcode + operand)
		if (is_label.size() != 0) { //if it is a label, print out content directly 

			while (out.size()>4) { //(but make sure 4 digits)
				out.erase(0,1);
			}

		} else { // if it is a combination of opcode and a label
			if (!isdigit(out[2])) {
				tag = out;
				tag.erase(0,1);

				if (label_map.find(tag) == label_map.end()) {
					//label does not exist, assume it has to exist.
				} else {
					tag_match = label_map.find(tag)->second; //match with address
					tag_match_trans = to_string(tag_match); //translate integer address to string
					while (tag_match_trans.size()<3) { //concatenate address with additional zeroes in front
						tag_match_trans = "0" + tag_match_trans;
					}
				}
				
				out = out_opc + tag_match_trans; //now its opcode + address (which is the operand)
			}
		}
	
    	transform(out.begin(), out.end(), out.begin(), ::toupper);

		cout << out << endl;
	}
			
	
}
