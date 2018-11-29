#include <iostream>
#include <fstream>
#include "stdlib.h"
#include <iomanip>
#include <cmath>

using namespace std;

int regs[32] = { 0 };
unsigned int pc = 0x0;

char memory[8 * 1024];	// only 8KB of memory located at address 0
void emitError(char *s)
{
	cout << s;
	exit(0);
}

void printPrefix(unsigned int instA, unsigned int instW)
{
	cout << "0x" << hex << std::setfill('0') << std::setw(8) << instA << "\t0x" << std::setw(8) << instW;
}
void instDecExec(unsigned int instWord)
{
	unsigned int rd, rs1, rs2, funct3, funct7, opcode;
	unsigned int I_imm, S_imm, SB_imm, U_imm, J_imm;
	//unsigned int address;
	unsigned int temp;
	unsigned int instPC = pc - 4;
	unsigned int sign;

	opcode = instWord & 0x0000007F;
	rd = (instWord >> 7) & 0x0000001F;
	funct3 = (instWord >> 12) & 0x00000007;
	rs1 = (instWord >> 15) & 0x0000001F;
	rs2 = (instWord >> 20) & 0x0000001F;
	funct7 = (instWord >> 25) & 0x00000007F;
	I_imm = ((instWord >> 20) & 0x7FF) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));//works
	S_imm = ((instWord >> 7) & 0x0000001F) | (((instWord >> 25) & 0x3F) << 7) | (((instWord >> 31) ? 0xFFFFF800 : 0x0));
	U_imm = ((instWord >> 12) & 0x7FFFF) | (((instWord >> 31) ? 0xFFF00000 : 0x0));
	J_imm = ((((instWord >> 21) & 0x3FF) << 1) + (((instWord >> 20) & 0x1) << 11) + (((instWord >> 12) & 0xFF) << 12) + (((instWord >> 31) ? 0xFFF00000 : 0x0)));
	SB_imm = ((((instWord >> 8) & 0xF) | ((instWord >> 25) & 0x3F) | ((instWord >> 7) & 0x1) | (((instWord >> 31) ? 0xFFFFF000 : 0x0))) << 1);
	printPrefix(instPC, instWord);

	if (opcode == 0x33) {		// R Instructions
		switch (funct3) {
			//cout << funct3 << endl;
		case 0x0: if (funct7 == 0x20) 
		{														//Sub
			cout << "\tSUB\tx" << dec << rd << ", x" << dec << rs1 << ", x" << dec << rs2 << "\n";
			regs[rd] = regs[rs1] - regs[rs2];
			cout <<dec<< regs[rd] << "\t" <<dec<< regs[rs1] << "\t" <<dec<< regs[rs2] << endl;
			system("pause");
			
		}

				  else {
					  cout << "\tADD\tx" << dec << rd << ", x" << dec << rs1 << ", x" << dec << rs2 << "\n";   //Add
					  regs[rd] = regs[rs1] + regs[rs2];
				  }
				  break;
		case 1:
		{
			cout << "\tSLL\tx" << dec <<  rd << ", x" << dec << rs1 << ", x" << dec <<  rs2 << "\n";		//SLL
			regs[rd] = regs[rs1] << (regs[rs2] & 0x001F);
		}break;
		case 2:
		{
			cout << "\tSLT\tx" << dec <<  rd << ", x" << dec <<  rs1 << ", x" << dec <<  rs2 << "\n";		//SLT
			regs[rd] = ((regs[rs1]) < (regs[rs2]));
			//cout<< (regs[rs1]) <<"\t" <<(regs[rs2])<<endl;
		}break;
		case 3:
		{
			cout << "\tSLTU\tx" << dec << rd << ", x" << dec <<  rs1 << ", x" << dec << rs2 << "\n";	//SLTU
			regs[rd] = unsigned(regs[rs1]) < unsigned(regs[rs2]);
		}break;
		case 4:
		{
			cout << "\tXOR\tx" << dec <<  rd << ", x" << dec << rs1 << ", x" << dec << rs2 << "\n";	//XOR
			regs[rd] = regs[rs1] ^ regs[rs2];
		}break;
		case 5:
		{
			if (funct7 == 0x0)
			{
				cout << "\tSRL\tx" << dec <<  rd << ", x" << dec << rs1 << ", x" << dec << rs2 << "\n";
				regs[rd] = regs[rs1] >> (unsigned int)regs[rs2];			//SRL
			}
			else
			{
				cout << "\tSRA\tx" << dec <<  rd << ", x" << dec <<  rs1 << ", x" << dec << rs2 << "\n";
				regs[rd] = (regs[rs1] >> regs[rs2]) | ((regs[rs1] >> 31) ? int((pow(2, 32) - 1) - (pow(2, 32 - regs[rs2]) - 1)) : 0x0);				//SRA
			}
		}break;
		case 7:
		{
			cout << "\tAND\tx" << dec <<  rd << ", x" << dec << rs1 << ", x" << dec << rs2 << "\n";	//AND
			regs[rd] = regs[rs1] & regs[rs2];
		} break;
		case 6:
		{
			cout << "\tOR\tx" << dec << rd << ", x" << dec << rs1 << ", x" << dec << rs2 << "\n";	//OR
			regs[rd] = regs[rs1] | regs[rs2];
		}
		break;
		default:
			cout << "\tUnkown R Instruction \n";
		}
	}
	else if (opcode == 0x13) {	// I instructions
		switch (funct3) {
		case 0:	cout << "\tADDI\tx" << dec <<  rd << ", x" << dec <<  rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			regs[rd] = regs[rs1] + (int)I_imm;
			break;
		case 1:	cout << "\tSLLI\tx" << dec << rd << ", x" << dec << rs1 << ", " << dec << rs2 << "\n";
			regs[rd] = regs[rs1] << rs2;
			//cout << regs[rs2] << endl;
			break;
		case 2:	cout << "\tSLTI\tx" << dec << rd << ", x" << dec << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			regs[rd] = regs[rs1] < (int)I_imm;
			break;
		case 3:	cout << "\tSLTIU\tx" << dec << rd << ", x" << dec << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			regs[rd] = unsigned(regs[rs1]) < unsigned((int)I_imm);
			break;
		case 5:	if (funct7 == 0) {
			cout << "\tSRLI\tx" << dec << rd << ", x" << dec << rs1 << ", " << hex << "0x" << rs2 << "\n";
			regs[rd] = unsigned(regs[rs1]) >> rs2;
		}
				else
				{
					cout << "\tSRAI\tx"  << dec << rd << ", x" << dec << rs1 << ", " << hex << "0x" << rs2 << "\n";
					regs[rd] = regs[rs1] >> rs2 | ((regs[rs1] >> 31) ? int((pow(2, 32) - 1) - (pow(2, 32 - rs2) - 1)) : 0x0);
				}
				break;
		case 4:	cout << "\tXORI\tx" << dec << rd << ", x" << dec << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			regs[rd] = regs[rs1] ^ (int)I_imm;
			break;
		case 6:	cout << "\tORI\tx" << dec << rd << ", x" << dec << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			regs[rd] = regs[rs1] | (int)I_imm;
			break;
		case 7:	cout << "\tANDI\tx" << dec << rd << ", x" << dec << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
			regs[rd] = regs[rs1] & (int)I_imm;
			break;
		default:
			cout << "\tUnkown I Instruction \n";
		}
	}
	else if (opcode == 0x23) {
		switch (funct3) {
		case 0://SB
			cout << "\tSB\tx"  << dec << rd << ", " << hex << "0x" << (int)S_imm << '(' << "x" << rs1 << ")\n";
			memory[S_imm + regs[rs1]] = regs[rs2];
			break;
		case 1:  //Shw
			cout << "\tSH\tx"  << dec << rd << ", " << hex << "0x" << (int)S_imm << '(' << "x" << rs1 << ")\n";
			while (S_imm + regs[rs1] % 2 == 0)
				S_imm++;
			temp = regs[rs1];
			memory[S_imm + rs1] = char(regs[rs2]);
			temp = temp >> 8;
			memory[S_imm + rs1+1] = char(temp);
			break;
		case 2:   //sw
			cout << "\tSW\tx" << dec << rs1 << ", " << hex << "0x" << (int)S_imm << '(' << "x" << rs1 << ")\n";
			while (S_imm + regs[rs1] % 4 != 0)
				S_imm++;
			temp = regs[rs1];
			memory[S_imm + regs[rs1]] = char(regs[rs2]);
			temp = temp >> 8;
			memory[S_imm + regs[rs1] + 1] = char(temp);
			temp = temp >> 8;
			memory[S_imm + regs[rs1] + 2] = char(temp);
			temp = temp >> 8;
			memory[S_imm + regs[rs1] + 3] = char(temp);
			break;
		}
	}
	else if (opcode == 0x3) {
		switch (funct3) {
		case 0: //lb
			cout << "\tLB\tx" << dec << rd << ", " << hex << "0x" << (int)I_imm << '(' << "x" << rs1 << ")\n";
			temp = 0;
			temp = temp | memory[regs[rs1] + I_imm];
			sign = temp >> 7;
			if (sign)
				regs[rd] = temp | 0xFFFFFF00;
			else
				regs[rd] = temp;
			break;
		case 1://lh
			cout << "\tLH\tx" << dec << rd << ", " << hex << "0x" << (int)I_imm << '(' << "x" << rs1 << ")\n";
			temp = 0;
			temp = temp | memory[regs[rs1] + I_imm + 1];
			temp = temp << 8;
			temp = temp | memory[regs[rs1] + I_imm];
			regs[rd] = temp;
			sign = temp >> 15;
			if (sign)
				regs[rd] = temp | 0xFFFF0000;
			else
				regs[rd] = temp;
			break;

		case 2:  //lw
			cout << "\tLW\tx" << dec << rd << ", " << hex << "0x" << (int)I_imm << '(' << "x" << rs1 << ")\n";
			temp = 0;
			temp = temp | memory[regs[rs1] + I_imm + 3];
			temp = temp << 8;
			temp = temp | memory[regs[rs1] + I_imm + 2];
			temp = temp << 8;
			temp = temp | memory[regs[rs1] + I_imm + 1];
			temp = temp << 8;
			temp = temp | memory[regs[rs1] + I_imm];
			regs[rd] = temp;
			break;
		case 4: //lbu
			cout << "\tLBU\tx" << dec << rd << ", " << hex << "0x" << (int)I_imm << '(' << "x" << rs1 << ")\n";
			temp = 0;
			temp = temp | memory[regs[rs1] + I_imm];
			regs[rd] = temp;
			break;
		case 5: //Lhu
			cout << "\tLHU\tx" << dec << rd << ", " << hex << "0x" << (int)I_imm << '(' << "x" << rs1 << ")\n";
			temp = 0;
			temp = temp | memory[regs[rs1] + I_imm + 1];
			temp = temp << 8;
			temp = temp | memory[regs[rs1] + I_imm];
			regs[rd] = temp;
			break;
		}
	}
	else if (opcode == 0x37)
	{
		cout << "\tLUI\tx" << dec << rd << ", " << hex << "0x" << (int)U_imm << "\n";
		regs[rd] = U_imm << 0xc;
	}
	else if (opcode == 0x63)//SB
		switch (funct3) {
		case 0://beq
			cout << "\tBEQ\tx" << dec << rs1 << ", x"  << dec << rs2 << ", " << hex << "0x" << (int)SB_imm << "\n";
		
			if (regs[rs1] == regs[rs2]) pc = instPC + (int)SB_imm;
			break;
		case 1://bne
			cout << "\tBNE\tx"  << dec << rs1 << ", x"  << dec << rs2 << ", " << hex << "0x" << (int)SB_imm << "\n";
			if (regs[rs1] != regs[rs2]) pc = instPC + (int)SB_imm;
			break;
		case 4://blt
			cout << "\tBLT\tx" << dec << rs1 << ", x" << dec << rs2 << ", " << hex << "0x" << (int)SB_imm << "\n";
			if (regs[rs1] < regs[rs2]) pc = instPC + (int)SB_imm;
			break;
		case 5://bge
			cout << "\tBGE\tx" << dec << rs1 << ", x"  << dec << rs2 << ", " << hex << "0x" << (int)SB_imm << "\n";
			
			if (regs[rs1] >= regs[rs2]) pc = instPC + (int)SB_imm;
			break;
		case 6://bltu
			cout << "\tBLTU\tx"  << dec << rs1 << ", x"  << dec << rs2 << ", " << hex << "0x" << (int)SB_imm << "\n";
			if (unsigned(regs[rs1]) < unsigned(regs[rs2])) pc = instPC + (int)SB_imm;
			break;
		case 7://bgeu
			cout << "\tBGEU\tx"  << dec << rs1 << ", x" << dec << rs2 << ", " << hex << "0x" << (int)SB_imm << "\n";
			if (regs[rs1] >= regs[rs2]) pc = instPC + (int)SB_imm;
			break;
		}
	else if (opcode == 0x6F)
	{   
		cout << "\tJAL\tx" << dec << rd << ", " << hex << "0x" << (int)J_imm << "\n";
		regs[rd] = pc+4;
		pc = instPC + (int)J_imm;
	}
	else  if (opcode == 0x67) 
	{   
		cout << "\tJALR\tx" << dec << rd << ", " <<  "x" << dec << rs1 << ", " << hex << "0x" << (int)I_imm << "\n";
		regs[rd] = pc; 
		pc = ((regs[rs1] + (int)I_imm) & 0xFFFFFFFE)-4;
	}
	else if (opcode == 0x17)
	{
		cout << "\tAUIPC\tx" << dec << rd << ", " << hex << "0x" << (int)U_imm << "\n";
		regs[rd] = pc + (int)(U_imm << 11);
	}
	else if (instWord == 0x73)
	{
		cout << "\tSCALL\n";
		if (regs[17] == 5)
			cin >>hex>> regs[17];
		else
			if (regs[17] == 1)
				cout << dec << regs[10] << endl;
			else
				if (regs[17] == 4)
				{
					int regtemp = regs[10];//temp
					while (memory[regs[regtemp]] != '\0')
					{
						cout << memory[regs[regtemp]];
						regtemp++;
					}
					cout << endl;
				}
				else if (regs[17] == 10)
					cout << "Terminating Program\n";
				else cout << "Invalid SCALL\n";
	}
	else {
		cout << "\tUnkown Instruction \n";
	}
	regs[0] = 0;
}

int main(int argc, char *argv[]) {

	unsigned int instWord = 0;
	ifstream inFile;
	ofstream outFile;
	if (argc<1) emitError("use: rv32i_sim <machine_code_file_name>\n");

	inFile.open(argv[1], ios::in | ios::binary | ios::ate);

	if (inFile.is_open())
	{
		int fsize = inFile.tellg();

		inFile.seekg(0, inFile.beg);
		if (!inFile.read((char *)memory, fsize)) emitError("Cannot read from input file\n");

		while (true) {
			instWord = (unsigned char)memory[pc] |
				(((unsigned char)memory[pc + 1]) << 8) |
				(((unsigned char)memory[pc + 2]) << 16) |
				(((unsigned char)memory[pc + 3]) << 24);
			pc += 4;
			//if (pc == 128) break;
						// stop when PC reached address 32
			instDecExec(instWord);
			if (instWord == 0x73 && regs[17] == 10) break;
		}

		// dump the registers
		for (int i = 0; i<32; i++)
			cout << "x" << dec << i << ": \t" << "0x" << hex << std::setfill('0') << std::setw(8) << regs[i] << "\n";

	}
	else emitError("Cannot access input file\n");
	system("pause");
}