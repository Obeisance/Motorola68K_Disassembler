//assemblyInstructions.cpp
#include <string>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "assemblyInstructions.h"
#include "stringAndNumberHandlers.h"
using namespace std;

int constructInstructionString(string commandArrayString, string commandString,
		string &outputString) {
//input a string with the short form of a command which indicates the correct form
//of the commandString; outputs a changed outputString with the written
//version of the command as well as an int which indicates the total number
//of instruction bytes
	int correctForm = 0; //0 means incorrect formatting

	//ABCD
	if (commandArrayString == "ABCD") {
		//add decimal with extend
		//ABCD Dy,Dx or ABCD -(Ay),-(Ax)
		//15 14 13 12 11 10 9  8 7 6 5 4   3   2 1 0
		//1  1  0  0  Reg. Rx  1 0 0 0 0  R/M  reg.Ry
		//if R/M = 0, Data register, else address register
		string regX = retrieveBitsFromInstruction(11, 3, commandString);
		regX = stringBitsToNumber(regX);
		string regY = retrieveBitsFromInstruction(2, 3, commandString);
		regY = stringBitsToNumber(regY);
		string RM = retrieveBitsFromInstruction(3, 1, commandString);
		if (RM == "0") {
			outputString.append("ABCD D");
			outputString.append(regY);
			outputString.append(",D");
			outputString.append(regX);
		} else {
			outputString.append("ABCD -(A");
			outputString.append(regY);
			outputString.append("),-(A");
			outputString.append(regX);
			outputString.append(")");
		}

		outputString.push_back('\t');
		outputString.append(
				"; Source(base10) + Destination(base10) + X(extendBit) -> Destination");
		outputString.push_back('\t');
		outputString.append(
				"the extend bit matches the 'carry on' bit to allow for binary addition");
		correctForm = 2;		//two bytes in instruction
		//ABCD
	} else if (commandArrayString == "ADD") {
		//ADD
		//add byte, word or long
		//ADD <ea>, Dn, or ADD Dn,<ea>
		//15 14 13 12 11 10 9   8 7 6  5 4 3  2 1 0
		//1  1  0  1  register  opmode  mode   reg
		//effective address bits 5-0
		//register is any of the 8 Data registers,
		//opmode
		//byte  word  long         operation
		//000   001   010     <ea> + Dn -> Dn
		//100   101   110     Dn  + <ea>-> <ea>
		//effective address field: if <ea> is source
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An*            001      An
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          111      100
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//effective address field: if <ea> is destination
		//addressing mode    mode    register
		//    Dn             ---      --
		//    An*            ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		//*word and long only

		//add the base command to the string
		string baseCommand = "ADD";
		outputString.append(baseCommand);

		//collect the portions of the command which determine
		//mode and addresses
		string regist = retrieveBitsFromInstruction(11, 3, commandString);
		regist = stringBitsToNumber(regist);
		string opmode = retrieveBitsFromInstruction(8, 3, commandString);
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (opmode == "000" || opmode == "100") {
			outputString.append(".B");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'B');
		} else if (opmode == "001" || opmode == "101") {
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'W');
		} else if (opmode == "010" || opmode == "110") {
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'L');
		} else {
			baseInstructionByteNumber = 0;//set this to 0 if no valid entries are made
		}

		if (opmode == "000" || opmode == "001" || opmode == "010") {
			outputString.append(" ");
			outputString.append(effectiveAddress);
			outputString.append(",D");
			outputString.append(regist);
		} else if (opmode == "100" || opmode == "101" || opmode == "110") {
			outputString.append(" D");
			outputString.append(regist);
			outputString.append(",");
			outputString.append(effectiveAddress);
		}

		//denote special cases where command may be invalid
		if (EAmode == "001"
				&& (opmode == "000" || opmode == "100" || opmode == "101"
						|| opmode == "110")) {
			//An is only word or long, and only when EA is a source operand
			baseInstructionByteNumber = 0;
		} else if ((EAmode == "000" || (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011"))
				&& (opmode == "100" || opmode == "101" || opmode == "110")) {
			//Dn, #<xxx>, (d16,PC), (d8,PC,Xn) are only when EA is a source operand
			baseInstructionByteNumber = 0;
		}
		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}
		outputString.push_back('\t');
		outputString.append("; source + destination -> destination");
		//ADD
	} else if (commandArrayString == "ADDA") {
		//ADDA
		//add address, word or long
		//ADDA <ea>, An,
		//15 14 13 12 11 10 9   8 7 6  5 4 3  2 1 0
		//1  1  0  1  register  opmode  mode   reg
		//effective address bits 5-0
		//register is any of the 8 address registers,
		//opmode
		//word  long         operation
		//011   111     <ea> + An -> An
		//effective address field: <ea> is source
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             001      An
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          111      100
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//base of instruction
		outputString.append("ADDA");

		//collect the portions of the command which determine
		//mode and addresses
		string regist = retrieveBitsFromInstruction(11, 3, commandString);
		regist = stringBitsToNumber(regist);
		string opmode = retrieveBitsFromInstruction(8, 3, commandString);
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (opmode == "011") {
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'W');
		} else if (opmode == "111") {
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'L');
		} else {
			baseInstructionByteNumber = 0;//set this to 0 if no valid entries are made
		}

		outputString.append(" ");
		outputString.append(effectiveAddress);
		outputString.append(",A");
		outputString.append(regist);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append("; source + destination -> destination");
		//ADDA
	} else if (commandArrayString == "ADDI") {
		//ADDI
		//add immediate byte, word or long
		//ADDI #<data>,<ea>
		//15 14 13 12 11 10 9  8  7 6   5 4 3  2 1 0
		//0  0  0  0  0  1  1  0  size   mode   reg
		//effective address bits 5-0
		//size
		//byte  word  long
		//00     01    10
		//effective address field: <ea> is destination
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---

		//base of instruction
		outputString.append("ADDI");

		//collect the portions of the command which determine
		//mode and addresses
		string size = retrieveBitsFromInstruction(7, 2, commandString);
		string data = "";
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (size == "00") {
			outputString.append(".B");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'B', -16);
			data = retrieveBitsFromInstruction(-9, 8, commandString);
			data = stringBitsToNumber(data); //convert data to decimal
			baseInstructionByteNumber = 4;
		} else if (size == "01") {
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'W', -16);
			data = retrieveBitsFromInstruction(-1, 16, commandString);
			data = stringBitsToNumber(data); //convert data to decimal
			baseInstructionByteNumber = 4;
		} else if (size == "10") {
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'L', -32);
			data = retrieveBitsFromInstruction(-1, 32, commandString);
			data = stringBitsToNumber(data); //convert data to decimal
			baseInstructionByteNumber = 6;
		} else {
			baseInstructionByteNumber = 0; //set this to 0 if no valid entries are made
		}

		//account for special cases
		if (EAmode == "001" || (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			//cannot use An, #<data>, (d16,PC), (d8,PC,Xn) or (bd,PC,Xn) as EA
			baseInstructionByteNumber = 0;
		}

		outputString.append(" #");
		outputString.append(data);
		outputString.append(",");
		outputString.append(effectiveAddress);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append("; immediate data + destination -> destination");
		//ADDI
	} else if (commandArrayString == "ADDQ") {
		//ADDQ
		//add immediate byte, word or long
		//ADDQ #<data>,<ea>
		//15 14 13 12 11 10 9  8  7 6   5 4 3  2 1 0
		//0  1  0  1    data   0  size   mode   reg
		//effective address bits 5-0
		//size
		//byte  word  long
		//00     01    10
		//effective address field: <ea> is destination
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             001      An
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---

		//base of instruction
		outputString.append("ADDQ");

		//collect the portions of the command which determine
		//mode and addresses
		string size = retrieveBitsFromInstruction(7, 2, commandString);
		string data = retrieveBitsFromInstruction(11, 3, commandString);
		data = stringBitsToNumber(data); //convert data to decimal
		if (data == "0") {
			data = "8";
		}
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (size == "00") {
			outputString.append(".B");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'B');
		} else if (size == "01") {
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'W');
		} else if (size == "10") {
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'L');
		} else {
			baseInstructionByteNumber = 0; //set this to 0 if no valid entries are made
		}

		//account for special cases
		if ((EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			//cannot use #<data>, (d16,PC), (d8,PC,Xn) or (bd,PC,Xn) as EA
			baseInstructionByteNumber = 0;
		}

		outputString.append(" #");
		outputString.append(data);
		outputString.append(",");
		outputString.append(effectiveAddress);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append("; immediate data + destination -> destination");
		//ADDQ
	} else if (commandArrayString == "ADDX") {
		//ADDX
		//add byte, word or long in register
		//ADDX Dy,Dx or ADDX -(Ay),-(Ax)
		//15 14 13 12 11 10 9  8  7 6  5 4  3   2 1 0
		//1  1  0  1    regX   1  size 0 0 R/M  regY
		//R/M = 0 -> data registers; = 1 -> address registers
		//size
		//byte  word  long
		//00     01    10

		//base of instruction
		outputString.append("ADDX");

		//collect the portions of the command which determine
		//mode and addresses
		string size = retrieveBitsFromInstruction(7, 2, commandString);
		string regX = retrieveBitsFromInstruction(11, 3, commandString);
		regX = stringBitsToNumber(regX); //convert data to decimal
		string regY = retrieveBitsFromInstruction(2, 3, commandString);
		regY = stringBitsToNumber(regY); //convert data to decimal
		string RM = retrieveBitsFromInstruction(3, 1, commandString);

		int baseInstructionByteNumber = 2;
		//add the size of the base command to the string
		if (size == "00") {
			outputString.append(".B");
		} else if (size == "01") {
			outputString.append(".W");
		} else if (size == "10") {
			outputString.append(".L");
		} else {
			baseInstructionByteNumber = 0; //set this to 0 if no valid entries are made
		}

		outputString.append(" ");
		if (RM == "0") {
			//data registers
			outputString.append("D");
			outputString.append(regY);
			outputString.append(",D");
			outputString.append(regX);
		} else if (RM == "1") {
			outputString.append("-(A");
			outputString.append(regY);
			outputString.append("),-(A");
			outputString.append(regX);
			outputString.append(")");
		}

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append("; source + destination + X -> destination");
		//ADDX
	} else if (commandArrayString == "AND") {
		//AND
		//logical compare source and destination
		//AND <EA>,Dn or AND Dn,<EA>
		//15 14 13 12 11 10 9  8 7 6  5 4 3   2 1 0
		//1  1  0  0    reg   opmode  EAmode  EAreg
		//effective address bits 5-0
		//opmode
		//byte  word  long      operation
		//000    001   010      <EA> && Dn -> Dn
		//100    101   110      Dn && <EA> -> <EA>

		//effective address field: if <ea> is source
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          111      100
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//effective address field: if <ea> is destination
		//addressing mode    mode    register
		//    Dn             ---      --
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---

		//add the base command to the string
		string baseCommand = "AND";
		outputString.append(baseCommand);

		//collect the portions of the command which determine
		//mode and addresses
		string regist = retrieveBitsFromInstruction(11, 3, commandString);
		regist = stringBitsToNumber(regist);
		string opmode = retrieveBitsFromInstruction(8, 3, commandString);
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (opmode == "000" || opmode == "100") {
			outputString.append(".B");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'B');
		} else if (opmode == "001" || opmode == "101") {
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'W');
		} else if (opmode == "010" || opmode == "110") {
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'L');
		} else {
			baseInstructionByteNumber = 0;//set this to 0 if no valid entries are made
		}

		if (opmode == "000" || opmode == "001" || opmode == "010") {
			outputString.append(" ");
			outputString.append(effectiveAddress);
			outputString.append(",D");
			outputString.append(regist);
		} else if (opmode == "100" || opmode == "101" || opmode == "110") {
			outputString.append(" D");
			outputString.append(regist);
			outputString.append(",");
			outputString.append(effectiveAddress);
		}

		//denote special cases where command may be invalid
		if (EAmode == "001") {
			//An cannot be <EA>
			baseInstructionByteNumber = 0;
		} else if ((EAmode == "000" || (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011"))
				&& (opmode == "100" || opmode == "101" || opmode == "110")) {
			//Dn, #<xxx>, (d16,PC), (d8,PC,Xn) are only when EA is a source operand
			baseInstructionByteNumber = 0;
		}

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}
		outputString.push_back('\t');
		outputString.append("; source && destination -> destination");
		//AND
	} else if (commandArrayString == "ANDI") {
		//ANDI
		//compare immediate byte, word or long with <EA>
		//ANDI #<data>,<ea>
		//15 14 13 12 11 10 9  8  7 6   5 4 3  2 1 0
		//0  0  0  0  0  0  1  0  size   mode   reg
		//effective address bits 5-0
		//size
		//byte  word  long
		//00     01    10
		//effective address field: <ea> is destination
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---

		//base of instruction
		outputString.append("ANDI");

		//collect the portions of the command which determine
		//mode and addresses
		string size = retrieveBitsFromInstruction(7, 2, commandString);
		string data = "";
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (size == "00") {
			outputString.append(".B");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'B', -16);
			data = retrieveBitsFromInstruction(-9, 8, commandString);
			data = stringBitsToNumber(data); //convert data to decimal
			baseInstructionByteNumber = 4;
		} else if (size == "01") {
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'W', -16);
			data = retrieveBitsFromInstruction(-1, 16, commandString);
			data = stringBitsToNumber(data); //convert data to decimal
			baseInstructionByteNumber = 4;
		} else if (size == "10") {
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'L', -32);
			data = retrieveBitsFromInstruction(-1, 32, commandString);
			data = stringBitsToNumber(data); //convert data to decimal
			baseInstructionByteNumber = 6;
		} else {
			baseInstructionByteNumber = 0; //set this to 0 if no valid entries are made
		}

		//account for special cases
		if (EAmode == "001" || (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			//cannot use An, #<data>, (d16,PC), (d8,PC,Xn) or (bd,PC,Xn) as EA
			baseInstructionByteNumber = 0;
		}

		outputString.append(" #");
		outputString.append(data);
		outputString.append(",");
		outputString.append(effectiveAddress);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append("; immediate data && destination -> destination");
		//ANDI
	} else if (commandArrayString == "ANDItoCCR") {
		//ANDI to CCR
		//compare immediate byte, word or long with CCR
		//ANDI #<data>,CCR
		//15 14 13 12 11 10 9  8  7 6  5 4 3 2 1 0
		//0  0  0  0  0  0  1  0  0 0  1 1 1 1 0 0

		//base of instruction
		outputString.append("ANDI");

		//collect the portions of the command which determine
		//mode and addresses
		string data = retrieveBitsFromInstruction(-9, 8, commandString);
		data = stringBitsToNumber(data); //convert data to decimal

		int baseInstructionByteNumber = 4;

		outputString.append(" #");
		outputString.append(data);
		outputString.append(",CCR");

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append("; immediate data && CCR -> CCR");
		//ANDI to CCR
	} else if (commandArrayString == "ASL_ASR_sizeSelect") {
		//ASL,ASR register shift
		//destination shifted by byte, word or long
		//ASd Dx,Dy or ASd #<data>,Dy or ASd<EA>
		//d is L or R
		//15 14 13 12 11 10 9   8   7 6   5   4 3  2 1 0
		//1  1  1  0   count?  D/R  size  i/r 0 0  Regis
		//            register
		//
		//i/r = 0 : count/Register contains shift count
		//1-7 or 0 = 8
		//i/r = 1 : count/Register contains data register
		//which contains shift count, modulo 64
		//
		//D/R specifies direction of shift: 0 = right, 1 = left
		//
		//size
		//byte   word   long
		// 00     01     10

		//base of instruction
		outputString.append("AS");

		//collect the portions of the command which determine
		//mode and addresses
		string count_Register = retrieveBitsFromInstruction(11, 3,
				commandString);
		count_Register = stringBitsToNumber(count_Register); //convert data to decimal
		string DR = retrieveBitsFromInstruction(8, 1, commandString);
		string size = retrieveBitsFromInstruction(7, 2, commandString);
		string ir = retrieveBitsFromInstruction(5, 1, commandString);
		string regis = retrieveBitsFromInstruction(2, 3, commandString);
		regis = stringBitsToNumber(regis); //convert data to decimal

		int baseInstructionByteNumber = 2;

		if (DR == "0") {
			outputString.append("R");
		} else if (DR == "1") {
			outputString.append("L");
		}

		if (size == "00") {
			outputString.append(".B");
		} else if (size == "01") {
			outputString.append(".W");
		} else if (size == "10") {
			outputString.append(".L");
		} else {
			baseInstructionByteNumber = 0;
		}

		outputString.append(" ");
		if (ir == "0") {
			//immediate count
			outputString.append("#");
			if (count_Register == "0") {
				count_Register = "8";
			}
			outputString.append(count_Register);
			outputString.append(",D");
			outputString.append(regis);
		} else if (ir == "1") {
			//register shift count
			outputString.append("D");
			outputString.append(count_Register);
			outputString.append(",D");
			outputString.append(regis);
		}

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append("; Destination shifted by Count -> Destination");
		//ASL,ASR register shift
	} else if (commandArrayString == "ASL_ASR") {
		//ASL,ASR memory shifts
		//destination shifted by byte, word or long
		//ASd Dx,Dy or ASd #<data>,Dy or ASd <EA>
		//d is L or R
		//15 14 13 12 11 10 9   8   7 6   5 4 3  2 1 0
		//1  1  1  0  0  0  0  D/R  1 1  EAmode  EAregi
		//
		//D/R specifies direction of shift: 0 = right, 1 = left
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             ---      --
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---

		//base of instruction
		outputString.append("AS");

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string DR = retrieveBitsFromInstruction(8, 1, commandString);

		int baseInstructionByteNumber = 2;

		if (DR == "0") {
			outputString.append("R");
		} else if (DR == "1") {
			outputString.append("L");
		}

		int extensionBytes = 0;
		string effectiveAddress = "";
		extensionBytes = assembleEffectiveAddress(commandString,
				effectiveAddress, 'B');
		outputString.append(" ");

		//special cases where format is incorrect
		if (EAmode == "000" || EAmode == "001"
				|| (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			baseInstructionByteNumber = 0;
		}

		outputString.append(effectiveAddress);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append("; Destination shifted by 1 bit -> Destination");
		//ASL,ASR memory shift
	} else if (commandArrayString == "Bcc") {
		//Bcc
		//if condition true, update PC with displacement
		//Bcc <label>
		//15 14 13 12  11 10 9 8   7 6 5 4 3 2 1 0
		//0  1  1  0   condition   8-bit displacement
		//if 8-bit == $00, use next 16 bits
		//if 8-bit == $FF, use next 32 bits - only for MC68020,68030,68040
		//but the CPU32 is similar to the 68020, so we'll include this interpretation
		//conditional tests
		//Mnemonic     condition   encoding
		//  T*            True       0000
		//  F*            False      0001
		//  HI            High       0010
		//  LS        Low or same    0011
		// CC(HI)     carry clear    0100
		// CS(LO)      Carry set     0101
		//  NE          not equal    0110
		//  EQ            equal      0111
		//  VC       overflow clear  1000
		//  VS       Overflow set    1001
		//  PL             Plus      1010
		//  MI            Minus		 1011
		//  GE      Greater or equal 1100
		//  LT          Less than    1101
		//  GT        greater than   1110
		//  LE       Less or equal   1111
		//* not available with Bcc
		//collect the bits to determine the instruction
		string conditionCode = retrieveBitsFromInstruction(11, 4,
				commandString);
		string condition = "";
		string displacement = retrieveBitsFromInstruction(7, 8, commandString);
		string conditionCodeSymbol = "";

		int baseInstructionByteNumber = 2;

		//base of command
		outputString.append("B");
		//determine condition
		if (conditionCode == "0010") {
			condition = "HI";
			conditionCodeSymbol = "HI";
		} else if (conditionCode == "0011") {
			condition = "Low or same";
			conditionCodeSymbol = "LS";
		} else if (conditionCode == "0100") {
			condition = "Carry clear";
			conditionCodeSymbol = "CC(HI)";
		} else if (conditionCode == "0101") {
			condition = "Carry set";
			conditionCodeSymbol = "CS(LO)";
		} else if (conditionCode == "0110") {
			condition = "Not equal";
			conditionCodeSymbol = "NE";
		} else if (conditionCode == "0111") {
			condition = "Equal";
			conditionCodeSymbol = "EQ";
		} else if (conditionCode == "1000") {
			condition = "Overflow clear";
			conditionCodeSymbol = "VC";
		} else if (conditionCode == "1001") {
			condition = "Overflow set";
			conditionCodeSymbol = "VS";
		} else if (conditionCode == "1010") {
			condition = "Plus";
			conditionCodeSymbol = "PL";
		} else if (conditionCode == "1011") {
			condition = "Minus";
			conditionCodeSymbol = "MI";
		} else if (conditionCode == "1100") {
			condition = "Greater or equal";
			conditionCodeSymbol = "GE";
		} else if (conditionCode == "1101") {
			condition = "Less than";
			conditionCodeSymbol = "LT";
		} else if (conditionCode == "1110") {
			condition = "Greater than";
			conditionCodeSymbol = "GT";
		} else if (conditionCode == "1111") {
			condition = "Less or equal";
			conditionCodeSymbol = "LE";
		} else {
			baseInstructionByteNumber = 0;
		}


		if(baseInstructionByteNumber != 0)
		{
			outputString.append(conditionCodeSymbol);

			//determine size
			if (displacement == "00000000" && baseInstructionByteNumber > 0) {
				displacement = retrieveBitsFromInstruction(-1, 16, commandString);
				outputString.append(".W");
				baseInstructionByteNumber = 4;
			} else if (displacement == "11111111") {
				displacement = retrieveBitsFromInstruction(-1, 32, commandString);
				outputString.append(".L");
				baseInstructionByteNumber = 6;
			} else if (baseInstructionByteNumber > 0) {
				outputString.append(".B");
			}
			string complement = twosComplement(displacement);
			displacement = stringBitsToNumber(displacement); //convert data to decimal
			displacement = stringNumber_to_hex(displacement); //convert decimal to hex

			outputString.append(" $");
			outputString.append(displacement);

			if (baseInstructionByteNumber != 0) {
				correctForm = baseInstructionByteNumber;
			}

			outputString.push_back('\t');
			outputString.append("; if condition (");
			outputString.append(condition);
			outputString.append(") true, then PC + 2 + dn -> PC, twos complement = ");
			outputString.append(complement);
			outputString.push_back(' ');
		}
		//Bcc
	} else if (commandArrayString == "BCHG_bit") {
		//BCHG bit number dynamic in register
		//TEST(<number> of destination) -> Z;
		//TEST(<number> of destination) -> <bit number> of destination
		//BCHG Dn,<EA> or BCHG #<data>,<EA>
		//15 14 13 12 11 10 9   8 7 6   5 4 3   2 1 0
		//0  0  0  0  register  1 0 1   EAmode  EAregi
		//
		//effective address field: <ea> is destination
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---
		//Dn = long, all others = byte
		//base of instruction
		outputString.append("BCHG");

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string regis = retrieveBitsFromInstruction(11, 3, commandString);
		regis = stringBitsToNumber(regis);

		int baseInstructionByteNumber = 2;
		int extensionBytes = 0;
		string effectiveAddress = "";
		if (EAmode == "000") {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'L');
			outputString.append(".L");
		} else {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'B');
			outputString.append(".B");
		}

		outputString.append(" ");

		//special cases where format is incorrect
		if (EAmode == "001" || (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			baseInstructionByteNumber = 0;
		}
		outputString.append("D");
		outputString.append(regis);
		outputString.append(",");
		outputString.append(effectiveAddress);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; Tests a bit in the destination specified by the bit number in the register-> inverts the bit in the destination");
		//BCHG
	} else if (commandArrayString == "BCHG") {
		//BCHG with immediate bit number static
		//TEST(<number> of destination) -> Z;
		//TEST(<number> of destination) -> <bit number> of destination
		//BCHG Dn,<EA> or BCHG #<data>,<EA>
		//15 14 13 12 11 10 9 8 7 6   5 4 3   2 1 0
		//0  0  0  0  1  0  0 0 0 1   EAmode  EAregi
		//
		//effective address field: <ea> is destination
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---
		//Dn = long, all others = byte
		//base of instruction
		outputString.append("BCHG");

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string bitNumber = retrieveBitsFromInstruction(-9, 8, commandString);
		bitNumber = stringBitsToNumber(bitNumber);

		int baseInstructionByteNumber = 4;
		int extensionBytes = 0;
		string effectiveAddress = "";
		if (EAmode == "000") {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'L', -16);
			outputString.append(".L");
		} else {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'B', -16);
			outputString.append(".B");
		}

		outputString.append(" ");

		//special cases where format is incorrect
		if (EAmode == "001" || (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			baseInstructionByteNumber = 0;
		}
		outputString.append("#");
		outputString.append(bitNumber);
		outputString.append(",");
		outputString.append(effectiveAddress);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; Tests a bit in the destination specified by the #<bit number> -> inverts the bit in the destination");
		//BCHG
	} else if (commandArrayString == "BCLR_bit") {
		//BCLR, bit number dynamic specified in register
		//TEST(<number> of destination) -> Z;
		//0 -> <bit number> of destination
		//BCLR Dn,<EA> or BCLR #<data>,<EA>
		//15 14 13 12 11 10 9   8 7 6   5 4 3   2 1 0
		//0  0  0  0  register  1 1 0   EAmode  EAregi
		//
		//effective address field: <ea> is destination
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---
		//Dn = long, all others = byte
		//base of instruction
		outputString.append("BCLR");

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string regis = retrieveBitsFromInstruction(11, 3, commandString);
		regis = stringBitsToNumber(regis);

		int baseInstructionByteNumber = 2;
		int extensionBytes = 0;
		string effectiveAddress = "";
		if (EAmode == "000") {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'L');
			outputString.append(".L");
		} else {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'B');
			outputString.append(".B");
		}

		outputString.append(" ");

		//special cases where format is incorrect
		if (EAmode == "001" || (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			baseInstructionByteNumber = 0;
		}
		outputString.append("D");
		outputString.append(regis);
		outputString.append(",");
		outputString.append(effectiveAddress);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; Tests a bit in the destination specified by the bit number in the register-> sets the bit in the destination to 0");
		//BCLR
	} else if (commandArrayString == "BCLR") {
		//BCLR, bit number static follows command
		//TEST(<number> of destination) -> Z;
		//0 -> <bit number> of destination
		//BCLR Dn,<EA> or BCLR #<data>,<EA>
		//15 14 13 12 11 10 9 8 7 6   5 4 3   2 1 0
		//0  0  0  0  1  0  0 0 1 0   EAmode  EAregi
		//
		//effective address field: <ea> is destination
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---
		//Dn = long, all others = byte
		//base of instruction
		outputString.append("BCLR");

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string bitNumber = retrieveBitsFromInstruction(-9, 8, commandString);
		bitNumber = stringBitsToNumber(bitNumber);

		int baseInstructionByteNumber = 4;
		int extensionBytes = 0;
		string effectiveAddress = "";
		if (EAmode == "000") {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'L', -16);
			outputString.append(".L");
		} else {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'B', -16);
			outputString.append(".B");
		}

		outputString.append(" ");

		//special cases where format is incorrect
		if (EAmode == "001" || (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			baseInstructionByteNumber = 0;
		}
		outputString.append("#");
		outputString.append(bitNumber);
		outputString.append(",");
		outputString.append(effectiveAddress);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; Tests a bit in the destination specified by the #<bit number> -> sets the bit in the destination to 0");
		//BCLR
	} else if (commandArrayString == "BKPT") {
		//BKPT
		//run breakpoint acknowledge cycle
		//TRAP as illegal instruction
		//BKPT #<data>
		//15 14 13 12 11 10 9 8 7 6 5 4 3  2 1 0
		//0  1  0  0  1  0  0 0 0 1 0 0 1  Vector
		//vector contains immediate data of 0-7, the breakpoint number
		string vector = retrieveBitsFromInstruction(2, 3, commandString);
		vector = stringBitsToNumber(vector);

		outputString.append("BKPT #");
		outputString.append(vector);

		correctForm = 2;

		outputString.push_back('\t');
		outputString.append(
				"; Run breakpoint ack. cycle; TRAP as illegal instruction");
		//BKPT
	} else if (commandArrayString == "BRA") {
		//BRA
		//PC + Dn -> PC
		//BRA <label>
		//15 14 13 12 11 10 9 8   7 6 5 4 3 2 1 0
		//0  1  1  0  0  0  0 0  8-bit displacement
		// if 8-bit == $00, use next 16 bits
		// if 8-bit == $FF, use next 32 bits, only MC68020, 68030, 68040
		//since the CPU32 is similar to the 68020, we'll include this interpretation
		//collect the bits to determine the instruction
		string displacement = retrieveBitsFromInstruction(7, 8, commandString);

		int baseInstructionByteNumber = 2;

		//base of command
		outputString.append("BRA");

		//determine size
		if (displacement == "00000000") {
			displacement = retrieveBitsFromInstruction(-1, 16, commandString);
			outputString.append(".W");
			baseInstructionByteNumber = 4;
		} else if (displacement == "11111111") {
			displacement = retrieveBitsFromInstruction(-1, 32, commandString);
			outputString.append(".L");
			baseInstructionByteNumber = 6;
		} else {
			outputString.append(".B");
		}
		string complement = twosComplement(displacement); //get the twos compliment
		displacement = stringBitsToNumber(displacement); //convert data to decimal
		displacement = stringNumber_to_hex(displacement); //convert decimal to hex

		outputString.append(" $");
		outputString.append(displacement);

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append("; PC + 2 + dn(twos complement = ");
		outputString.append(complement);
		outputString.append(" ) -> PC");
		//BRA
	} else if (commandArrayString == "BSET_bit") {
		//BSET bit number dynamic, specified in a register
		//TEST(<bit number> of destination) -> Z;
		//1 -> <bit number> of destination
		//BSET Dn,<EA> or BSET #<data>,<EA>
		//15 14 13 12 11 10 9   8 7 6   5 4 3   2 1 0
		//0  0  0  0  register  1 1 1   EAmode  EAregi
		//
		//effective address field: <ea> is destination
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---
		//Dn = long, all others = byte
		//base of instruction
		outputString.append("BSET");

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string regis = retrieveBitsFromInstruction(11, 3, commandString);
		regis = stringBitsToNumber(regis);

		int baseInstructionByteNumber = 2;
		int extensionBytes = 0;
		string effectiveAddress = "";
		if (EAmode == "000") {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'L');
			outputString.append(".L");
		} else {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'B');
			outputString.append(".B");
		}

		outputString.append(" ");

		//special cases where format is incorrect
		if (EAmode == "001" || (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			baseInstructionByteNumber = 0;
		}
		outputString.append("D");
		outputString.append(regis);
		outputString.append(",");
		outputString.append(effectiveAddress);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; Tests a bit in the destination specified by the bit number in the register-> sets the bit in the destination to 1");
		//BSET
	} else if (commandArrayString == "BSET") {
		//BSET bit number static, follows command
		//TEST(<number> of destination) -> Z;
		//0 -> <bit number> of destination
		//BSET Dn,<EA> or BSET #<data>,<EA>
		//15 14 13 12 11 10 9 8 7 6   5 4 3   2 1 0
		//0  0  0  0  1  0  0 0 1 1   EAmode  EAregi
		//
		//effective address field: <ea> is destination
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---
		//Dn = long, all others = byte
		//base of instruction
		outputString.append("BSET");

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string bitNumber = retrieveBitsFromInstruction(-9, 8, commandString);
		bitNumber = stringBitsToNumber(bitNumber);

		int baseInstructionByteNumber = 4;
		int extensionBytes = 0;
		string effectiveAddress = "";
		if (EAmode == "000") {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'L', -16);
			outputString.append(".L");
		} else {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'B', -16);
			outputString.append(".B");
		}

		outputString.append(" ");

		//special cases where format is incorrect
		if (EAmode == "001" || (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			baseInstructionByteNumber = 0;
		}
		outputString.append("#");
		outputString.append(bitNumber);
		outputString.append(",");
		outputString.append(effectiveAddress);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; Tests a bit in the destination specified by the #<bit number> -> sets the bit in the destination to 1");
		//BSET
	} else if (commandArrayString == "BSR") {
		//BSR
		//SP -4-> SP; PC->(SP); PC + dn -> PC
		//BSR <label>
		//15 14 13 12 11 10 9 8    7 6 5 4 3 2 1 0
		//0  1  1  0  0  0  0 1   8-bit displacement
		// if 8-bit == $00, use next 16 bits
		// if 8-bit == $FF, use next 32 bits
		//collect the bits to determine the instruction
		string displacement = retrieveBitsFromInstruction(7, 8, commandString);

		int baseInstructionByteNumber = 2;

		//base of command
		outputString.append("BSR");

		//determine size
		if (displacement == "00000000") {
			displacement = retrieveBitsFromInstruction(-1, 16, commandString);
			outputString.append(".W");
			baseInstructionByteNumber = 4;
		} else if (displacement == "11111111") {
			displacement = retrieveBitsFromInstruction(-1, 32,
					commandString);
			outputString.append(".L");
			baseInstructionByteNumber = 6;
		} else {
			outputString.append(".B");
		}
		string complement = twosComplement(displacement); //twos complement
		displacement = stringBitsToNumber(displacement); //convert data to decimal
		displacement = stringNumber_to_hex(displacement); //convert decimal to hex

		outputString.append(" $");
		outputString.append(displacement);

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append(
				"; SP -4-> SP; PC->(SP); PC + 2 + dn(twos complement = ");
		outputString.append(complement);
		outputString.append(" ) -> PC");
		//BSR
	} else if (commandArrayString == "BTST_bit") {
		//BTST bit number dynamic, specified in register		//TEST(<bit number> of destination) -> Z;
		//TEST(<bit number> of destination) -> Z
		//BTST Dn,<EA> or BTST #<data>,<EA>
		//15 14 13 12 11 10 9   8 7 6   5 4 3   2 1 0
		//0  0  0  0  register  1 0 0   EAmode  EAregi
		//effective address field: <ea> is destination
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          111      100
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011
		//Dn = long, all others = byte
		//base of instruction
		outputString.append("BTST");

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string regis = retrieveBitsFromInstruction(11, 3, commandString);
		regis = stringBitsToNumber(regis);

		int baseInstructionByteNumber = 2;
		int extensionBytes = 0;
		string effectiveAddress = "";
		if (EAmode == "000") {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'L');
			outputString.append(".L");
		} else {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'B');
			outputString.append(".B");
		}

		outputString.append(" ");

		//special cases where format is incorrect
		if (EAmode == "001") {
			//cannot use An
			baseInstructionByteNumber = 0;
		}
		outputString.append("D");
		outputString.append(regis);
		outputString.append(",");
		outputString.append(effectiveAddress);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; Tests a bit in the destination specified by the bit number in the register-> Z in condition register");
		//BTST
	} else if (commandArrayString == "BTST") {
		//BTST bit number static as immediate data
		//TEST(<bit number> of destination) -> Z
		//BTST Dn,<EA> or BTST #<data>,<EA>
		//15 14 13 12 11 10 9  8 7 6   5 4 3   2 1 0
		//0  0  0  0  1  0  0  0 0 0   EAmode  EAregi
		//effective address field: <ea> is destination
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          111      100
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011
		//Dn = long, all others = byte
		//base of instruction
		outputString.append("BTST");

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string bitNumber = retrieveBitsFromInstruction(-9, 8, commandString);
		bitNumber = stringBitsToNumber(bitNumber);

		int baseInstructionByteNumber = 4;
		int extensionBytes = 0;
		string effectiveAddress = "";
		if (EAmode == "000") {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'L', -16);
			outputString.append(".L");
		} else {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'B', -16);
			outputString.append(".B");
		}

		outputString.append(" ");

		//special cases where format is incorrect
		if (EAmode == "001") {
			//cannot use An
			baseInstructionByteNumber = 0;
		}
		outputString.append("#");
		outputString.append(bitNumber);
		outputString.append(",");
		outputString.append(effectiveAddress);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; Tests a bit in the destination specified by the bit number -> Z in condition register");
		//BTST
	} else if (commandArrayString == "CHK") {
		//CHK
		//if Dn < 0 or Dn > Source then trap
		//CHK <EA>,Dn
		//15 14 13 12 11 10 9   8 7  6   5 4 3  2 1 0
		//0  1  0  0  register  size 0    mode   reg
		//effective address bits 5-0
		//size
		//word  long
		// 11    10*
		//*only 68020,68030,68040
		//CPU32 is like 68020, so include the 'Long' interp
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          111      100
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//base of instruction
		outputString.append("CHK");

		//collect the portions of the command which determine
		//mode and addresses
		string size = retrieveBitsFromInstruction(8, 2, commandString);
		string regis = retrieveBitsFromInstruction(11, 3, commandString);
		regis = stringBitsToNumber(regis); //convert data to decimal
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (size == "11") {
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString, effectiveAddress, 'W');
		} else if(size == "10") {
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString, effectiveAddress, 'L');
		} else {
			baseInstructionByteNumber = 0; //set this to 0 if no valid entries are made
		}

		//account for special cases
		if (EAmode == "001") {
			//cannot use An as EA
			baseInstructionByteNumber = 0;
		}

		outputString.append(" ");
		outputString.append(effectiveAddress);
		outputString.append(",D");
		outputString.append(regis);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append("; if Dn < 0 or Dn > source then TRAP");
		//CHK
	} else if (commandArrayString == "CHK2") {
		//CHK2
		//if Rn < lower bound or Rn > upper bound then TRAP
		//CHK2 <EA>,Rn
		//15 14 13 12 11 10 9   8 7 6   5 4 3  2 1 0
		//0  0  0  0  0  size   0 1 1    mode   reg
		//
		//-1  -2 -3 -4 -5 -6 -7 -8 -9 -10 -11 -12 -13 -14 -15 -16
		//D/A register  1  0  0  0  0  0   0   0   0   0   0   0
		//effective address bits 5-0
		//size
		//byte   word  long
		// 00     01    10
		//D/A = 0 - data register, = 1 - Address register
		//register = data or address register
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             ---      --
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            ---      --
		//  -(An)            ---      --
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//base of instruction
		outputString.append("CHK2");

		//collect the portions of the command which determine
		//mode and addresses
		string size = retrieveBitsFromInstruction(10, 2, commandString);
		string regis = retrieveBitsFromInstruction(-2, 3, commandString);
		regis = stringBitsToNumber(regis); //convert data to decimal
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string DA = retrieveBitsFromInstruction(-1, 1, commandString);

		int baseInstructionByteNumber = 4;
		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (size == "00") {
			outputString.append(".B");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'B', -16);
		} else if (size == "01") {
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'W', -16);
		} else if (size == "10") {
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'L', -16);
		} else {
			baseInstructionByteNumber = 0; //set this to 0 if no valid entries are made
		}

		//account for special cases
		if (EAmode == "000" || EAmode == "001" || EAmode == "011"
				|| EAmode == "100" || (EAmode == "111" && EAregi == "100")) {
			//cannot use Dn, An, (An)+, -(An), #<data> as EA
			baseInstructionByteNumber = 0;
		}

		outputString.append(" ");
		outputString.append(effectiveAddress);
		if (DA == "0") {
			outputString.append(",D");
		} else {
			outputString.append(",A");
		}
		outputString.append(regis);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; if Rn < Lower bound or Rn > upper bound then TRAP");
		//CHK2
	} else if (commandArrayString == "CLR") {
		//CLR
		//0 -> Destination
		//CLR <EA>
		//15 14 13 12 11 10 9  8  7 6   5 4 3  2 1 0
		//0  1  0  0  0  0  1  0  size   mode   reg
		//
		//effective address bits 5-0
		//size
		//byte   word  long
		// 00     01    10
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---

		//base of instruction
		outputString.append("CLR");

		//collect the portions of the command which determine
		//mode and addresses
		string size = retrieveBitsFromInstruction(7, 2, commandString);
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (size == "00") {
			outputString.append(".B");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'B');
		} else if (size == "01") {
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'W');
		} else if (size == "10") {
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'L');
		} else {
			baseInstructionByteNumber = 0;//set this to 0 if no valid entries are made
		}

		//account for special cases
		if (EAmode == "001" || (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")
				|| (EAmode == "111" && EAregi == "100")) {
			//cannot use An, #<data>, (d16,PC), (d8,PC,Xn) or (bd,PC,Xn) as EA
			baseInstructionByteNumber = 0;
		}

		outputString.append(" ");
		outputString.append(effectiveAddress);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append("; 0 -> Destination");
		//CLR
	} else if (commandArrayString == "CMP") {
		//CMP
		//Destination - Source -> cc
		//CMP <EA>,Dn
		//15 14 13 12 11 10 9   8 7 6   5 4 3  2 1 0
		//1  0  1  1  register  opmode   mode   reg
		//
		//effective address bits 5-0
		//opmode
		//byte  word  long
		// 000   001   010
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             001      An
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          111      100
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//base of instruction
		outputString.append("CMP");

		//collect the portions of the command which determine
		//mode and addresses
		string opmode = retrieveBitsFromInstruction(8, 3, commandString);
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string regis = retrieveBitsFromInstruction(11, 3, commandString);
		regis = stringBitsToNumber(regis);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (opmode == "000") {
			outputString.append(".B");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'B');
		} else if (opmode == "001") {
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'W');
		} else if (opmode == "010") {
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'L');
		} else {
			baseInstructionByteNumber = 0;//set this to 0 if no valid entries are made
		}

		outputString.append(" ");
		outputString.append(effectiveAddress);
		outputString.append(",D");
		outputString.append(regis);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append("; Destination - source -> cc");
		//CMP
	} else if (commandArrayString == "CMPA") {
		//CMPA
		//Destination - Source -> cc
		//CMPA <EA>,An
		//15 14 13 12 11 10 9   8 7 6   5 4 3  2 1 0
		//1  0  1  1  register  opmode   mode   reg
		//
		//effective address bits 5-0
		//opmode
		//word  long
		// 011   111
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             001      An
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          111      100
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//base of instruction
		outputString.append("CMPA");

		//collect the portions of the command which determine
		//mode and addresses
		string opmode = retrieveBitsFromInstruction(8, 3, commandString);
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string regis = retrieveBitsFromInstruction(11, 3, commandString);
		regis = stringBitsToNumber(regis);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (opmode == "011") {
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'W');
		} else if (opmode == "111") {
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'L');
		} else {
			baseInstructionByteNumber = 0;//set this to 0 if no valid entries are made
		}

		outputString.append(" ");
		outputString.append(effectiveAddress);
		outputString.append(",A");
		outputString.append(regis);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append("; Destination - source -> cc");
		//CMPA
	} else if (commandArrayString == "CMPI") {
		//CMPI
		//Destination - immediate data -> cc
		//CMPI #<data>,<EA>
		//15 14 13 12 11 10 9 8  7 6   5 4 3  2 1 0
		//0  0  0  0  1  1  0 0  size   mode   reg
		//followed by immediate data
		//effective address bits 5-0
		//size
		//byte  word  long
		// 00    01    10
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//base of instruction
		outputString.append("CMPI");

		//collect the portions of the command which determine
		//mode and addresses
		string size = retrieveBitsFromInstruction(7, 2, commandString);
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string data = "";

		int baseInstructionByteNumber = 4;
		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (size == "00") {
			data = retrieveBitsFromInstruction(-9, 8, commandString);
			outputString.append(".B");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'B', -16);
		} else if (size == "01") {
			data = retrieveBitsFromInstruction(-1, 16, commandString);
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'W', -16);
		} else if (size == "10") {
			data = retrieveBitsFromInstruction(-1, 32, commandString);
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'L', -32);
			baseInstructionByteNumber = 6;
		} else {
			baseInstructionByteNumber = 0;//set this to 0 if no valid entries are made
		}
		data = stringBitsToNumber(data);

		//account for special cases of formatting
		if (EAmode == "001" || (EAmode == "111" && EAregi == "100")) {
			//cannot use An or #<data> as EA
			baseInstructionByteNumber = 0;
		}

		outputString.append(" #");
		outputString.append(data);
		outputString.append(",");
		outputString.append(effectiveAddress);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append("; Destination - immediate data -> cc");
		//CMPI
	} else if (commandArrayString == "CMPM") {
		//CMPM
		//Destination - Source -> cc
		//CMPM (Ay)+,(Ax)+
		//15 14 13 12 11 10 9   8 7 6   5 4 3  2 1 0
		//1  0  1  1  regis Ax  1 size  0 0 1  regis Ay
		//
		//size
		//byte word  long
		// 00   01    10

		//base of instruction
		outputString.append("CMPM");

		//collect the portions of the command which determine
		//mode and addresses
		string size = retrieveBitsFromInstruction(7, 2, commandString);
		string regisAy = retrieveBitsFromInstruction(2, 3, commandString);
		string regisAx = retrieveBitsFromInstruction(11, 3, commandString);
		regisAy = stringBitsToNumber(regisAy);
		regisAx = stringBitsToNumber(regisAx);

		int baseInstructionByteNumber = 2;
		//add the size of the base command to the string
		if (size == "00") {
			outputString.append(".B");
		} else if (size == "01") {
			outputString.append(".W");
		} else if (size == "10") {
			outputString.append(".L");
		} else {
			baseInstructionByteNumber = 0;//set this to 0 if no valid entries are made
		}

		outputString.append(" (A");
		outputString.append(regisAy);
		outputString.append(")+,(A");
		outputString.append(regisAx);
		outputString.append(")+");

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append("; Destination - source -> cc");
		//CMPM
	} else if (commandArrayString == "CMP2") {
		//CMP2
		//compare Rn < lower bound or Rn > upper bound then set condition codes
		//CMP2 <EA>,Rn
		//15 14 13 12 11 10 9   8 7 6   5 4 3  2 1 0
		//0  0  0  0  0  size   0 1 1    mode   reg
		//
		//-1  -2 -3 -4 -5 -6 -7 -8 -9 -10 -11 -12 -13 -14 -15 -16
		//D/A register  0  0  0  0  0  0   0   0   0   0   0   0
		//effective address bits 5-0
		//size
		//byte   word  long
		// 00     01    10
		//D/A = 0 - data register, = 1 - Address register
		//register = data or address register
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             ---      --
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            ---      --
		//  -(An)            ---      --
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//base of instruction
		outputString.append("CMP2");

		//collect the portions of the command which determine
		//mode and addresses
		string size = retrieveBitsFromInstruction(10, 2, commandString);
		string regis = retrieveBitsFromInstruction(-2, 3, commandString);
		regis = stringBitsToNumber(regis); //convert data to decimal
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string DA = retrieveBitsFromInstruction(-1, 1, commandString);

		int baseInstructionByteNumber = 4;
		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (size == "00") {
			outputString.append(".B");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'B', -16);
		} else if (size == "01") {
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'W', -16);
		} else if (size == "10") {
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'L', -16);
		} else {
			baseInstructionByteNumber = 0; //set this to 0 if no valid entries are made
		}

		//account for special cases
		if (EAmode == "000" || EAmode == "001" || EAmode == "011"
				|| EAmode == "100" || (EAmode == "111" && EAregi == "100")) {
			//cannot use Dn, An, (An)+, -(An), #<data> as EA
			baseInstructionByteNumber = 0;
		}

		outputString.append(" ");
		outputString.append(effectiveAddress);
		if (DA == "0") {
			outputString.append(",D");
		} else {
			outputString.append(",A");
		}
		outputString.append(regis);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; compare Rn < Lower bound or Rn > upper bound then set condition codes");
		//CMP2
	} else if (commandArrayString == "DBcc") {
		//DBcc
		//if condition false, then (Dn - 1 -> Dn; if Dn != -1 then PC + dn -> PC)
		//DBcc Dn,<label>
		//15 14 13 12  11 10 9 8  7 6 5 4 3 2 1 0
		//0  1  0  1   condition  1 1 0 0 1 regis
		//+16 bit displacement
		//conditional tests
		//Mnemonic     condition   encoding
		//  T             True       0000
		//  F             False      0001
		//  HI            High       0010
		//  LS        Low or same    0011
		// CC(HI)     carry clear    0100
		// CS(LO)      Carry set     0101
		//  NE          not equal    0110
		//  EQ            equal      0111
		//  VC       overflow clear  1000
		//  VS       Overflow set    1001
		//  PL             Plus      1010
		//  MI            Minus		 1011
		//  GE      Greater or equal 1100
		//  LT          Less than    1101
		//  GT        greater than   1110
		//  LE       Less or equal   1111
		//
		//collect the bits to determine the instruction
		string conditionCode = retrieveBitsFromInstruction(11, 4,
				commandString);
		string condition = "";
		string regis = retrieveBitsFromInstruction(2, 3, commandString);
		regis = stringBitsToNumber(regis); //convert data to decimal
		string displacement = retrieveBitsFromInstruction(-1, 16,
				commandString);

		int baseInstructionByteNumber = 4;

		//base of command
		outputString.append("DB");
		//determine condition
		if (conditionCode == "0000") {
			condition = "True";
			conditionCode = "T";
		} else if (conditionCode == "0001") {
			condition = "False";
			conditionCode = "F";
		} else if (conditionCode == "0010") {
			condition = "HI";
			conditionCode = "HI";
		} else if (conditionCode == "0011") {
			condition = "Low or same";
			conditionCode = "LS";
		} else if (conditionCode == "0100") {
			condition = "Carry clear";
			conditionCode = "CC(HI)";
		} else if (conditionCode == "0101") {
			condition = "Carry set";
			conditionCode = "CS(LO)";
		} else if (conditionCode == "0110") {
			condition = "Not equal";
			conditionCode = "NE";
		} else if (conditionCode == "0111") {
			condition = "Equal";
			conditionCode = "EQ";
		} else if (conditionCode == "1000") {
			condition = "Overflow clear";
			conditionCode = "VC";
		} else if (conditionCode == "1001") {
			condition = "Overflow set";
			conditionCode = "VS";
		} else if (conditionCode == "1010") {
			condition = "Plus";
			conditionCode = "PL";
		} else if (conditionCode == "1011") {
			condition = "Minus";
			conditionCode = "MI";
		} else if (conditionCode == "1100") {
			condition = "Greater or equal";
			conditionCode = "GE";
		} else if (conditionCode == "1101") {
			condition = "Less than";
			conditionCode = "LT";
		} else if (conditionCode == "1110") {
			condition = "Greater than";
			conditionCode = "GT";
		} else if (conditionCode == "1111") {
			condition = "Less or equal";
			conditionCode = "LE";
		} else {
			baseInstructionByteNumber = 0;
		}
		outputString.append(conditionCode);

		//determine size
		outputString.append(".W");
		string complement = twosComplement(displacement);
		displacement = stringBitsToNumber(displacement); //convert data to decimal
		displacement = stringNumber_to_hex(displacement); //convert decimal to hex

		outputString.append(" D");
		outputString.append(regis);
		outputString.append(",");
		outputString.append("$");
		outputString.append(displacement);

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append("; if condition (");
		outputString.append(condition);
		outputString.append(
				") false, then (Dn - 1 -> Dn; if Dn != -1 then PC + dn + 2 -> PC) twos complement = ");
		outputString.append(complement);
		outputString.push_back(' ');
		//DBcc
	} else if (commandArrayString == "DIVS_DIVSL") {
		//DIVS,DIVSL word
		//Signed divide
		//destination / source -> destination
		//DIVS.W <ea>,Dn	32/16 -> 16r - 16q
		//DIVS.L <ea>,Dq	32/32 -> 32q
		//DIVS.L <ea>,Dr:Dq	64/32 -> 32r - 32q
		//DIVSL.L<ea>,Dr:Dq	32/32 -> 32r - 32q
		//*the last 3 versions only apply to 68020,68030,68040 and CPU32
		//15 14 13 12  11 10 9   8 7 6   5 4 3  2 1 0
		//1  0  0  0    regis    1 1 1    mode   reg
		//
		//effective address bits 5-0
		//register = data register
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          111      100
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//base of instruction
		outputString.append("DIVS.W");

		//collect the portions of the command which determine
		//mode and addresses
		string regis = retrieveBitsFromInstruction(11, 3, commandString);
		regis = stringBitsToNumber(regis); //convert data to decimal
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		extensionBytes = assembleEffectiveAddress(commandString,
				effectiveAddress, 'W');

		//account for special cases
		if (EAmode == "001") {
			//cannot use An as EA
			baseInstructionByteNumber = 0;
		}

		outputString.append(" ");
		outputString.append(effectiveAddress);
		outputString.append(",D");
		outputString.append(regis);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; destination(32 bits)/source(16 bits) -> destination (16 remainder bits then 16 quotient bits");
		//DIVS,DIVSL word
	} else if (commandArrayString == "DIVS_DIVSL_follow") {
		//DIVS,DIVSL long
		//Signed divide
		//destination / source -> destination
		//DIVS.W <ea>,Dn	32/16 -> 16r - 16q
		//DIVS.L <ea>,Dq	32/32 -> 32q
		//DIVS.L <ea>,Dr:Dq	64/32 -> 32r - 32q
		//DIVSL.L<ea>,Dr:Dq	32/32 -> 32r - 32q
		//*the last 3 versions only apply to 68020,68030,68040 and CPU32
		//15 14 13 12 11 10 9 8 7 6   5 4 3  2 1 0
		//0  1  0  0  1  1  0 0 0 1    mode   reg
		//
		//-1  -2 -3 -4  -5  -6   -7 -8 -9 -10 -11 -12 -13 -14 -15 -16
		//0  registerDq  1  size  0  0  0  0   0   0   0   registerDr
		//size = 0 ->32 bit, size = 1 -> 64 bit
		//effective address bits 5-0
		//register = data register
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          111      100
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//base of instruction
		outputString.append("DIVS");

		//collect the portions of the command which determine
		//mode and addresses
		string size = retrieveBitsFromInstruction(-6, 1, commandString);
		string regisDq = retrieveBitsFromInstruction(-2, 3, commandString);
		regisDq = stringBitsToNumber(regisDq); //convert data to decimal
		string regisDr = retrieveBitsFromInstruction(-14, 3, commandString);
		regisDr = stringBitsToNumber(regisDr); //convert data to decimal
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 4;
		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (regisDr != regisDq && size == "0") {
			outputString.append("L");
		} else if (regisDr == regisDq && size == "1") {
			//this is not allowed
			baseInstructionByteNumber = 0;
		}
		outputString.append(".L ");

		extensionBytes = assembleEffectiveAddress(commandString,
				effectiveAddress, 5, 'L', -16);
		outputString.append(effectiveAddress);
		outputString.append(",D");
		//account for special cases
		if (EAmode == "001") {
			//cannot use An as EA
			baseInstructionByteNumber = 0;
		}

		if (regisDr != regisDq) {
			outputString.append(regisDr);
			outputString.append(":D");
		}
		outputString.append(regisDq);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; destination(32 or 64 bits)/source(32 bits) -> destination (32 remainder bits then 32 quotient bits. If only one data register, then it has 32 quotient bits");
		//DIVS,DIVSL long
	} else if (commandArrayString == "DIVU_DIVUL") {
		//DIVU,DIVUL word
		//Unsigned divide
		//destination / source -> destination
		//DIVU.W <ea>,Dn	32/16 -> 16r - 16q
		//DIVU.L <ea>,Dq	32/32 -> 32q
		//DIVU.L <ea>,Dr:Dq	64/32 -> 32r - 32q
		//DIVUL.L<ea>,Dr:Dq	32/32 -> 32r - 32q
		//*the last 3 versions only apply to 68020,68030,68040 and CPU32
		//15 14 13 12  11 10 9   8 7 6   5 4 3  2 1 0
		//1  0  0  0    regis    0 1 1    mode   reg
		//
		//effective address bits 5-0
		//register = data register
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          111      100
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//base of instruction
		outputString.append("DIVU.W");

		//collect the portions of the command which determine
		//mode and addresses
		string regis = retrieveBitsFromInstruction(11, 3, commandString);
		regis = stringBitsToNumber(regis); //convert data to decimal
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		extensionBytes = assembleEffectiveAddress(commandString,
				effectiveAddress, 'W');

		//account for special cases
		if (EAmode == "001") {
			//cannot use An as EA
			baseInstructionByteNumber = 0;
		}

		outputString.append(" ");
		outputString.append(effectiveAddress);
		outputString.append(",D");
		outputString.append(regis);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; destination(32 bits)/source(16 bits) -> destination (16 remainder bits then 16 quotient bits)");
		//DIVU,DIVUL word
	} else if (commandArrayString == "DIVU_DIVUL_follow") {
		//DIVU,DIVUL long
		//Unsigned divide
		//destination / source -> destination
		//DIVU.W <ea>,Dn	32/16 -> 16r - 16q
		//DIVU.L <ea>,Dq	32/32 -> 32q
		//DIVU.L <ea>,Dr:Dq	64/32 -> 32r - 32q
		//DIVUL.L<ea>,Dr:Dq	32/32 -> 32r - 32q
		//*the last 3 versions only apply to 68020,68030,68040 and CPU32
		//15 14 13 12 11 10 9 8 7 6   5 4 3  2 1 0
		//0  1  0  0  1  1  0 0 0 1    mode   reg
		//
		//-1  -2 -3 -4  -5  -6   -7 -8 -9 -10 -11 -12 -13 -14 -15 -16
		//0  registerDq  0  size  0  0  0  0   0   0   0   registerDr
		//size = 0 ->32 bit, size = 1 -> 64 bit
		//effective address bits 5-0
		//register = data register
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          111      100
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//base of instruction
		outputString.append("DIVU");

		//collect the portions of the command which determine
		//mode and addresses
		string size = retrieveBitsFromInstruction(-6, 1, commandString);
		string regisDq = retrieveBitsFromInstruction(-2, 3, commandString);
		regisDq = stringBitsToNumber(regisDq); //convert data to decimal
		string regisDr = retrieveBitsFromInstruction(-14, 3, commandString);
		regisDr = stringBitsToNumber(regisDr); //convert data to decimal
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 4;
		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (regisDr != regisDq && size == "0") {
			outputString.append("L");
		} else if (regisDr == regisDq && size == "1") {
			//this is not allowed
			baseInstructionByteNumber = 0;
		}
		outputString.append(".L ");

		extensionBytes = assembleEffectiveAddress(commandString,
				effectiveAddress, 5, 'L', -16);
		outputString.append(effectiveAddress);
		outputString.append(",D");
		//account for special cases
		if (EAmode == "001") {
			//cannot use An as EA
			baseInstructionByteNumber = 0;
		}

		if (regisDr != regisDq) {
			outputString.append(regisDr);
			outputString.append(":D");
		}
		outputString.append(regisDq);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; destination(32 or 64 bits)/source(32 bits) -> destination (32 remainder bits then 32 quotient bits. If only one data register, then it has 32 quotient bits");
		//DIVU,DIVUL long
	} else if (commandArrayString == "EOR") {
		//EOR
		//exclusive or
		//EOR Dn,<EA>
		//15 14 13 12 11 10 9   8 7 6   5 4 3  2 1 0
		//1  0  1  1  register  opmode   mode   reg
		//
		//effective address bits 5-0
		//opmode
		//byte  word  long
		// 100   101   110
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---

		//base of instruction
		outputString.append("EOR");

		//collect the portions of the command which determine
		//mode and addresses
		string opmode = retrieveBitsFromInstruction(8, 3, commandString);
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string regis = retrieveBitsFromInstruction(11, 3, commandString);
		regis = stringBitsToNumber(regis);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (opmode == "100") {
			outputString.append(".B");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'B');
		} else if (opmode == "101") {
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'W');
		} else if (opmode == "110") {
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'L');
		} else {
			baseInstructionByteNumber = 0;//set this to 0 if no valid entries are made
		}

		//account for incorrect effective address modes
		if (EAmode == "001" || (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			//cannot use An, #<data>, (d16,PC), (d8,PC,Xn) as EA
			baseInstructionByteNumber = 0;
		}

		outputString.append(" D");
		outputString.append(regis);
		outputString.append(",");
		outputString.append(effectiveAddress);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; source OR(exclusive) destination -> destination");
		//EOR
	} else if (commandArrayString == "EORI") {
		//EORI
		//Exclusive OR with immediate data
		//EORI #<data>,<EA>
		//15 14 13 12 11 10 9 8  7 6   5 4 3  2 1 0
		//0  0  0  0  1  0  1 0  size   mode   reg
		//
		//effective address bits 5-0
		//size
		//byte   word  long
		// 00     01    10
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---

		//base of instruction
		outputString.append("EORI");

		//collect the portions of the command which determine
		//mode and addresses
		string size = retrieveBitsFromInstruction(7, 2, commandString);
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string data = "";

		int baseInstructionByteNumber = 4;
		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (size == "00") {
			data = retrieveBitsFromInstruction(-9, 8, commandString);
			outputString.append(".B");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'B', -16);
		} else if (size == "01") {
			data = retrieveBitsFromInstruction(-1, 16, commandString);
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'W', -16);
		} else if (size == "10") {
			data = retrieveBitsFromInstruction(-1, 32, commandString);
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'L', -32);
			baseInstructionByteNumber = 6;
		} else {
			baseInstructionByteNumber = 0;//set this to 0 if no valid entries are made
		}

		//account for special cases
		if (EAmode == "001" || (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			//cannot use An, #<data>, (d16,PC), (d8,PC,Xn) as EA
			baseInstructionByteNumber = 0;
		}
		data = stringBitsToNumber(data);
		outputString.append(" #");
		outputString.append(data);
		outputString.append(",");
		outputString.append(effectiveAddress);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; immediate data OR(exclusive) destination -> destination");
		//EORI
	} else if (commandArrayString == "EORItoCCR") {
		//EORI to CCR
		//exclusive OR immediate data to condition code
		//source OR CCR -> CCR
		//EORI #<data>,CCR
		//15 14 13 12 11 10 9 8  7 6   5 4 3  2 1 0
		//0  0  0  0  1  0  1 0  0 0   1 1 1  1 0 0
		//followed  by an 8-bit byte in the LSB of a word
		//size = byte
		//base of instruction
		outputString.append("EORI #");

		//collect the portions of the command which determine
		//mode and addresses
		string data = retrieveBitsFromInstruction(-9, 8, commandString);

		int baseInstructionByteNumber = 4;

		data = stringBitsToNumber(data);
		outputString.append(data);
		outputString.append(",CCR");

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append("; immediate data OR(exclusive) CCR -> CCR");
		//EORI to CCR
	} else if (commandArrayString == "EXG") {
		//EXG
		//exchange registers
		//EXG Dx,Dy or EXG Ax,Ay or EXG Dx,Ay
		//15 14 13 12 11 10 9  8  7 6 5 4 3  2 1 0
		//1  1  0  0  regisRx  1   opmode    regisRy
		//
		//size = long
		//if opmode == 01000 -> data registers
		//             01001 -> address registers
		//	           10001 -> data register and address register

		//base of instruction
		outputString.append("EXG");

		//collect the portions of the command which determine
		//mode and addresses
		string opmode = retrieveBitsFromInstruction(7, 5, commandString);
		string regisRx = retrieveBitsFromInstruction(11, 3, commandString);
		string regisRy = retrieveBitsFromInstruction(2, 3, commandString);
		regisRx = stringBitsToNumber(regisRx);
		regisRy = stringBitsToNumber(regisRy);

		int baseInstructionByteNumber = 2;

		//add the size of the base command to the string
		if (opmode == "01000") {
			outputString.append(" D");
			outputString.append(regisRx);
			outputString.append(",D");
			outputString.append(regisRy);
		} else if (opmode == "01001") {
			outputString.append(" A");
			outputString.append(regisRx);
			outputString.append(",A");
			outputString.append(regisRy);
		} else if (opmode == "10001") {
			outputString.append(" D");
			outputString.append(regisRx);
			outputString.append(",A");
			outputString.append(regisRy);
		} else {
			baseInstructionByteNumber = 0;//set this to 0 if no valid entries are made
		}

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append("; exchange contents of two 32-bit registers");
		//EXG
	} else if (commandArrayString == "EXT_EXTB") {
		//EXT,EXTB
		//sign extend
		//EXT.W Dn extend byte to word
		//EXT.L Dn extend word to long
		//EXTB.L Dn extend byte to long
		//15 14 13 12 11 10 9  8 7 6   5 4 3  2 1 0
		//0  1  0  0  1  0  0  opmode  0 0 0  register
		//
		//size = word or long
		//if opmode == 010 -> byte to word
		//             011 -> word to long
		//	           111 -> byte to long

		//base of instruction
		outputString.append("EXT");

		//collect the portions of the command which determine
		//mode and addresses
		string opmode = retrieveBitsFromInstruction(8, 3, commandString);
		string regis = retrieveBitsFromInstruction(2, 3, commandString);
		regis = stringBitsToNumber(regis);

		int baseInstructionByteNumber = 2;

		//add the size of the base command to the string
		if (opmode == "010") {
			outputString.append(".W D");
		} else if (opmode == "011") {
			outputString.append(".L D");
		} else if (opmode == "111") {
			outputString.append("B.L D");
		} else {
			baseInstructionByteNumber = 0;//set this to 0 if no valid entries are made
		}
		outputString.append(regis);

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append(
				"; sign-extend byte to word (EXT.W) or long (EXTB.L) or word to long (EXT.L)");
		//EXT,EXTB
	} else if (commandArrayString == "ILLEGAL") {
		//ILLEGAL
		//take illegal instruction trap
		//*SSP - 2 -> SSP; Vector offset ->(SSP);
		//SSP - 4 -> SSP; PC -> (SSP);
		//SSP - 2 -> SSP; SR -> (SSP);
		//illegal instruction vector address -> PC
		//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
		//0  1  0  0  1  0  1 0 1 1 1 1 1 1 0 0

		//base of command
		outputString.append("ILLEGAL");
		correctForm = 2;
		outputString.push_back('\t');
		outputString.append("; take illegal instruction trap, vector number 4");
		//ILLEGAL
	} else if (commandArrayString == "JMP") {
		//JMP
		//Destination Address -> PC
		//JMP <EA>
		//15 14 13 12 11 10 9 8  7 6   5 4 3  2 1 0
		//0  1  0  0  1  1  1 0  1 1   mode   reg
		//
		//effective address bits 5-0
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             ---      --
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            ---      --
		//  -(An)            ---      --
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//base of instruction
		outputString.append("JMP ");

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;

		string effectiveAddress = "";
		int extensionBytes = 0;
		extensionBytes = assembleEffectiveAddress(commandString,
				effectiveAddress, 'B');

		outputString.append(effectiveAddress);

		//account for special cases
		if (EAmode == "000" || EAmode == "001" || EAmode == "011"
				|| EAmode == "100" || (EAmode == "111" && EAregi == "100")
				) {
			//cannot use Dn, An, (An)+, -(An), #<data>, as EA
			baseInstructionByteNumber = 0;
		}

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append("; destination address -> PC");
		//JMP
	} else if (commandArrayString == "JSR") {
		//JSR
		//Jump to subroutine
		//SP - 4 -> Sp; PC -> (SP); destination Address -> PC
		//JSR <ea>
		//15 14 13 12 11 10 9 8  7 6   5 4 3  2 1 0
		//0  1  0  0  1  1  1 0  1 0   mode   reg
		//
		//effective address bits 5-0
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             ---      --
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            ---      --
		//  -(An)            ---      --
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//base of instruction
		outputString.append("JSR ");

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;

		string effectiveAddress = "";
		int extensionBytes = 0;
		extensionBytes = assembleEffectiveAddress(commandString,
				effectiveAddress, 'B');

		outputString.append(effectiveAddress);

		//account for special cases
		if (EAmode == "000" || EAmode == "001" || EAmode == "011"
				|| EAmode == "100" || (EAmode == "111" && EAregi == "100")
				) {
			//cannot use Dn, An, (An)+, -(An), #<data>, as EA
			baseInstructionByteNumber = 0;
		}

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; SP - 4 -> Sp; PC -> (SP); destination address -> PC");
		//JSR
	} else if (commandArrayString == "LEA") {
		//LEA
		//load effective address
		//<ea> -> An
		//LEA <EA>,An
		//15 14 13 12 11 10 9   8 7 6   5 4 3  2 1 0
		//0  1  0  0  register  1 1 1   mode   reg
		//size = long
		//effective address bits 5-0
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             ---      --
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            ---      --
		//  -(An)            ---      --
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//base of instruction
		outputString.append("LEA ");

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string regis = retrieveBitsFromInstruction(11, 3, commandString);
		regis = stringBitsToNumber(regis);

		int baseInstructionByteNumber = 2;

		string effectiveAddress = "";
		int extensionBytes = 0;
		extensionBytes = assembleEffectiveAddress(commandString,
				effectiveAddress, 'L');

		outputString.append(effectiveAddress);
		outputString.append(",A");
		outputString.append(regis);

		//account for special cases
		if (EAmode == "000" || EAmode == "001" || EAmode == "011"
				|| EAmode == "100" || (EAmode == "111" && EAregi == "100")
				) {
			//cannot use Dn, An, (An)+, -(An), #<data>, as EA
			baseInstructionByteNumber = 0;
		}

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append("; load effective address into address register");
		//LEA
	} else if (commandArrayString == "LINK_word") {
		//LINK word
		//link and allocate
		//SP - 4 -> SP; An -> (SP); SP -> An; SP + dn -> SP
		//LINK An,#<displacement>
		//15 14 13 12 11 10 9  8 7 6 5 4 3  2 1 0
		//0  1  0  0  1  1  1  0 0 1 0 1 0 register
		//followed by 16 or 32 bit displacement
		//size = word, long

		//base of instruction
		outputString.append("LINK.W A");

		//collect the portions of the command which determine
		//mode and addresses
		string displacement = retrieveBitsFromInstruction(-1, 16,
				commandString);
		string regis = retrieveBitsFromInstruction(2, 3, commandString);
		regis = stringBitsToNumber(regis);
		string allocateDisplacement = twosComplement(displacement);
		displacement = stringBitsToNumber(displacement);
		displacement = stringNumber_to_hex(displacement);

		int baseInstructionByteNumber = 4;

		outputString.append(regis);
		outputString.append(",$");
		outputString.append(displacement);

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append("; SP - 4 -> SP; An ->(SP); SP-> An; SP+dn -> SP, dn => twos complement = ");
		outputString.append(allocateDisplacement);
		outputString.push_back(' ');
		//LINK word
	} else if (commandArrayString == "LINK_long") {
		//LINK long
		//link and allocate
		//SP - 4 -> SP; An -> (SP); SP -> An; SP + dn -> SP
		//LINK An,#<displacement>
		//15 14 13 12 11 10 9  8 7 6 5 4 3  2 1 0
		//0  1  0  0  1  1  1  0 0 1 0 1 0 register
		//followed by 16 or 32 bit displacement
		//size = word, long

		//base of instruction
		outputString.append("LINK.L A");

		//collect the portions of the command which determine
		//mode and addresses
		string displacement = retrieveBitsFromInstruction(-1, 32,
				commandString);
		string regis = retrieveBitsFromInstruction(2, 3, commandString);
		regis = stringBitsToNumber(regis);
		string allocateDisplacement = twosComplement(displacement);
		displacement = stringBitsToNumber(displacement);
		displacement = stringNumber_to_hex(displacement);

		int baseInstructionByteNumber = 6;

		outputString.append(regis);
		outputString.append(",$");
		outputString.append(displacement);

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append("; SP - 4 -> SP; An ->(SP); SP-> An; SP+dn -> SP, dn => twos complement = ");
		outputString.append(allocateDisplacement);
		outputString.push_back(' ');
		//LINK long
	} else if (commandArrayString == "LSL_LSR_sizeSelect") {
		//LSL,LSR logical register shift
		//destination shifted by byte, word or long
		//LSd Dx,Dy or LSd #<data>,Dy or LSd<EA>
		//d is L or R
		//15 14 13 12 11 10 9   8   7 6   5   4 3  2 1 0
		//1  1  1  0   count?  D/R  size  i/r 0 1  Regis
		//            register
		//
		//i/r = 0 : count/Register contains shift count
		//1-7 or 0 = 8
		//i/r = 1 : count/Register contains data register
		//which contains shift count, modulo 64
		//
		//D/R specifies direction of shift: 0 = right, 1 = left
		//
		//size
		//byte   word   long
		// 00     01     10

		//base of instruction
		outputString.append("LS");

		//collect the portions of the command which determine
		//mode and addresses
		string count_Register = retrieveBitsFromInstruction(11, 3,
				commandString);
		count_Register = stringBitsToNumber(count_Register); //convert data to decimal
		string DR = retrieveBitsFromInstruction(8, 1, commandString);
		string size = retrieveBitsFromInstruction(7, 2, commandString);
		string ir = retrieveBitsFromInstruction(5, 1, commandString);
		string regis = retrieveBitsFromInstruction(2, 3, commandString);
		regis = stringBitsToNumber(regis); //convert data to decimal

		int baseInstructionByteNumber = 2;

		if (DR == "0") {
			outputString.append("R");
		} else if (DR == "1") {
			outputString.append("L");
		}

		if (size == "00") {
			outputString.append(".B");
		} else if (size == "01") {
			outputString.append(".W");
		} else if (size == "10") {
			outputString.append(".L");
		} else {
			baseInstructionByteNumber = 0;
		}

		outputString.append(" ");
		if (ir == "0") {
			//immediate count
			outputString.append("#");
			if (count_Register == "0") {
				count_Register = "8";
			}
			outputString.append(count_Register);
			outputString.append(",D");
			outputString.append(regis);
		} else if (ir == "1") {
			//register shift count
			outputString.append("D");
			outputString.append(count_Register);
			outputString.append(",D");
			outputString.append(regis);
		}

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append("; Destination shifted by Count -> Destination");
		//LSL,LSR logical register shift
	} else if (commandArrayString == "LSL_LSR") {
		//LSL,LSR logical memory shift
		//destination shifted by byte, word or long
		//LSd Dx,Dy or LSd #<data>,Dy or LSd <EA>
		//d is L or R
		//15 14 13 12 11 10 9   8   7 6   5 4 3  2 1 0
		//1  1  1  0  0  0  1  D/R  1 1  EAmode  EAregi
		//
		//D/R specifies direction of shift: 0 = right, 1 = left
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             ---      --
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---

		//base of instruction
		outputString.append("LS");

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string DR = retrieveBitsFromInstruction(8, 1, commandString);

		int baseInstructionByteNumber = 2;

		if (DR == "0") {
			outputString.append("R");
		} else if (DR == "1") {
			outputString.append("L");
		}

		int extensionBytes = 0;
		string effectiveAddress = "";
		extensionBytes = assembleEffectiveAddress(commandString,
				effectiveAddress, 'B');
		outputString.append(" ");

		//special cases where format is incorrect
		if (EAmode == "000" || EAmode == "001"
				|| (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			baseInstructionByteNumber = 0;
		}

		outputString.append(effectiveAddress);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append("; Destination shifted by 1 bit -> Destination");
		//LSL,LSR memory shift
	} else if (commandArrayString == "MOVE") {
		//MOVE
		//move data from source to destination
		//Source -> Destination
		//MOVE <ea>,<ea>
		//15 14 13 12 11 10 9   8 7 6  5 4 3  2 1 0
		//0  0  size  EAregi    EAmode EAmode2 EAregi
		//              destination   |    source
		//source effective address bits 5-0
		//destination effective address bits 11-6
		//size
		//byte  word  long
		//01     11    10
		//effective address field: destination
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---

		//effective address field: if <ea> is source
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             001      An
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          111      100
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//add the base command to the string
		string baseCommand = "MOVE";
		outputString.append(baseCommand);

		//collect the portions of the command which determine
		//mode and addresses
		string size = retrieveBitsFromInstruction(13, 2, commandString);
		string sourceEAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string sourceEAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string destinEAmode = retrieveBitsFromInstruction(8, 3, commandString);
		string destinEAregi = retrieveBitsFromInstruction(11, 3, commandString);

		int baseInstructionByteNumber = 2;
		string sourceEffectiveAddress = "";
		int sourceExtensionBytes = 0;
		string destinEffectiveAddress = "";
		int destinExtensionBytes = 0;
		//add the size of the base command to the string
		if (size == "01") {
			outputString.append(".B");
			sourceExtensionBytes = assembleEffectiveAddress(commandString,
					sourceEffectiveAddress, 'B');
			destinExtensionBytes = assembleEffectiveAddress(commandString,
					destinEffectiveAddress, destinEAmode, destinEAregi, 'B',
					(sourceExtensionBytes * -8));
		} else if (size == "11") {
			outputString.append(".W");
			sourceExtensionBytes = assembleEffectiveAddress(commandString,
					sourceEffectiveAddress, 'W');
			destinExtensionBytes = assembleEffectiveAddress(commandString,
					destinEffectiveAddress, destinEAmode, destinEAregi, 'W',
					(sourceExtensionBytes * -8));
		} else if (size == "10") {
			outputString.append(".L");
			sourceExtensionBytes = assembleEffectiveAddress(commandString,
					sourceEffectiveAddress, 'L');
			destinExtensionBytes = assembleEffectiveAddress(commandString,
					destinEffectiveAddress, destinEAmode, destinEAregi, 'L',
					(sourceExtensionBytes * -8));
		} else {
			baseInstructionByteNumber = 0;//set this to 0 if no valid entries are made
		}

		outputString.append(" ");
		outputString.append(sourceEffectiveAddress);
		outputString.append(",");
		outputString.append(destinEffectiveAddress);

		//denote special cases where command may be invalid
		if (destinEAmode == "001"
				|| (destinEAmode == "111" && destinEAregi == "100")
				|| (destinEAmode == "111" && destinEAregi == "010")
				|| (destinEAmode == "111" && destinEAregi == "011")) {
			//cannot use An, #<data>, (d16,PC), (d8,PC,Xn) as EA source operand
			baseInstructionByteNumber = 0;
		}

		if (baseInstructionByteNumber != 0 && sourceExtensionBytes != -1
				&& destinExtensionBytes != -1) {
			correctForm = baseInstructionByteNumber + sourceExtensionBytes
					+ destinExtensionBytes;
		}
		outputString.push_back('\t');
		outputString.append("; source -> destination");
		//MOVE
	} else if (commandArrayString == "MOVEA") {
		//MOVEA
		//Source -> destination
		//MOVEA <EA>,An
		//15 14 13 12 11 10 9   8 7 6  5 4 3  2 1 0
		//0  0  size  register  0 0 1  EAmode EAregi
		//source effective address bits 5-0
		//size
		//word  long
		// 11    10
		//effective address field:
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             001      An
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          111      100
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//add the base command to the string
		string baseCommand = "MOVEA";
		outputString.append(baseCommand);

		//collect the portions of the command which determine
		//mode and addresses
		string size = retrieveBitsFromInstruction(13, 2, commandString);
		string sourceEAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string sourceEAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string regis = retrieveBitsFromInstruction(11, 3, commandString);
		regis = stringBitsToNumber(regis);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (size == "11") {
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'W');
		} else if (size == "10") {
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'L');
		} else {
			baseInstructionByteNumber = 0;//set this to 0 if no valid entries are made
		}

		outputString.append(" ");
		outputString.append(effectiveAddress);
		outputString.append(",A");
		outputString.append(regis);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}
		outputString.push_back('\t');
		outputString.append("; source -> destination");
		//MOVEA
	} else if (commandArrayString == "MOVEfromCCR") {
		//MOVE from CCR
		//move from the condition code register
		//CCR->destination
		//MOVE CCR,<EA>
		//15 14 13 12 11 10 9  8 7 6  5 4 3  2 1 0
		//0  1  0  0  0  0  1  0 1 1  EAmode EAregi
		//effective address bits 5-0
		//effective address field: destination
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---

		//add the base command to the string
		string baseCommand = "MOVE CCR,";
		outputString.append(baseCommand);

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		extensionBytes = assembleEffectiveAddress(commandString,
				effectiveAddress, 'W');

		outputString.append(effectiveAddress);

		//denote special cases where command may be invalid
		if (EAmode == "001" || (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			//cannot use An, #<data>, (d16,PC), (d8,PC,Xn) as EA
			baseInstructionByteNumber = 0;
		}

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}
		outputString.push_back('\t');
		outputString.append("; CCR (word) -> destination");
		//MOVE from CCR
	} else if (commandArrayString == "MOVEtoCCR") {
		//MOVE to CCR
		//move to the condition code register
		//Source -> CCR
		//MOVE <EA>,CCR
		//15 14 13 12 11 10 9  8 7 6  5 4 3  2 1 0
		//0  1  0  0  0  1  0  0 1 1  EAmode EAregi
		//effective address bits 5-0
		//effective address field: source
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          111      100
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//add the base command to the string
		string baseCommand = "MOVE ";
		outputString.append(baseCommand);

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		extensionBytes = assembleEffectiveAddress(commandString,
				effectiveAddress, 'W');

		outputString.append(effectiveAddress);
		outputString.append(",CCR");

		//denote special cases where command may be invalid
		if (EAmode == "001") {
			//cannot use An as EA source operand
			baseInstructionByteNumber = 0;
		}

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}
		outputString.push_back('\t');
		outputString.append("; Source (word) -> CCR");
		//MOVE to CCR
	} else if (commandArrayString == "MOVEfromSR") {
		//MOVE from SR
		//move from status register
		//SR -> Destination
		//MOVE SR,<EA>
		//15 14 13 12 11 10 9  8 7 6  5 4 3  2 1 0
		//0  1  0  0  0  0  0  0 1 1  EAmode EAregi
		//effective address bits 5-0
		//effective address field: source
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---

		//add the base command to the string
		string baseCommand = "MOVE SR,";
		outputString.append(baseCommand);

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		extensionBytes = assembleEffectiveAddress(commandString,
				effectiveAddress, 'W');

		outputString.append(effectiveAddress);

		//denote special cases where command may be invalid
		if (EAmode == "001" || (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			//cannot use An, #<data>, (d16,PC), (d8,PC,Xn) as EA
			baseInstructionByteNumber = 0;
		}

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}
		outputString.push_back('\t');
		outputString.append("; SR (word) -> destination");
		//MOVE from SR
	} else if (commandArrayString == "MOVEM") {
		//MOVEM
		//move multiple registers
		//Registers -> Destination; Source -> registers
		//MOVEM <list>,<EA>
		//MOVEM <EA>,<list>
		//15 14 13 12 11 10 9  8 7   6   5 4 3  2 1 0
		//0  1  0  0  1  DR 0  0 1 size  EAmode EAregi
		//followed by 16-bit register list mask
		//if DR = 0 - register to memory; = 1 - memory ot register
		//size = 0 - word transfer; 1 - long transfer
		//effective address bits 5-0
		//effective address field: destination
		//addressing mode    mode    register
		//    Dn             ---      --
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            ---      --
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---
		//
		//effective address field: source
		//addressing mode    mode    register
		//    Dn             ---      --
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            ---      --
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//add the base command to the string
		string baseCommand = "MOVEM";
		outputString.append(baseCommand);

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string DR = retrieveBitsFromInstruction(10, 1, commandString);
		string size = retrieveBitsFromInstruction(6, 1, commandString);
		string registerListMask = retrieveBitsFromInstruction(-1, 16,
				commandString);

		int baseInstructionByteNumber = 4;
		string effectiveAddress = "";
		int extensionBytes = 0;
		if (size == "0") {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'W', -16);
			outputString.append(".W");
		} else if (size == "1") {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'L', -16);
			outputString.append(".L");
		}

		//loop through the registerListMask to assign the register
		//register list mask: A7,A6,A5,A4,A3,A2,A1,A0,D7,D6,D5,D4,D3,D2,D1,D0 is reversed for -(An)
		string registerList = "";
		bool firstAddress = true;
		for (int i = 0; i < 16; i++) {
			char listMask[16] = { '7', '6', '5', '4', '3', '2', '1', '0', '7',
					'6', '5', '4', '3', '2', '1', '0' };
			char listRegi[16] = { 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'A', 'D',
					'D', 'D', 'D', 'D', 'D', 'D', 'D' };
			if (registerListMask[i] == '1') {
				if (firstAddress == false) {
					registerList.push_back('/');
				} else {
					firstAddress = false;
				}
				if (EAmode == "100") {
					registerList.push_back(listRegi[15 - i]);
					registerList.push_back(listMask[15 - i]);

				} else {
					registerList.push_back(listRegi[i]);
					registerList.push_back(listMask[i]);
				}
			}
		}

		outputString.append(" ");
		if (DR == "0") {
			//% denotes binary
			//outputString.append("%");
			//outputString.append(registerListMask);
			outputString.append(registerList);
			outputString.append(",");
			outputString.append(effectiveAddress);
		} else if (DR == "1") {
			outputString.append(effectiveAddress);
			outputString.append(",");
			//outputString.append("%");
			//outputString.append(registerListMask);
			outputString.append(registerList);
		}

		//denote special cases where command may be invalid
		if (DR == "0"
				&& (EAmode == "000" || EAmode == "001" || EAmode == "011"
						|| (EAmode == "111" && EAregi == "100")
						|| (EAmode == "111" && EAregi == "010")
						|| (EAmode == "111" && EAregi == "011"))) {
			//<EA> as destination
			//cannot use Dn, An, (An)+, #<data>, (d16,PC), (d8,PC,Xn) as EA
			baseInstructionByteNumber = 0;
		} else if (DR == "1"
				&& (EAmode == "000" || EAmode == "001" || EAmode == "100"
						|| (EAmode == "111" && EAregi == "100"))) {
			//<EA> as source
			//cannot use Dn, An, -(An), #<data>, (d16,PC), (d8,PC,Xn) as EA
			baseInstructionByteNumber = 0;
		}

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}
		outputString.push_back('\t');
		outputString.append(
				"; Registers -> destination (OR) source -> registers; register list mask: A7,A6,A5,A4,A3,A2,A1,A0,D7,D6,D5,D4,D3,D2,D1,D0 is reversed for -(An)");
		//MOVEM
	} else if (commandArrayString == "MOVEP") {
		//MOVEP
		//move peripheral data
		//source -> destination
		//MOVEP Dx,(d16,Ay) or MOVEP (d16,Ay),Dx
		//15 14 13 12   11 10 9   8 7 6   5 4 3  2 1 0
		//0  0  0  0  data regis  opmode  0 0 1 address register
		//followed by 16-bit displacement
		//
		//opmode
		//100 - word from memory to register
		//101 - long from memory to register
		//110 - word from register to memory
		//111 - long from register to memory

		//add the base command to the string
		string baseCommand = "MOVEP";
		outputString.append(baseCommand);

		//collect the portions of the command which determine
		//mode and addresses
		string dataRegi = retrieveBitsFromInstruction(11, 3, commandString);
		string addrRegi = retrieveBitsFromInstruction(2, 3, commandString);
		string opmode = retrieveBitsFromInstruction(8, 3, commandString);
		string displacement = retrieveBitsFromInstruction(-1, 16,
				commandString);
		dataRegi = stringBitsToNumber(dataRegi);
		addrRegi = stringBitsToNumber(addrRegi);
		string complement = twosComplement(displacement);
		displacement = stringBitsToNumber(displacement);
		displacement = stringNumber_to_hex(displacement);

		int baseInstructionByteNumber = 4;

		if (opmode == "100") {
			outputString.append(".W D");
			outputString.append(dataRegi);
			outputString.append(",($");
			outputString.append(displacement);
			outputString.append(",A");
			outputString.append(addrRegi);
			outputString.append(")");
		} else if (opmode == "101") {
			outputString.append(".L D");
			outputString.append(dataRegi);
			outputString.append(",($");
			outputString.append(displacement);
			outputString.append(",A");
			outputString.append(addrRegi);
			outputString.append(")");
		} else if (opmode == "110") {
			outputString.append(".W ($");
			outputString.append(displacement);
			outputString.append(",A");
			outputString.append(addrRegi);
			outputString.append("),D");
			outputString.append(dataRegi);
		} else if (opmode == "111") {
			outputString.append(".L ($");
			outputString.append(displacement);
			outputString.append(",A");
			outputString.append(addrRegi);
			outputString.append("),D");
			outputString.append(dataRegi);
		} else {
			baseInstructionByteNumber = 0;
		}

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}
		outputString.push_back('\t');
		outputString.append("; Source -> destination, twos complement = ");
		outputString.append(complement);
		outputString.push_back(' ');
		//MOVEP
	} else if (commandArrayString == "MOVEQ") {
		//MOVEQ
		//Move quick
		//immediate data -> destination
		//MOVEQ #<data>,Dn
		//15 14 13 12   11 10 9  8   7 6 5 4 3 2 1 0
		//0  1  1  1    register 0        data
		//

		//add the base command to the string
		string baseCommand = "MOVEQ #";
		outputString.append(baseCommand);

		//collect the portions of the command which determine
		//mode and addresses
		string regis = retrieveBitsFromInstruction(11, 3, commandString);
		string data = retrieveBitsFromInstruction(7, 8, commandString);
		regis = stringBitsToNumber(regis);
		data = stringBitsToNumber(data);

		int baseInstructionByteNumber = 2;

		outputString.append(data);
		outputString.append(",D");
		outputString.append(regis);

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}
		outputString.push_back('\t');
		outputString.append("; Immediate data -> destination");
		//MOVEQ
	} else if (commandArrayString == "MULS") {
		//MULS word
		//signed multiply
		//source x destination -> destination
		//MULS.W <ea>,Dn      16*16 -> 32
		//MULS.L <ea>,Dl      32*32 -> 32
		//MULS.L <ea>,Dh-Dl   32*32 -> 64
		//last two command versions only apply to 68020,68030,68040 and CPU32
		//15 14 13 12  11 10 9   8 7 6   5 4 3  2 1 0
		//1  1  0  0    regis    1 1 1    mode   reg
		//
		//effective address bits 5-0
		//register = data register
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          111      100
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//base of instruction
		outputString.append("MULS.W");

		//collect the portions of the command which determine
		//mode and addresses
		string regis = retrieveBitsFromInstruction(11, 3, commandString);
		regis = stringBitsToNumber(regis); //convert data to decimal
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		extensionBytes = assembleEffectiveAddress(commandString,
				effectiveAddress, 'W');

		//account for special cases
		if (EAmode == "001") {
			//cannot use An as EA
			baseInstructionByteNumber = 0;
		}

		outputString.append(" ");
		outputString.append(effectiveAddress);
		outputString.append(",D");
		outputString.append(regis);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; destination(16 bits)*source(16 bits) -> destination (32 bits)");
		//MULS word
	} else if (commandArrayString == "MULS_follow") {
		//MULS long
		//signed multiply
		//source x destination -> destination
		//MULS.W <ea>,Dn      16*16 -> 32
		//MULS.L <ea>,Dl      32*32 -> 32
		//MULS.L <ea>,Dh-Dl   32*32 -> 64
		//last two command versions only apply to 68020,68030,68040 and CPU32
		//15 14 13 12 11 10 9 8 7 6   5 4 3  2 1 0
		//0  1  0  0  1  1  0 0 0 0    mode   reg
		//
		//-1  -2 -3 -4  -5  -6   -7 -8 -9 -10 -11 -12 -13 -14 -15 -16
		//0  registerDl  1  size  0  0  0  0   0   0   0   registerDh
		//size = 0 ->32 bit, size = 1 -> 64 bit
		//effective address bits 5-0
		//register = data register
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          111      100
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//base of instruction
		outputString.append("MULS.L ");

		//collect the portions of the command which determine
		//mode and addresses
		string size = retrieveBitsFromInstruction(-6, 1, commandString);
		string regisDl = retrieveBitsFromInstruction(-2, 3, commandString);
		regisDl = stringBitsToNumber(regisDl); //convert data to decimal
		string regisDh = retrieveBitsFromInstruction(-14, 3, commandString);
		regisDh = stringBitsToNumber(regisDh); //convert data to decimal
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 4;
		string effectiveAddress = "";
		int extensionBytes = 0;

		extensionBytes = assembleEffectiveAddress(commandString,
				effectiveAddress, 5, 'L', -16);
		outputString.append(effectiveAddress);
		outputString.append(",D");
		//account for special cases
		if (EAmode == "001") {
			//cannot use An as EA
			baseInstructionByteNumber = 0;
		} else if (regisDh == regisDl && size == "1") {
			//this is not allowed
			baseInstructionByteNumber = 0;
		}

		if (size == "1") {
			outputString.append(regisDh);
			outputString.append("-D");
		}
		outputString.append(regisDl);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; destination(32)*source(32 bits) -> destination (32) or destination - source (64 bits)");
		//MULS long
	} else if (commandArrayString == "MULU") {
		//MULU word
		//unsigned multiply
		//source x destination -> destination
		//MULU.W <ea>,Dn      16*16 -> 32
		//MULU.L <ea>,Dl      32*32 -> 32
		//MULU.L <ea>,Dh-Dl   32*32 -> 64
		//last two command versions only apply to 68020,68030,68040 and CPU32
		//15 14 13 12  11 10 9   8 7 6   5 4 3  2 1 0
		//1  1  0  0    regis    0 1 1    mode   reg
		//
		//effective address bits 5-0
		//register = data register
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          111      100
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//base of instruction
		outputString.append("MULU.W");

		//collect the portions of the command which determine
		//mode and addresses
		string regis = retrieveBitsFromInstruction(11, 3, commandString);
		regis = stringBitsToNumber(regis); //convert data to decimal
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		extensionBytes = assembleEffectiveAddress(commandString,
				effectiveAddress, 'W');

		//account for special cases
		if (EAmode == "001") {
			//cannot use An as EA
			baseInstructionByteNumber = 0;
		}

		outputString.append(" ");
		outputString.append(effectiveAddress);
		outputString.append(",D");
		outputString.append(regis);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; destination(16 bits)*source(16 bits) -> destination (32 bits)");
		//MULU word
	} else if (commandArrayString == "MULU_follow") {
		//MULU long
		//unsigned multiply
		//source x destination -> destination
		//MULS.W <ea>,Dn      16*16 -> 32
		//MULS.L <ea>,Dl      32*32 -> 32
		//MULS.L <ea>,Dh-Dl   32*32 -> 64
		//last two command versions only apply to 68020,68030,68040 and CPU32
		//15 14 13 12 11 10 9 8 7 6   5 4 3  2 1 0
		//0  1  0  0  1  1  0 0 0 0    mode   reg
		//
		//-1  -2 -3 -4  -5  -6   -7 -8 -9 -10 -11 -12 -13 -14 -15 -16
		//0  registerDl  0  size  0  0  0  0   0   0   0   registerDh
		//size = 0 ->32 bit, size = 1 -> 64 bit
		//effective address bits 5-0
		//register = data register
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          111      100
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//base of instruction
		outputString.append("MULU.L ");

		//collect the portions of the command which determine
		//mode and addresses
		string size = retrieveBitsFromInstruction(-6, 1, commandString);
		string regisDl = retrieveBitsFromInstruction(-2, 3, commandString);
		regisDl = stringBitsToNumber(regisDl); //convert data to decimal
		string regisDh = retrieveBitsFromInstruction(-14, 3, commandString);
		regisDh = stringBitsToNumber(regisDh); //convert data to decimal
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 4;
		string effectiveAddress = "";
		int extensionBytes = 0;

		extensionBytes = assembleEffectiveAddress(commandString,
				effectiveAddress, 5, 'L', -16);
		outputString.append(effectiveAddress);
		outputString.append(",D");
		//account for special cases
		if (EAmode == "001") {
			//cannot use An as EA
			baseInstructionByteNumber = 0;
		} else if (regisDh == regisDl && size == "1") {
			//this is not allowed
			baseInstructionByteNumber = 0;
		}

		if (size == "1") {
			outputString.append(regisDh);
			outputString.append("-D");
		}
		outputString.append(regisDl);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; destination(32)*source(32 bits) -> destination (32) or destination - source (64 bits)");
		//MULU long
	} else if (commandArrayString == "NBCD") {
		//NBCD
		//negate decimal with extend
		//0 - destination(base10) - X -> destination
		//NBCD <EA>
		//15 14 13 12 11 10 9 8 7 6   5 4 3  2 1 0
		//0  1  0  0  1  0  0 0 0 0    mode   reg
		//
		//effective address bits 5-0
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---

		//base of instruction
		outputString.append("NBCD ");

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;

		extensionBytes = assembleEffectiveAddress(commandString,
				effectiveAddress, 'B');
		outputString.append(effectiveAddress);

		//account for special cases
		if (EAmode == "001" || (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			//cannot use An, #<data>, (d16,PC), (d8,PC,Xn) as EA
			baseInstructionByteNumber = 0;
		}

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append("; 0 - destination(base10) - X -> destination");
		//NBCD
	} else if (commandArrayString == "NEG") {
		//NEG
		//negate
		//0 - destination -> destination
		//NEG <EA>
		//15 14 13 12 11 10 9 8   7 6   5 4 3  2 1 0
		//0  1  0  0  0  1  0 0  size    mode   reg
		//
		//size
		//byte   word   long
		// 00     01     10
		//effective address bits 5-0
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---

		//base of instruction
		outputString.append("NEG");

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string size = retrieveBitsFromInstruction(7, 2, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		if (size == "00") {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'B');
			outputString.append(".B");
		} else if (size == "01") {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'W');
			outputString.append(".W");
		} else if (size == "10") {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'L');
			outputString.append(".L");
		} else {
			baseInstructionByteNumber = 0;
		}
		outputString.append(" ");
		outputString.append(effectiveAddress);

		//account for special cases
		if (EAmode == "001" || (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			//cannot use An, #<data>, (d16,PC), (d8,PC,Xn) as EA
			baseInstructionByteNumber = 0;
		}

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append("; 0 - destination -> destination  i.e. negate");
		//NEG
	} else if (commandArrayString == "NEGX") {
		//NEGX
		// negate with extend
		// 0 - destination - X -> destination
		// NEGX <EA>
		//15 14 13 12 11 10 9 8   7 6   5 4 3  2 1 0
		//0  1  0  0  0  0  0 0  size    mode   reg
		//
		//size
		//byte   word   long
		// 00     01     10
		//effective address bits 5-0
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---

		//base of instruction
		outputString.append("NEGX");

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string size = retrieveBitsFromInstruction(7, 2, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		if (size == "00") {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'B');
			outputString.append(".B");
		} else if (size == "01") {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'W');
			outputString.append(".W");
		} else if (size == "10") {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'L');
			outputString.append(".L");
		} else {
			baseInstructionByteNumber = 0;
		}
		outputString.append(" ");
		outputString.append(effectiveAddress);

		//account for special cases
		if (EAmode == "001" || (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			//cannot use An, #<data>, (d16,PC), (d8,PC,Xn) as EA
			baseInstructionByteNumber = 0;
		}

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; 0 - destination - X -> destination  i.e. negate and extend");
		//NEGX
	} else if (commandArrayString == "NOP") {
		//NOP
		//no operation
		correctForm = 2;
		outputString.append("NOP");

		outputString.push_back('\t');
		outputString.append(
				"; no operation; synchronize pipeline to prevent overlap");
		//NOP
	} else if (commandArrayString == "NOT") {
		//NOT
		//logical compliment
		//~destination -> destination
		//NOT <EA>
		//15 14 13 12 11 10 9 8   7 6   5 4 3  2 1 0
		//0  1  0  0  0  1  1 0  size    mode   reg
		//
		//size
		//byte   word   long
		// 00     01     10
		//effective address bits 5-0
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---

		//base of instruction
		outputString.append("NOT");

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string size = retrieveBitsFromInstruction(7, 2, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		if (size == "00") {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'B');
			outputString.append(".B");
		} else if (size == "01") {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'W');
			outputString.append(".W");
		} else if (size == "10") {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'L');
			outputString.append(".L");
		} else {
			baseInstructionByteNumber = 0;
		}
		outputString.append(" ");
		outputString.append(effectiveAddress);

		//account for special cases
		if (EAmode == "001" || (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			//cannot use An, #<data>, (d16,PC), (d8,PC,Xn) as EA
			baseInstructionByteNumber = 0;
		}

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; logical compliment: ~Destination -> destination");
		//NOT
	} else if (commandArrayString == "OR") {
		//OR
		//inclusive OR logical
		//source OR destination -> destination
		//OR <EA>,Dn or OR Dn,<EA>
		//15 14 13 12 11 10 9   8 7 6   5 4 3  2 1 0
		//1  0  0  0  register  opmode   mode   reg
		//
		//effective address bits 5-0
		//register is any of the 8 Data registers,
		//opmode
		//byte  word  long         operation
		//000   001   010     <ea> OR Dn -> Dn
		//100   101   110     Dn  OR <ea>-> <ea>
		//effective address field: if <ea> is source
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          111      100
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//effective address field: if <ea> is destination
		//addressing mode    mode    register
		//    Dn             ---      --
		//    An*            ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		//*word and long only

		//add the base command to the string
		string baseCommand = "OR";
		outputString.append(baseCommand);

		//collect the portions of the command which determine
		//mode and addresses
		string regist = retrieveBitsFromInstruction(11, 3, commandString);
		regist = stringBitsToNumber(regist);
		string opmode = retrieveBitsFromInstruction(8, 3, commandString);
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (opmode == "000" || opmode == "100") {
			outputString.append(".B");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'B');
		} else if (opmode == "001" || opmode == "101") {
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'W');
		} else if (opmode == "010" || opmode == "110") {
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'L');
		} else {
			baseInstructionByteNumber = 0;//set this to 0 if no valid entries are made
		}

		if (opmode == "000" || opmode == "001" || opmode == "010") {
			outputString.append(" ");
			outputString.append(effectiveAddress);
			outputString.append(",D");
			outputString.append(regist);
		} else if (opmode == "100" || opmode == "101" || opmode == "110") {
			outputString.append(" D");
			outputString.append(regist);
			outputString.append(",");
			outputString.append(effectiveAddress);
		}

		//denote special cases where command may be invalid
		if (EAmode == "001"
				&& (opmode == "000" || opmode == "001" || opmode == "010")) {
			//cannot use An when EA is a source operand
			baseInstructionByteNumber = 0;
		} else if ((EAmode == "000" || EAmode == "001"
				|| (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011"))
				&& (opmode == "100" || opmode == "101" || opmode == "110")) {
			//An, Dn, #<xxx>, (d16,PC), (d8,PC,Xn) cannot be used when EA is a destination operand
			baseInstructionByteNumber = 0;
		}

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}
		outputString.push_back('\t');
		outputString.append(
				"; source OR(inclusive) destination -> destination");
		//OR
	} else if (commandArrayString == "ORI") {
		//ORI
		//Inclusive OR, immediate data
		//Immediate data OR destination -> destination
		//ORI #<data>,<EA>
		//15 14 13 12 11 10 9 8   7 6   5 4 3  2 1 0
		//0  0  0  0  0  0  0 0  size    mode   reg
		//
		//size
		//byte   word   long
		// 00     01     10
		//effective address bits 5-0
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---

		//base of instruction
		outputString.append("ORI");

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string size = retrieveBitsFromInstruction(7, 2, commandString);
		string data = "";

		int baseInstructionByteNumber = 4;
		string effectiveAddress = "";
		int extensionBytes = 0;
		if (size == "00") {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'B', -16);
			outputString.append(".B");
			data = retrieveBitsFromInstruction(-9, 8, commandString);
		} else if (size == "01") {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'W', -16);
			outputString.append(".W");
			data = retrieveBitsFromInstruction(-1, 16, commandString);
		} else if (size == "10") {
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'L', -32);
			outputString.append(".L");
			data = retrieveBitsFromInstruction(-1, 32, commandString);
			baseInstructionByteNumber = 6;
		} else {
			baseInstructionByteNumber = 0;
		}

		data = stringBitsToNumber(data);
		outputString.append(" #");
		outputString.append(data);
		outputString.append(",");
		outputString.append(effectiveAddress);

		//account for special cases
		if (EAmode == "001" || (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			//cannot use An, #<data>, (d16,PC), (d8,PC,Xn) as EA
			baseInstructionByteNumber = 0;
		}

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; immediate data OR(inclusive) destination -> destination");
		//ORI
	} else if (commandArrayString == "ORItoCCR") {
		//ORI to CCR
		//Inclusive OR immediate data to Condition code register
		//ORI #<data>,CCR
		//15 14 13 12 11 10 9 8 7 6 5 4 3 2 1 0
		//0  0  0  0  0  0  0 0 0 0 1 1 1 1 0 0
		//followed by an 8-bit <data>
		//size - byte
		//base of instruction
		outputString.append("ORI");

		//collect the portions of the command which determine
		//mode and addresses
		string data = retrieveBitsFromInstruction(-9, 8, commandString);

		int baseInstructionByteNumber = 4;

		data = stringBitsToNumber(data);
		outputString.append(" #");
		outputString.append(data);
		outputString.append(",CCR");

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append("; immediate data OR(inclusive) CCR -> CCR");
		//ORItoCCR
	} else if (commandArrayString == "PEA") {
		//PEA
		//push effective address
		//SP - 4 -> SP; <ea> -> (SP)
		//PEA <EA>
		//15 14 13 12 11 10 9 8 7 6   5 4 3  2 1 0
		//0  1  0  0  1  0  0 0 0 1    mode   reg
		//
		//size long
		//effective address bits 5-0
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             ---      --
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            ---      --
		//  -(An)            ---      --
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//base of instruction
		outputString.append("PEA ");

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		extensionBytes = assembleEffectiveAddress(commandString,
				effectiveAddress, 'L');

		outputString.append(effectiveAddress);

		//account for special cases
		if (EAmode == "000" || EAmode == "001" || EAmode == "011"
				|| EAmode == "100" || (EAmode == "111" && EAregi == "100")) {
			//cannot use Dn, An, (An)+, -(An), #<data> as EA
			baseInstructionByteNumber = 0;
		}

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append("; SP - 4 -> SP; <ea> -> (SP)");
		//PEA
	} else if (commandArrayString == "ROL_ROR_register") {
		//ROL,ROR register rotate
		//Rotate (without extend)
		//Destination rotated by <count> -> destination
		//ROd Dx,Dy or ROd #<data>,Dy or ROd <ea>
		//where d = L or R
		//15 14 13 12 11 10 9   8   7 6   5   4 3  2 1 0
		//1  1  1  0   count?  D/R  size  i/r 1 1  Regis
		//            register
		//
		//i/r = 0 : count/Register contains shift count
		//1-7 or 0 = 8
		//i/r = 1 : count/Register contains data register
		//which contains shift count, modulo 64
		//
		//D/R specifies direction of shift: 0 = right, 1 = left
		//
		//size
		//byte   word   long
		// 00     01     10

		//base of instruction
		outputString.append("RO");

		//collect the portions of the command which determine
		//mode and addresses
		string count_Register = retrieveBitsFromInstruction(11, 3,
				commandString);
		count_Register = stringBitsToNumber(count_Register); //convert data to decimal
		string DR = retrieveBitsFromInstruction(8, 1, commandString);
		string size = retrieveBitsFromInstruction(7, 2, commandString);
		string ir = retrieveBitsFromInstruction(5, 1, commandString);
		string regis = retrieveBitsFromInstruction(2, 3, commandString);
		regis = stringBitsToNumber(regis);

		int baseInstructionByteNumber = 2;

		if (DR == "0") {
			outputString.append("R");
		} else if (DR == "1") {
			outputString.append("L");
		}

		if (size == "00") {
			outputString.append(".B");
		} else if (size == "01") {
			outputString.append(".W");
		} else if (size == "10") {
			outputString.append(".L");
		} else {
			baseInstructionByteNumber = 0;
		}

		outputString.append(" ");
		if (ir == "0") {
			//immediate count
			outputString.append("#");
			if (count_Register == "0") {
				count_Register = "8";
			}
			outputString.append(count_Register);
			outputString.append(",D");
			outputString.append(regis);
		} else if (ir == "1") {
			//register shift count
			outputString.append("D");
			outputString.append(count_Register);
			outputString.append(",D");
			outputString.append(regis);
		}

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append("; Destination rotated by Count -> Destination");
		//ROL,ROR logical register shift
	} else if (commandArrayString == "ROL_ROR_memory") {
		//ROL,ROR memory rotate
		//Rotate (without extend)
		//Destination rotated by <count> -> destination
		//ROd Dx,Dy or ROd #<data>,Dy or ROd <ea>
		//where d = L or R
		//15 14 13 12 11 10 9   8   7 6   5 4 3  2 1 0
		//1  1  1  0  0  1  1  D/R  1 1  EAmode  EAregi
		//
		//D/R specifies direction of shift: 0 = right, 1 = left
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             ---      --
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---

		//base of instruction
		outputString.append("RO");

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string DR = retrieveBitsFromInstruction(8, 1, commandString);

		int baseInstructionByteNumber = 2;

		if (DR == "0") {
			outputString.append("R");
		} else if (DR == "1") {
			outputString.append("L");
		}

		int extensionBytes = 0;
		string effectiveAddress = "";
		extensionBytes = assembleEffectiveAddress(commandString,
				effectiveAddress, 'W');
		outputString.append(" ");

		//special cases where format is incorrect
		if (EAmode == "000" || EAmode == "001"
				|| (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			baseInstructionByteNumber = 0;
		}

		outputString.append(effectiveAddress);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append("; Destination rotated by 1 bit -> Destination");
		//ROL,ROR memory shift
	} else if (commandArrayString == "ROXL_ROXR_register") {
		//ROXL,ROXR register shift
		//rotate with extend
		//Destination rotated with X by count -> destination
		//ROXd Dx,Dy or ROXd #<data>,Dy or ROXd <EA>
		//where d is direction L or R
		//15 14 13 12 11 10 9   8   7 6   5   4 3  2 1 0
		//1  1  1  0   count?  D/R  size  i/r 1 0  Regis
		//            register
		//
		//i/r = 0 : count/Register contains shift count
		//1-7 or 0 = 8
		//i/r = 1 : count/Register contains data register
		//which contains shift count, modulo 64
		//
		//D/R specifies direction of shift: 0 = right, 1 = left
		//
		//size
		//byte   word   long
		// 00     01     10

		//base of instruction
		outputString.append("ROX");

		//collect the portions of the command which determine
		//mode and addresses
		string count_Register = retrieveBitsFromInstruction(11, 3,
				commandString);
		count_Register = stringBitsToNumber(count_Register); //convert data to decimal
		string DR = retrieveBitsFromInstruction(8, 1, commandString);
		string size = retrieveBitsFromInstruction(7, 2, commandString);
		string ir = retrieveBitsFromInstruction(5, 1, commandString);
		string regis = retrieveBitsFromInstruction(2, 3, commandString);
		regis = stringBitsToNumber(regis);

		int baseInstructionByteNumber = 2;

		if (DR == "0") {
			outputString.append("R");
		} else if (DR == "1") {
			outputString.append("L");
		}

		if (size == "00") {
			outputString.append(".B");
		} else if (size == "01") {
			outputString.append(".W");
		} else if (size == "10") {
			outputString.append(".L");
		} else {
			baseInstructionByteNumber = 0;
		}

		outputString.append(" ");
		if (ir == "0") {
			//immediate count
			outputString.append("#");
			if (count_Register == "0") {
				count_Register = "8";
			}
			outputString.append(count_Register);
			outputString.append(",D");
			outputString.append(regis);
		} else if (ir == "1") {
			//register shift count
			outputString.append("D");
			outputString.append(count_Register);
			outputString.append(",D");
			outputString.append(regis);
		}

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append(
				"; Destination rotated(with extend) by Count -> Destination");
		//ROXL,ROXR logical register shift
	} else if (commandArrayString == "ROXL_ROXR_memory") {
		//ROXL,ROXR memory shift
		//rotate with extend
		//Destination rotated with X by count -> destination
		//ROXd Dx,Dy or ROXd #<data>,Dy or ROXd <EA>
		//where d is direction L or R
		//15 14 13 12 11 10 9   8   7 6   5 4 3  2 1 0
		//1  1  1  0  0  1  0  D/R  1 1  EAmode  EAregi
		//
		//D/R specifies direction of shift: 0 = right, 1 = left
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             ---      --
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---

		//base of instruction
		outputString.append("ROX");

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string DR = retrieveBitsFromInstruction(8, 1, commandString);

		int baseInstructionByteNumber = 2;

		if (DR == "0") {
			outputString.append("R");
		} else if (DR == "1") {
			outputString.append("L");
		}

		int extensionBytes = 0;
		string effectiveAddress = "";
		extensionBytes = assembleEffectiveAddress(commandString,
				effectiveAddress, 'W');
		outputString.append(" ");

		//special cases where format is incorrect
		if (EAmode == "000" || EAmode == "001"
				|| (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			baseInstructionByteNumber = 0;
		}

		outputString.append(effectiveAddress);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; Destination rotated (with extend) by 1 bit -> Destination");
		//ROXL,ROXR memory shift
	} else if (commandArrayString == "RTD") {
		//RTD
		//return and deallocate
		//(SP) -> PC; SP + 4 + dn -> SP
		//RTD #<displacement>
		//15 14 13 12 11 10 9  8  7  6  5 4 3  2 1 0
		//0  1  0  0  1  1  1  0  0  1  1 1 0  1 0 0
		//followed by 16-bit displacement

		//base of instruction
		outputString.append("RTD #");

		//collect the portions of the command which determine
		//mode and addresses
		string displacement = retrieveBitsFromInstruction(-1, 16,
				commandString);
		string complement = twosComplement(displacement);
		displacement = stringBitsToNumber(displacement);

		int baseInstructionByteNumber = 4;

		outputString.append(displacement);

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append(
				"; return and deallocate (SP) -> PC; SP + 4 + dn -> SP, twos complement = ");
		outputString.append(complement);
		outputString.push_back(' ');
		//RTD
	} else if (commandArrayString == "RTR") {
		//RTR
		//return and restore condition codes
		//(SP)-> CCR; SP + 2 -> SP; (SP)-> PC; SP + 4 -> SP
		//RTR
		//15 14 13 12 11 10 9  8  7  6  5 4 3  2 1 0
		//0  1  0  0  1  1  1  0  0  1  1 1 0  1 1 1

		//base of instruction
		outputString.append("RTR");

		int baseInstructionByteNumber = 2;

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append(
				"; return and restore condition codes (SP)-> CCR; SP + 2 -> SP; (SP)-> PC; SP + 4 -> SP");
		//RTR
	} else if (commandArrayString == "RTS") {
		//RTS
		//Return from subroutine
		//(SP) -> PC; SP + 4 -> SP
		//RTS
		//15 14 13 12 11 10 9  8  7  6  5 4 3  2 1 0
		//0  1  0  0  1  1  1  0  0  1  1 1 0  1 0 1

		//base of instruction
		outputString.append("RTS");

		int baseInstructionByteNumber = 2;

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append(
				"; return from subroutine (SP) -> PC; SP + 4 -> SP");
		//RTS
	} else if (commandArrayString == "SBCD") {
		//SBCD
		//Subtract decimal with extend
		//Destination(base10) - source(base10) - X -> destination
		//SBCD Dx,Dy or SBCD -(Ax),-(Ay)
		//15 14 13 12 11 10 9  8 7 6 5 4   3   2 1 0
		//1  0  0  0  Reg. Ry  1 0 0 0 0  R/M  reg.Rx
		//if R/M = 0, Data register, else address register
		string regX = retrieveBitsFromInstruction(2, 3, commandString);
		regX = stringBitsToNumber(regX);
		string regY = retrieveBitsFromInstruction(11, 3, commandString);
		regY = stringBitsToNumber(regY);
		string RM = retrieveBitsFromInstruction(3, 1, commandString);
		if (RM == "0") {
			outputString.append("SBCD D");
			outputString.append(regX);
			outputString.append(",D");
			outputString.append(regY);
		} else {
			outputString.append("SBCD -(A");
			outputString.append(regX);
			outputString.append("),-(A");
			outputString.append(regY);
			outputString.append(")");
		}

		outputString.push_back('\t');
		outputString.append(
				"; Destination(base10) - Source(base10) - X(extendBit) -> Destination");
		correctForm = 2;		//two bytes in instruction
		//SBCD
	} else if (commandArrayString == "Scc") {
		//Scc
		//if condition true, then 1s -> destination; else 1s -> destination
		//Scc <EA>
		//15 14 13 12  11 10 9 8  7 6  5 4 3  2 1 0
		//0  1  0  1   condition  1 1  EAmode EAregi
		//size = byte
		//conditional tests
		//Mnemonic     condition   encoding
		//  T             True       0000
		//  F             False      0001
		//  HI            High       0010
		//  LS        Low or same    0011
		// CC(HI)     carry clear    0100
		// CS(LO)      Carry set     0101
		//  NE          not equal    0110
		//  EQ            equal      0111
		//  VC       overflow clear  1000
		//  VS       Overflow set    1001
		//  PL             Plus      1010
		//  MI            Minus		 1011
		//  GE      Greater or equal 1100
		//  LT          Less than    1101
		//  GT        greater than   1110
		//  LE       Less or equal   1111
		//
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---

		//collect the bits to determine the instruction
		string conditionCode = retrieveBitsFromInstruction(11, 4,
				commandString);
		string condition = "";
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);

		int baseInstructionByteNumber = 2;

		//base of command
		outputString.append("S");
		//determine condition
		if (conditionCode == "0000") {
			condition = "True";
			conditionCode = "T";
		} else if (conditionCode == "0001") {
			condition = "False";
			conditionCode = "F";
		} else if (conditionCode == "0010") {
			condition = "HI";
			conditionCode = "HI";
		} else if (conditionCode == "0011") {
			condition = "Low or same";
			conditionCode = "LS";
		} else if (conditionCode == "0100") {
			condition = "Carry clear";
			conditionCode = "CC(HI)";
		} else if (conditionCode == "0101") {
			condition = "Carry set";
			conditionCode = "CS(LO)";
		} else if (conditionCode == "0110") {
			condition = "Not equal";
			conditionCode = "NE";
		} else if (conditionCode == "0111") {
			condition = "Equal";
			conditionCode = "EQ";
		} else if (conditionCode == "1000") {
			condition = "Overflow clear";
			conditionCode = "VC";
		} else if (conditionCode == "1001") {
			condition = "Overflow set";
			conditionCode = "VS";
		} else if (conditionCode == "1010") {
			condition = "Plus";
			conditionCode = "PL";
		} else if (conditionCode == "1011") {
			condition = "Minus";
			conditionCode = "MI";
		} else if (conditionCode == "1100") {
			condition = "Greater or equal";
			conditionCode = "GE";
		} else if (conditionCode == "1101") {
			condition = "Less than";
			conditionCode = "LT";
		} else if (conditionCode == "1110") {
			condition = "Greater than";
			conditionCode = "GT";
		} else if (conditionCode == "1111") {
			condition = "Less or equal";
			conditionCode = "LE";
		} else {
			baseInstructionByteNumber = 0;
		}

		outputString.append(conditionCode);
		outputString.append(" ");
		int extensionBytes = 0;
		string effectiveAddress = "";
		extensionBytes = assembleEffectiveAddress(commandString,
				effectiveAddress, 'B');
		outputString.append(effectiveAddress);

		//special cases where format is incorrect
		if (EAmode == "001" || (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			baseInstructionByteNumber = 0;
		}

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append("; if condition (");
		outputString.append(condition);
		outputString.append(
				") true, then 1s -> destination; else 0s -> destination");
		//Scc
	} else if (commandArrayString == "SUB") {
		//SUB
		//subtract
		//Destination - source -> destination
		//SUB <ea>,Dn or SUB Dn,<EA>
		//15 14 13 12 11 10 9   8 7 6  5 4 3  2 1 0
		//1  0  0  1  register  opmode  mode   reg
		//effective address bits 5-0
		//register is any of the 8 Data registers,
		//opmode
		//byte  word  long         operation
		//                  destin   src   destn
		//000   001   010     Dn  - <ea>-> Dn
		//100   101   110     <ea> - Dn -> <ea>
		//effective address field: if <ea> is source
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An*            001      An
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          111      100
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//effective address field: if <ea> is destination
		//addressing mode    mode    register
		//    Dn             ---      --
		//    An*            ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		//*byte only

		//add the base command to the string
		string baseCommand = "SUB";
		outputString.append(baseCommand);

		//collect the portions of the command which determine
		//mode and addresses
		string regist = retrieveBitsFromInstruction(11, 3, commandString);
		regist = stringBitsToNumber(regist);
		string opmode = retrieveBitsFromInstruction(8, 3, commandString);
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (opmode == "000" || opmode == "100") {
			outputString.append(".B");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'B');
		} else if (opmode == "001" || opmode == "101") {
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'W');
		} else if (opmode == "010" || opmode == "110") {
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'L');
		} else {
			baseInstructionByteNumber = 0;//set this to 0 if no valid entries are made
		}

		if (opmode == "100" || opmode == "101" || opmode == "110") {
			outputString.append(" D");
			outputString.append(regist);
			outputString.append(",");
			outputString.append(effectiveAddress);
		} else if (opmode == "000" || opmode == "001" || opmode == "010") {
			outputString.append(" ");
			outputString.append(effectiveAddress);
			outputString.append(",D");
			outputString.append(regist);
		}

		//denote special cases where command may be invalid
		if (EAmode == "001"
				&& (opmode == "000" || opmode == "100" || opmode == "101"
						|| opmode == "110")) {
			//An is only word or long, and only when EA is a source operand
			baseInstructionByteNumber = 0;
		} else if ((EAmode == "000" || (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011"))
				&& (opmode == "100" || opmode == "101" || opmode == "110")) {
			//Dn, #<xxx>, (d16,PC), (d8,PC,Xn) are only when EA is a source operand
			baseInstructionByteNumber = 0;
		}

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}
		outputString.push_back('\t');
		outputString.append("; destination -  source -> destination");
		//SUB
	} else if (commandArrayString == "SUBA") {
		//SUBA
		//Subtract address
		//Destination - source -> destination
		//SUBA <EA>,An
		//15 14 13 12 11 10 9   8 7 6  5 4 3  2 1 0
		//1  0  0  1  register  opmode  mode   reg
		//effective address bits 5-0
		//register is any of the 8 address registers,
		//opmode
		//word  long         operation
		//011   111     An - <ea> -> An
		//effective address field: <ea> is source
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             001      An
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          111      100
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//base of instruction
		outputString.append("SUBA");

		//collect the portions of the command which determine
		//mode and addresses
		string regist = retrieveBitsFromInstruction(11, 3, commandString);
		regist = stringBitsToNumber(regist);
		string opmode = retrieveBitsFromInstruction(8, 3, commandString);
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (opmode == "011") {
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'W');
		} else if (opmode == "111") {
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'L');
		} else {
			baseInstructionByteNumber = 0;//set this to 0 if no valid entries are made
		}

		outputString.append(" ");
		outputString.append(effectiveAddress);
		outputString.append(",A");
		outputString.append(regist);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append("; destination - source -> destination");
		//SUBA
	} else if (commandArrayString == "SUBI") {
		//SUBI
		//subtract immediate
		//Destination - immediate data -> destination
		//SUBI #<data>,<EA>
		//15 14 13 12 11 10 9  8  7 6   5 4 3  2 1 0
		//0  0  0  0  0  1  0  0  size   mode   reg
		//effective address bits 5-0
		//size
		//byte  word  long
		//00     01    10
		//effective address field: <ea> is destination
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---

		//base of instruction
		outputString.append("SUBI");

		//collect the portions of the command which determine
		//mode and addresses
		string size = retrieveBitsFromInstruction(7, 2, commandString);
		string data = "";
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (size == "00") {
			outputString.append(".B");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'B', -16);
			data = retrieveBitsFromInstruction(-9, 8, commandString);
			data = stringBitsToNumber(data); //convert data to decimal
			baseInstructionByteNumber = 4;
		} else if (size == "01") {
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'W', -16);
			data = retrieveBitsFromInstruction(-1, 16, commandString);
			data = stringBitsToNumber(data); //convert data to decimal
			baseInstructionByteNumber = 4;
		} else if (size == "10") {
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'L', -32);
			data = retrieveBitsFromInstruction(-1, 32, commandString);
			data = stringBitsToNumber(data); //convert data to decimal
			baseInstructionByteNumber = 6;
		} else {
			baseInstructionByteNumber = 0; //set this to 0 if no valid entries are made
		}

		//account for special cases
		if (EAmode == "001" || (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			//cannot use An, #<data>, (d16,PC), (d8,PC,Xn) or (bd,PC,Xn) as EA
			baseInstructionByteNumber = 0;
		}

		outputString.append(" #");
		outputString.append(data);
		outputString.append(",");
		outputString.append(effectiveAddress);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append("; destination - immediate data -> destination");
		//SUBI
	} else if (commandArrayString == "SUBQ") {
		//SUBQ
		//Subtract Quick
		//Destination - immediate data -> destination
		//SUBQ #<data>,<EA>
		//15 14 13 12 11 10 9  8  7 6   5 4 3  2 1 0
		//0  1  0  1    data   1  size   mode   reg
		//effective address bits 5-0
		//size
		//byte  word  long
		//00     01    10
		//effective address field: <ea> is destination
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             001      An
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---

		//base of instruction
		outputString.append("SUBQ");

		//collect the portions of the command which determine
		//mode and addresses
		string size = retrieveBitsFromInstruction(7, 2, commandString);
		string data = retrieveBitsFromInstruction(11, 3, commandString);
		data = stringBitsToNumber(data); //convert data to decimal
		if (data == "0") {
			data = "8";
		}
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (size == "00") {
			outputString.append(".B");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'B');
		} else if (size == "01") {
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'W');
		} else if (size == "10") {
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'L');
		} else {
			baseInstructionByteNumber = 0; //set this to 0 if no valid entries are made
		}

		//account for special cases
		if ((EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			//cannot use #<data>, (d16,PC), (d8,PC,Xn) or (bd,PC,Xn) as EA
			baseInstructionByteNumber = 0;
		}

		outputString.append(" #");
		outputString.append(data);
		outputString.append(",");
		outputString.append(effectiveAddress);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append("; destination - immediate data -> destination");
		//SUBQ
	} else if (commandArrayString == "SUBX") {
		//SUBX
		//Subtract with extend
		//Destination - source - X -> destination
		//SUBX Dx,Dy or SUBX -(Ay),-(Ax)
		//15 14 13 12 11 10 9  8  7 6  5 4  3   2 1 0
		//1  0  0  1    regY   1  size 0 0 R/M  regX
		//R/M = 0 -> data registers; = 1 -> address registers
		//size
		//byte  word  long
		//00     01    10

		//base of instruction
		outputString.append("SUBX");

		//collect the portions of the command which determine
		//mode and addresses
		string size = retrieveBitsFromInstruction(7, 2, commandString);
		string regX = retrieveBitsFromInstruction(2, 3, commandString);
		regX = stringBitsToNumber(regX); //convert data to decimal
		string regY = retrieveBitsFromInstruction(11, 3, commandString);
		regY = stringBitsToNumber(regY); //convert data to decimal
		string RM = retrieveBitsFromInstruction(3, 1, commandString);

		int baseInstructionByteNumber = 2;
		//add the size of the base command to the string
		if (size == "00") {
			outputString.append(".B");
		} else if (size == "01") {
			outputString.append(".W");
		} else if (size == "10") {
			outputString.append(".L");
		} else {
			baseInstructionByteNumber = 0; //set this to 0 if no valid entries are made
		}

		outputString.append(" ");
		if (RM == "0") {
			//data registers
			outputString.append("D");
			outputString.append(regX);
			outputString.append(",D");
			outputString.append(regY);
		} else if (RM == "1") {
			outputString.append("-(A");
			outputString.append(regX);
			outputString.append("),-(A");
			outputString.append(regY);
			outputString.append(")");
		}

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append("; destination - source - X -> destination");
		//SUBX
	} else if (commandArrayString == "SWAP") {
		//SWAP
		//Swap register halves
		//Register (31 thru 16) -> register (15 thru 0)
		//SWAP Dn
		//15 14 13 12 11 10 9  8  7 6  5 4  3   2 1 0
		//0  1  0  0  1  0  0  0  0 1  0 0  0   regis
		//
		//size = word

		//base of instruction
		outputString.append("SWAP D");

		//collect the portions of the command which determine
		//mode and addresses
		string regis = retrieveBitsFromInstruction(2, 3, commandString);
		regis = stringBitsToNumber(regis); //convert data to decimal

		outputString.append(regis);

		int baseInstructionByteNumber = 2;
		correctForm = baseInstructionByteNumber;

		outputString.push_back('\t');
		outputString.append("; Register (31 thru 16) -> register (15 thru 0)");
		//SWAP
	} else if (commandArrayString == "TAS") {
		//TAS
		//test and set an operand
		//Destination tested -> condition codes; 1 -> bit 7 of destinaion
		//TAS <EA>
		//15 14 13 12 11 10 9  8  7 6   5 4 3  2 1 0
		//0  1  0  0   1  0 1  0  1 1   EAmode EAregi
		//effective address bits 5-0
		//size = byte
		//effective address field: <ea> is destination
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---

		//base of instruction
		outputString.append("TAS ");

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;

		extensionBytes = assembleEffectiveAddress(commandString,
				effectiveAddress, 'B');

		//account for special cases
		if (EAmode == "001" || (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			//cannot use An, #<data>, (d16,PC), (d8,PC,Xn) or (bd,PC,Xn) as EA
			baseInstructionByteNumber = 0;
		}

		outputString.append(effectiveAddress);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; Destination tested -> condition codes; 1 -> bit 7 of destinaion");
		//TAS
	} else if (commandArrayString == "TRAP") {
		//TRAP
		//Trap
		//1 -> S-bit of SR
		//*SSP - 2 -> SSP; format/offset -> (SSP);
		//SSP - 4 -> SSP; PC -> (SSP); SSP - 2 -> SSP;
		//SR -> (SSP); vector address -> PC
		//TRAP #<vector>
		//15 14 13 12 11 10 9  8  7 6   5 4  3 2 1 0
		//0  1  0  0   1  1 1  0  0 1   0 0  vector

		//base of instruction
		outputString.append("TRAP ");

		//collect the portions of the command which determine
		//mode and addresses
		string vector = retrieveBitsFromInstruction(3, 4, commandString);
		vector = stringBitsToNumber(vector);

		int baseInstructionByteNumber = 2;

		outputString.append("#");
		outputString.append(vector);

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append(
				"; 		//1 -> S-bit of SR; *SSP - 2 -> SSP; format/offset -> (SSP); SSP - 4 -> SSP; PC -> (SSP); SSP - 2 -> SSP; SR -> (SSP); vector address -> PC");
		//TRAP
	} else if (commandArrayString == "TRAPcc") {
		//TRAPcc
		//trap on condition
		//if cc, then TRAP
		//TRAPcc or TRAPcc.W #<data> or TRAPcc.L #<data>
		//15 14 13 12  11 10 9 8  7 6  5 4 3  2 1 0
		//0  1  0  1   condition  1 1  1 1 1  opmode
		//opmode
		//010  = word
		//011 long
		//100 no following bits
		//conditional tests
		//Mnemonic     condition   encoding
		//  T             True       0000
		//  F             False      0001
		//  HI            High       0010
		//  LS        Low or same    0011
		// CC(HI)     carry clear    0100
		// CS(LO)      Carry set     0101
		//  NE          not equal    0110
		//  EQ            equal      0111
		//  VC       overflow clear  1000
		//  VS       Overflow set    1001
		//  PL             Plus      1010
		//  MI            Minus		 1011
		//  GE      Greater or equal 1100
		//  LT          Less than    1101
		//  GT        greater than   1110
		//  LE       Less or equal   1111

		//collect the bits to determine the instruction
		string conditionCode = retrieveBitsFromInstruction(11, 4,
				commandString);
		string condition = "";
		string opmode = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;

		//base of command
		outputString.append("TRAP");
		//determine condition
		if (conditionCode == "0000") {
			condition = "True";
			conditionCode = "T";
		} else if (conditionCode == "0001") {
			condition = "False";
			conditionCode = "F";
		} else if (conditionCode == "0010") {
			condition = "HI";
			conditionCode = "HI";
		} else if (conditionCode == "0011") {
			condition = "Low or same";
			conditionCode = "LS";
		} else if (conditionCode == "0100") {
			condition = "Carry clear";
			conditionCode = "CC(HI)";
		} else if (conditionCode == "0101") {
			condition = "Carry set";
			conditionCode = "CS(LO)";
		} else if (conditionCode == "0110") {
			condition = "Not equal";
			conditionCode = "NE";
		} else if (conditionCode == "0111") {
			condition = "Equal";
			conditionCode = "EQ";
		} else if (conditionCode == "1000") {
			condition = "Overflow clear";
			conditionCode = "VC";
		} else if (conditionCode == "1001") {
			condition = "Overflow set";
			conditionCode = "VS";
		} else if (conditionCode == "1010") {
			condition = "Plus";
			conditionCode = "PL";
		} else if (conditionCode == "1011") {
			condition = "Minus";
			conditionCode = "MI";
		} else if (conditionCode == "1100") {
			condition = "Greater or equal";
			conditionCode = "GE";
		} else if (conditionCode == "1101") {
			condition = "Less than";
			conditionCode = "LT";
		} else if (conditionCode == "1110") {
			condition = "Greater than";
			conditionCode = "GT";
		} else if (conditionCode == "1111") {
			condition = "Less or equal";
			conditionCode = "LE";
		} else {
			baseInstructionByteNumber = 0;
		}

		outputString.append(conditionCode);

		if (opmode == "010") {
			outputString.append(".W #");
			string data = retrieveBitsFromInstruction(-1, 16, commandString);
			data = stringBitsToNumber(data);
			outputString.append(data);
			baseInstructionByteNumber = 4;
		} else if (opmode == "011") {
			outputString.append(".L #");
			string data = retrieveBitsFromInstruction(-1, 32, commandString);
			data = stringBitsToNumber(data);
			outputString.append(data);
			baseInstructionByteNumber = 6;
		} else if (opmode == "100") {

		} else {
			baseInstructionByteNumber = 0;
		}

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append("; if condition (");
		outputString.append(condition);
		outputString.append(") true, then TRAP");
		//TRAPcc
	} else if (commandArrayString == "TRAPV") {
		//TRAPV
		//trap on overflow
		//If V then TRAP
		//TRAPV
		//15 14 13 12  11 10 9 8  7 6  5 4 3  2 1 0
		//0  1  0  0   1  1  1 0  0 1  1 1 0  1 1 0

		int baseInstructionByteNumber = 2;

		//base of command
		outputString.append("TRAPV");

		correctForm = baseInstructionByteNumber;

		outputString.push_back('\t');
		outputString.append("; if V then TRAP");
		//TRAPcc
	} else if (commandArrayString == "TST") {
		//TST
		//Test an operand
		//Destination tested -> condition codes
		//TST <EA>
		//15 14 13 12 11 10 9  8  7 6   5 4 3  2 1 0
		//0  1  0  0  1  0  1  0  size   mode   reg
		//effective address bits 5-0
		//size
		//byte  word  long
		//00     01    10
		//effective address field: <ea> is destination
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             001      An
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          111      100
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//base of instruction
		outputString.append("TST");

		//collect the portions of the command which determine
		//mode and addresses
		string size = retrieveBitsFromInstruction(7, 2, commandString);
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (size == "00") {
			outputString.append(".B");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'B');
		} else if (size == "01") {
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'W');
		} else if (size == "10") {
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 'L');
		} else {
			baseInstructionByteNumber = 0;//set this to 0 if no valid entries are made
		}

		//account for special cases
		if (size == "00" && EAmode == "001") {
			//(An) is only word or long for CPU32
			baseInstructionByteNumber = 0;
		}

		outputString.append(" ");
		outputString.append(effectiveAddress);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append("; Destination tested vs 0 -> condition codes set");
		//TST
	} else if (commandArrayString == "UNLK") {
		//UNLK
		//unlink
		//An -> SP; (SP) -> An; SP + 4 -> SP
		//UNLK An
		//15 14 13 12 11 10 9  8  7 6   5 4 3  2 1 0
		//0  1  0  0  1  1  1  0  0 1   0 1 1 register

		//base of instruction
		outputString.append("UNLK A");

		//collect the portions of the command which determine
		//mode and addresses
		string regis = retrieveBitsFromInstruction(2, 3, commandString);
		regis = stringBitsToNumber(regis);

		int baseInstructionByteNumber = 2;

		outputString.append(regis);

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append("; unlink: An -> SP; (SP) -> An; SP + 4 -> SP");
		//UNLK
	} else if (commandArrayString == "ANDItoSR") {
		//ANDI to SR
		//and immediate to status register
		//if supervisor state then source L SR -> SR else trap
		//ANDI #<data>,SR
		//15 14 13 12 11 10 9  8  7 6  5 4 3 2 1 0
		//0  0  0  0  0  0  1  0  0 1  1 1 1 1 0 0

		//base of instruction
		outputString.append("ANDI #");

		//collect the portions of the command which determine
		//mode and addresses
		string data = retrieveBitsFromInstruction(-1, 16, commandString);
		data = stringBitsToNumber(data); //convert data to decimal

		int baseInstructionByteNumber = 4;

		outputString.append(data);
		outputString.append(",SR");

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append(
				"; if supervisor state then source L SR -> SR else trap");
		//ANDI to SR
	} else if (commandArrayString == "EORItoSR") {
		//EORI to SR
		//exclusive or immediate to status register
		//if supervisor state then source OR SR -> SR else TRAP
		//EORI #<data>,SR
		//15 14 13 12 11 10 9  8  7 6  5 4 3 2 1 0
		//0  0  0  0  1  0  1  0  0 1  1 1 1 1 0 0

		//base of instruction
		outputString.append("EORI");

		//collect the portions of the command which determine
		//mode and addresses
		string data = retrieveBitsFromInstruction(-1, 16, commandString);
		data = stringBitsToNumber(data); //convert data to decimal

		int baseInstructionByteNumber = 4;

		outputString.append(" #");
		outputString.append(data);
		outputString.append(",SR");

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append(
				"; if supervisor state then source OR(exclusive) SR -> SR else TRAP");
		//EORI to SR
	} else if (commandArrayString == "MOVEfromSR") {
		//MOVE from SR
		//move from the status register
		//if supervisor state then SR -> Destination else TRAP
		//MOVE SR,<EA>
		//15 14 13 12 11 10 9  8  7 6   5 4 3  2 1 0
		//0  1  0  0   0  0 0  0  1 1   EAmode EAregi
		//effective address bits 5-0
		//size = word
		//effective address field: <ea> is destination
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---

		//base of instruction
		outputString.append("MOVE SR,");

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;

		extensionBytes = assembleEffectiveAddress(commandString,
				effectiveAddress, 'W');

		//account for special cases
		if (EAmode == "001" || (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			//cannot use An, #<data>, (d16,PC), (d8,PC,Xn) or (bd,PC,Xn) as EA
			baseInstructionByteNumber = 0;
		}

		outputString.append(effectiveAddress);

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; if supervisor state then SR -> Destination else TRAP");
		//MOVE from SR
	} else if (commandArrayString == "MOVEtoSR") {
		//MOVE to SR
		//move to the status register
		//if supervisor state then source -> SR else TRAP
		//MOVE <EA>,SR
		//15 14 13 12 11 10 9  8  7 6   5 4 3  2 1 0
		//0  1  0  0   0  1 1  0  1 1   EAmode EAregi
		//effective address bits 5-0
		//size = word
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             000      Dn
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          111      100
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//base of instruction
		outputString.append("MOVE");

		//collect the portions of the command which determine
		//mode and addresses
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 2;
		string effectiveAddress = "";
		int extensionBytes = 0;

		extensionBytes = assembleEffectiveAddress(commandString,
				effectiveAddress, 'W');

		//account for special cases
		if (EAmode == "001") {
			//cannot use An as EA
			baseInstructionByteNumber = 0;
		}
		outputString.append(" ");
		outputString.append(effectiveAddress);
		outputString.append(",SR");

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; if supervisor state then source -> SR else TRAP");
		//MOVE to SR
	} else if (commandArrayString == "MOVE_USP") {
		//MOVE USP
		//move user stack pointer
		//if supervisor state then USP -> An or An -> USP else TRAP
		//MOVE USP,An or MOVE An,USP
		//15 14 13 12 11 10 9  8  7 6   5 4 3  2 1 0
		//0  1  0  0   1  1 1  0  0 1   1 0 DR register
		//if DR = 0, transfer An to SP
		//if DR = 1, transfer SP to An
		//effective address bits 5-0
		//size = long

		//base of instruction
		outputString.append("MOVE");

		//collect the portions of the command which determine
		//mode and addresses
		string regis = retrieveBitsFromInstruction(2, 3, commandString);
		regis = stringBitsToNumber(regis);
		string DR = retrieveBitsFromInstruction(3, 1, commandString);

		int baseInstructionByteNumber = 2;

		//account for special cases
		if (DR == "0") {
			outputString.append(" A");
			outputString.append(regis);
			outputString.append(",USP");
		} else if (DR == "1") {
			outputString.append(" USP,A");
			outputString.append(regis);
		}

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append(
				"; if supervisor state then USP -> An or An -> USP else TRAP");
		//MOVE to USP
	} else if (commandArrayString == "MOVEC") {
		//MOVEC
		//move control register
		//if supervisor state then Rc -> Rn or Rn -> Rc else TRAP
		//MOVEC Rc,Rn or MOVEC Rn,Rc
		//15 14 13 12 11 10 9  8  7 6   5 4 3  2 1 0
		//0  1  0  0   1  1 1  0  0 1   1 1 1  0 1 DR

		//-1   -2 -3 -4  -5 -6 -7 -8 -9 -10 -11 -12 -13 -14 -15 -16
		//A/D  register               control register
		//if DR = 0, control register to general register
		//if DR = 1, general register to control register
		//if A/D = 0 -> data register
		//if A/D = 1 -> address register
		//size = long
		//if control register = (hex)
		//000  source function code (SFC)
		//001  destination function code (DFC)
		//800  user stack pointer (USP)
		//810 vector base register (VBR)

		//base of instruction
		outputString.append("MOVEC");

		//collect the portions of the command which determine
		//mode and addresses
		string regis = retrieveBitsFromInstruction(-2, 3, commandString);
		regis = stringBitsToNumber(regis);
		string DR = retrieveBitsFromInstruction(0, 1, commandString);
		string AD = retrieveBitsFromInstruction(-1, 1, commandString);
		string controlRegister = retrieveBitsFromInstruction(-5, 12,
				commandString);
		controlRegister = stringBitsToNumber(controlRegister);
		controlRegister = stringNumber_to_hex(controlRegister);

		int baseInstructionByteNumber = 4;
		string generalRegister = "";
		if (AD == "0") {
			generalRegister = "D";
		} else if (AD == "1") {
			generalRegister = "A";
		}

		string controlRegisterCode = "";
		if (controlRegister == "000" || controlRegister == "0") {
			controlRegisterCode = "SFC";
		} else if (controlRegister == "001" || controlRegister == "1") {
			controlRegisterCode = "DFC";
		} else if (controlRegister == "800") {
			controlRegisterCode = "USP";
		} else if (controlRegister == "801") {
			controlRegisterCode = "VBR";
		} else {
			baseInstructionByteNumber = 0;
		}

		//account for special cases
		if (DR == "1") {
			outputString.append(" ");
			outputString.append(generalRegister);
			outputString.append(regis);
			outputString.append(",");
			outputString.append(controlRegisterCode);
		} else if (DR == "0") {
			outputString.append(" ");
			outputString.append(controlRegisterCode);
			outputString.append(",");
			outputString.append(generalRegister);
			outputString.append(regis);
		}

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append(
				"; if supervisor state then Rc -> Rn or Rn -> Rc else TRAP");
		//MOVEC
	} else if (commandArrayString == "MOVES") {
		//MOVES
		//move address space
		//if supervisor state then Rn -> destination [DFC] or source [SFC] -> Rn else TRAP
		//MOVES Rn,<EA> or MOVES <EA>,Rn
		//15 14 13 12 11 10 9  8  7 6   5 4 3  2 1 0
		//0  0  0  0   1  1 1  0  size  EAmode EAregi

		//-1   -2 -3 -4  -5 -6 -7 -8 -9 -10 -11 -12 -13 -14 -15 -16
		//A/D  register   DR 0  0  0  0  0    0   0   0   0   0  0
		//if DR = 0, from <EA> to general register
		//if DR = 1, from general register to <EA>
		//if A/D = 0 -> data register
		//if A/D = 1 -> address register
		//byte   word   long
		//00      01     10
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             ---      --
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            011      An
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         ---      ---
		//  (d8,PC,Xn)       ---      ---
		// (bd,PC,Xn)		 ---      ---

		//base of instruction
		outputString.append("MOVES");

		//collect the portions of the command which determine
		//mode and addresses
		string regis = retrieveBitsFromInstruction(-2, 3, commandString);
		regis = stringBitsToNumber(regis);
		string DR = retrieveBitsFromInstruction(-5, 1, commandString);
		string AD = retrieveBitsFromInstruction(-1, 1, commandString);
		string size = retrieveBitsFromInstruction(7, 2, commandString);
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 4;
		string generalRegister = "";
		if (AD == "0") {
			generalRegister = "D";
		} else if (AD == "1") {
			generalRegister = "A";
		}

		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (size == "00") {
			outputString.append(".B");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'B', -16);
		} else if (size == "01") {
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'W', -16);
		} else if (size == "10") {
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'L', -16);
		} else {
			baseInstructionByteNumber = 0;//set this to 0 if no valid entries are made
		}

		//account for special cases
		if (DR == "1") {
			outputString.append(" ");
			outputString.append(generalRegister);
			outputString.append(regis);
			outputString.append(",");
			outputString.append(effectiveAddress);
		} else if (DR == "0") {
			outputString.append(" ");
			outputString.append(effectiveAddress);
			outputString.append(",");
			outputString.append(generalRegister);
			outputString.append(regis);
		}

		//account for special unallowed cases
		if (EAmode == "000" || EAmode == "001"
				|| (EAmode == "111" && EAregi == "100")
				|| (EAmode == "111" && EAregi == "010")
				|| (EAmode == "111" && EAregi == "011")) {
			baseInstructionByteNumber = 0;
		}

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; if supervisor state then Rn -> destination [DFC] or source [SFC] -> Rn else TRAP");
		//MOVES
	} else if (commandArrayString == "ORItoSR") {
		//ORI to SR
		//inclusive OR immediate to status register
		//if supervisor state then source OR SR -> SR else trap
		//ORI #<data>,SR
		//15 14 13 12 11 10 9  8  7 6  5 4 3 2 1 0
		//0  0  0  0  0  0  0  0  0 1  1 1 1 1 0 0

		//base of instruction
		outputString.append("ORI");

		//collect the portions of the command which determine
		//mode and addresses
		string data = retrieveBitsFromInstruction(-1, 16, commandString);
		data = stringBitsToNumber(data); //convert data to decimal

		int baseInstructionByteNumber = 4;

		outputString.append(" #");
		outputString.append(data);
		outputString.append(",SR");

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append(
				"; if supervisor state then source OR SR -> SR else trap");
		//ORI to SR
	} else if (commandArrayString == "RESET") {
		//RESET
		//reset external devices
		//if supervisor state then assert RESERT line else trap
		//RESET
		//15 14 13 12 11 10 9  8  7 6  5 4 3 2 1 0
		//0  1  0  0  1  1  1  0  0 1  1 1 0 0 0 0

		//base of instruction
		outputString.append("RESET");

		int baseInstructionByteNumber = 2;

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append(
				"; if supervisor state then assert RESERT line else trap");
		//RESET
	} else if (commandArrayString == "RTE") {
		//RTE
		//return from exception
		//if supervisor state then (SP) -> SR; SP + 2 -> SP; (SP) -> PC; SP + 4 -> SP; restore state and deallocate stack according to (SP) else TRAP
		//RTE
		//15 14 13 12 11 10 9  8  7 6 5 4 3  2 1 0
		//0  1  0  0   1  1 1  0  0 1 1 1 0  0 1 1
		//the format is as follows:
		//-1 -2 -3 -4  -5 -6   -7 -8 -9 -10 -11 -12 -13 -14 -15 -16
		//  format      0  0               vector offset
		//vector assignment can be any number 0-255 excepting 12, 16-23, 59-63. some of
		//64-255 (which are user defined) may be unused.
		//vector offset can not be a handful of hex numbers
		//000,004,008,00C,010,014,018,01C,020,024,028,02C,034,038,03C,060,064,068,06C,070,074,078,07C,080-08C,0C0,0C4,0C8,0CC,0D0,0D4,0D8,0DC,0E0,0E4,0E8,100D3FC
		string vectorOffset = retrieveBitsFromInstruction(-7, 10,
				commandString);
		vectorOffset = stringBitsToNumber(vectorOffset);
		vectorOffset = stringNumber_to_hex(vectorOffset);

		//base of instruction
		outputString.append("RTE");

		int baseInstructionByteNumber = 2; //or 4? confused about whether both words are needed, or just the one according to the format

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append(
				"; if supervisor state then (SP) -> SR; SP + 2 -> SP; (SP) -> PC; SP + 4 -> SP; restore state and deallocate stack according to (SP) else TRAP");
		//RTE
	} else if (commandArrayString == "STOP") {
		//STOP
		//load status register and stop
		//if supervisor state then immediate data -> SR; STOP else TRAP
		//STOP #<data>
		//15 14 13 12 11 10 9  8  7 6 5 4 3  2 1 0
		//0  1  0  0   1  1 1  0  0 1 1 1 0  0 1 0
		//+ 16 bit immediate data

		string data = retrieveBitsFromInstruction(-1, 16, commandString);
		data = stringBitsToNumber(data);

		//base of instruction
		outputString.append("RTE #");
		outputString.append(data);

		int baseInstructionByteNumber = 4;

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append(
				"; if supervisor state then immediate data -> SR; STOP else TRAP");
		//STOP
	} else if (commandArrayString == "BGND") {
		//BGND
		//enter background mode
		//if background mode enabled then enter background mode ELSE format/vector offset -> -(SSP); PC -> -(SSP); SR -> -(SSP); (Vector) -> PC
		//BGND
		//15 14 13 12 11 10 9  8  7 6 5 4 3  2 1 0
		//0  1  0  0   1  0 1  0  1 1 1 1 1  0 1 0

		//base of instruction
		outputString.append("BGND");

		int baseInstructionByteNumber = 2;

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append(
				"; if background mode enabled then enter background mode ELSE format/vector offset -> -(SSP); PC -> -(SSP); SR -> -(SSP); (Vector) -> PC");
		//BGND
	} else if (commandArrayString == "LPSTOP") {
		//LPSTOP
		//low power stop
		//if supervisor state then immediate data -> SR; interrupt mask -> external bus interface; STOP else TRAP
		//LPSTOP #<data>
		//15 14 13 12 11 10 9  8  7 6 5 4 3  2 1 0
		//1  1  1  1   1  0 0  0  0 0 0 0 0  0 0 0

		//-1 -2 -3 -4 -5 -6 -7 -8 -9 -10 -11 -12 -13 -14 -15 -16
		// 0  0  0  0  0  0  0  1  1  1   0   0   0   0   0   0
		//+ 16 bit immediate data

		string data = retrieveBitsFromInstruction(-17, 16, commandString);
		data = stringBitsToNumber(data);

		//base of instruction
		outputString.append("LPSTOP #");
		outputString.append(data);

		int baseInstructionByteNumber = 6;

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append(
				"; if supervisor state then immediate data -> SR; interrupt mask -> external bus interface; STOP else TRAP");
		//LPSTOP
	} else if (commandArrayString == "TBLS_TBLSN_table") {
		//TBLS,TBLSN table lookup and interpolate
		//table lookup and interpolate (signed)
		//rounded:
		//	entry(n) +{(entry(N+1) - entry(n)) x D x 7 - 0} / 256 -> Dx
		//unrounded:
		//  entry(n)x256 +{(entry(N+1) - entry(n)) x D x 7 - 0} -> Dx
		//entry(n) and entry(n+1) are either
		//consecutive entries in a table pointed to by <EA> and indexed by Dx 15-8 pi size or
		//the registers Dym, Dyn respectively
		//TBLS <size><EA>,Dx	result rounded
		//TBLSN <size><EA>,Dx	not rounded
		//TBLS <size>Dym:Dyn,Dx	rounded
		//TBLSN <size>Dym:Dyn,Dx no rounded
		//15 14 13 12 11 10 9  8  7 6  5 4 3  2 1 0
		//1  1  1  1   1  0 0  0  0 0 EAmode  EAregi

		//-1  -2 -3 -4 -5 -6 -7 -8  -9 -10  -11 -12 -13 -14 -15 -16
		// 0   regDx    1  R  0  1   size     0   0   0   0   0   0
		//
		//regDx = destination register
		//R = rounding mode R = 0 -> rounding;  R = 1 unrounded
		//
		//byte   word   long
		//00      01     10
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             ---      --
		//    An             ---      --
		//   (An)            ---      --
		//  (An)+            ---      --
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//base of instruction
		outputString.append("TBLS");

		//collect the portions of the command which determine
		//mode and addresses
		string regisDx = retrieveBitsFromInstruction(-2, 3, commandString);
		regisDx = stringBitsToNumber(regisDx);
		string R = retrieveBitsFromInstruction(-6, 1, commandString);
		string size = retrieveBitsFromInstruction(-9, 2, commandString);
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 4;

		if (R == "1") {
			//not rounded
			outputString.append("N");
		}

		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (size == "00") {
			outputString.append(".B");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'B', -16);
		} else if (size == "01") {
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'W', -16);
		} else if (size == "10") {
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'L', -16);
		} else {
			baseInstructionByteNumber = 0;//set this to 0 if no valid entries are made
		}

		outputString.append(" ");
		outputString.append(effectiveAddress);
		outputString.append(",D");
		outputString.append(regisDx);

		//account for special unallowed cases
		if (EAmode == "000" || EAmode == "001" || EAmode == "010"
				|| EAmode == "011" || (EAmode == "111" && EAregi == "100")) {
			baseInstructionByteNumber = 0;
		}

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; table lookup and interpolate (no rounding if TBLSN) signed");
		//TBLS,TBLSN table lookup and interpolate
	} else if (commandArrayString == "TBLS_TBLSN_register") {
		//TBLS,TBLSN data register interpolate
		//table lookup and interpolate (signed)
		//rounded:
		//	entry(n) +{(entry(N+1) - entry(n)) x D x 7 - 0} / 256 -> Dx
		//unrounded:
		//  entry(n)x256 +{(entry(N+1) - entry(n)) x D x 7 - 0} -> Dx
		//entry(n) and entry(n+1) are either
		//consecutive entries in a table pointed to by <EA> and indexed by Dx 15-8 pi size or
		//the registers Dym, Dyn respectively
		//TBLS <size><EA>,Dx	result rounded
		//TBLSN <size><EA>,Dx	not rounded
		//TBLS <size>Dym:Dyn,Dx	rounded
		//TBLSN <size>Dym:Dyn,Dx no rounded
		//15 14 13 12 11 10 9  8  7 6  5 4 3  2 1 0
		//1  1  1  1   1  0 0  0  0 0  0 0 0  RegisDym

		//-1  -2 -3 -4 -5 -6 -7 -8  -9 -10  -11 -12 -13 -14 -15 -16
		// 0   regDx    1  R  0  1   size     0   0   0   regisDyn
		//
		//regDx = destination register
		//R = rounding mode R = 0 -> rounding;  R = 1 unrounded
		//
		//byte   word   long
		//00      01     10

		//base of instruction
		outputString.append("TBLS");

		//collect the portions of the command which determine
		//mode and addresses
		string regisDx = retrieveBitsFromInstruction(-2, 3, commandString);
		regisDx = stringBitsToNumber(regisDx);
		string R = retrieveBitsFromInstruction(-6, 1, commandString);
		string size = retrieveBitsFromInstruction(-9, 2, commandString);
		string regisDyn = retrieveBitsFromInstruction(-14, 3, commandString);
		regisDyn = stringBitsToNumber(regisDyn);
		string regisDym = retrieveBitsFromInstruction(2, 3, commandString);
		regisDym = stringBitsToNumber(regisDym);

		int baseInstructionByteNumber = 4;

		if (R == "1") {
			//not rounded
			outputString.append("N");
		}

		//add the size of the base command to the string
		if (size == "00") {
			outputString.append(".B");
		} else if (size == "01") {
			outputString.append(".W");
		} else if (size == "10") {
			outputString.append(".L");
		} else {
			baseInstructionByteNumber = 0;//set this to 0 if no valid entries are made
		}

		outputString.append(" D");
		outputString.append(regisDym);
		outputString.append(":D");
		outputString.append(regisDyn);
		outputString.append(",D");
		outputString.append(regisDx);

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append(
				"; data register lookup and interpolate (no rounding if TBLSN) signed");
		//TBLS,TBLSN data register lookup and interpolate
	} else if (commandArrayString == "TBLU_TBLUN_table") {
		//TBLU,TBLUN table lookup and interpolation
		//unsigned
		//rounded:
		//	entry(n) +{(entry(N+1) - entry(n)) x D x 7 - 0} / 256 -> Dx
		//unrounded:
		//  entry(n)x256 +{(entry(N+1) - entry(n)) x D x 7 - 0} -> Dx
		//entry(n) and entry(n+1) are either
		//consecutive entries in a table pointed to by <EA> and indexed by Dx 15-8 pi size or
		//the registers Dym, Dyn respectively
		//TBLU <size><EA>,Dx	result rounded
		//TBLUN <size><EA>,Dx	not rounded
		//TBLU <size>Dym:Dyn,Dx	rounded
		//TBLUN <size>Dym:Dyn,Dx no rounded
		//15 14 13 12 11 10 9  8  7 6  5 4 3  2 1 0
		//1  1  1  1   1  0 0  0  0 0 EAmode  EAregi

		//-1  -2 -3 -4 -5 -6 -7 -8  -9 -10  -11 -12 -13 -14 -15 -16
		// 0   regDx    0  R  0  1   size     0   0   0   0   0   0
		//
		//regDx = destination register
		//R = rounding mode R = 0 -> rounding;  R = 1 unrounded
		//
		//byte   word   long
		//00      01     10
		//effective address field: <ea>
		//addressing mode    mode    register
		//    Dn             ---      --
		//    An             ---      --
		//   (An)            010      An
		//  (An)+            ---      --
		//  -(An)            100      An
		// (d16,An)          101      An
		// (d8,An,Xn)        110      An
		// (bd,An,Xn)		 110      An
		//  (xxx).W          111      000
		//  (xxx).L          111      001
		//  #<data>          ---      ---
		//  (d16,PC)         111      010
		//  (d8,PC,Xn)       111      011
		// (bd,PC,Xn)		 111      011

		//base of instruction
		outputString.append("TBLU");

		//collect the portions of the command which determine
		//mode and addresses
		string regisDx = retrieveBitsFromInstruction(-2, 3, commandString);
		regisDx = stringBitsToNumber(regisDx);
		string R = retrieveBitsFromInstruction(-6, 1, commandString);
		string size = retrieveBitsFromInstruction(-9, 2, commandString);
		string EAmode = retrieveBitsFromInstruction(5, 3, commandString);
		string EAregi = retrieveBitsFromInstruction(2, 3, commandString);

		int baseInstructionByteNumber = 4;

		if (R == "1") {
			//not rounded
			outputString.append("N");
		}

		string effectiveAddress = "";
		int extensionBytes = 0;
		//add the size of the base command to the string
		if (size == "00") {
			outputString.append(".B");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'B', -16);
		} else if (size == "01") {
			outputString.append(".W");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'W', -16);
		} else if (size == "10") {
			outputString.append(".L");
			extensionBytes = assembleEffectiveAddress(commandString,
					effectiveAddress, 5, 'L', -16);
		} else {
			baseInstructionByteNumber = 0;//set this to 0 if no valid entries are made
		}

		outputString.append(" ");
		outputString.append(effectiveAddress);
		outputString.append(",D");
		outputString.append(regisDx);

		//account for special unallowed cases
		if (EAmode == "000" || EAmode == "001" || EAmode == "011"
				|| (EAmode == "111" && EAregi == "100")) {
			baseInstructionByteNumber = 0;
		}

		if (baseInstructionByteNumber != 0 && extensionBytes != -1) {
			correctForm = baseInstructionByteNumber + extensionBytes;
		}

		outputString.push_back('\t');
		outputString.append(
				"; table lookup and interpolate (no rounding if TBLSN) unsigned");
		//TBLU,TBLUN table lookup and interpolate
	} else if (commandArrayString == "TBLU_TBLUN_register") {
		//TBLU,TBLUN data register lookup and interpolation (unsigned)
		//rounded:
		//	entry(n) +{(entry(N+1) - entry(n)) x D x 7 - 0} / 256 -> Dx
		//unrounded:
		//  entry(n)x256 +{(entry(N+1) - entry(n)) x D x 7 - 0} -> Dx
		//entry(n) and entry(n+1) are either
		//consecutive entries in a table pointed to by <EA> and indexed by Dx 15-8 pi size or
		//the registers Dym, Dyn respectively
		//TBLU <size><EA>,Dx	result rounded
		//TBLUN <size><EA>,Dx	not rounded
		//TBLU <size>Dym:Dyn,Dx	rounded
		//TBLUN <size>Dym:Dyn,Dx no rounded
		//15 14 13 12 11 10 9  8  7 6  5 4 3  2 1 0
		//1  1  1  1   1  0 0  0  0 0  0 0 0  RegisDym

		//-1  -2 -3 -4 -5 -6 -7 -8  -9 -10  -11 -12 -13 -14 -15 -16
		// 0   regDx    0  R  0  0   size     0   0   0   regisDyn
		//
		//regDx = destination register
		//R = rounding mode R = 0 -> rounding;  R = 1 unrounded
		//
		//byte   word   long
		//00      01     10

		//base of instruction
		outputString.append("TBLU");

		//collect the portions of the command which determine
		//mode and addresses
		string regisDx = retrieveBitsFromInstruction(-2, 3, commandString);
		regisDx = stringBitsToNumber(regisDx);
		string R = retrieveBitsFromInstruction(-6, 1, commandString);
		string size = retrieveBitsFromInstruction(-9, 2, commandString);
		string regisDyn = retrieveBitsFromInstruction(-14, 3, commandString);
		regisDyn = stringBitsToNumber(regisDyn);
		string regisDym = retrieveBitsFromInstruction(2, 3, commandString);
		regisDym = stringBitsToNumber(regisDym);

		int baseInstructionByteNumber = 4;

		if (R == "1") {
			//not rounded
			outputString.append("N");
		}

		//add the size of the base command to the string
		if (size == "00") {
			outputString.append(".B");
		} else if (size == "01") {
			outputString.append(".W");
		} else if (size == "10") {
			outputString.append(".L");
		} else {
			baseInstructionByteNumber = 0;//set this to 0 if no valid entries are made
		}

		outputString.append(" D");
		outputString.append(regisDym);
		outputString.append(":D");
		outputString.append(regisDyn);
		outputString.append(",D");
		outputString.append(regisDx);

		if (baseInstructionByteNumber != 0) {
			correctForm = baseInstructionByteNumber;
		}

		outputString.push_back('\t');
		outputString.append(
				"; data register lookup and interpolate (no rounding if TBLSN) unsigned");
		//TBLU,TBLUN data register lookup and interpolate
	}

	return correctForm;
}

string retrieveBitsFromInstruction(int bitPlace, int numBits,
		string commandString) {
	//return a string of the numBits number of bits at bitPlace (defined
	//in commandString relative to bit assignment
	//('15'------'8''7'------'0')('-1'------'-8''-9'------'-16');
	string bitsToReturn = "";
	for (int i = 0; i < numBits; i++) {
		bitsToReturn.push_back(commandString[(15 - bitPlace) + i]);
	}

	return bitsToReturn;
}

int assembleEffectiveAddress(string commandString, string &effectiveAddress,
		string EAmode, string EAregi, char operationSize,
		int lastBitInCommand) {
	//input the bytes of a commandString and a reference string which will be
	//changed to the appropriate effective address syntax. Also include char B, W,
	// or L to indicate the operation size. Output an int
	//which indicates the number of extension bytes beyond the command
	//or -1 if the format is incorrect
	int extensionBytes = -1;

	//effective address field:
	//addressing mode    mode    register
	//    Dn             000      Dn
	//    An             001      An
	//   (An)            010      An
	//  (An)+            011      An
	//  -(An)            100      An
	// (d16,An)          101      An
	// (d8,An,Xn)        110      An
	// (bd,An,Xn)		 110      An
	//  (xxx).W          111      000
	//  (xxx).L          111      001
	//  #<data>          111      100
	//  (d16,PC)         111      010
	//  (d8,PC,Xn)       111      011
	// (bd,PC,Xn)		 111      011

	//notes about memory addressing:
	//there are 9 ways of effective addressing modes which
	//are used in building machine code commands
	//Addressing mode			Syntax			Mode field		Reg field
	//	Register Direct		- use data in register
	//		Data					Dn				000
	//      Address					An				001
	//	Register Indirect	-use address in register to access data
	//		Address					(An)			010
	//		Address w/postinc.		(An)+			011						-increment by size
	//		Address w/predecrement	-(An)			100						-decrement by size
	//		Address w/displacement	(d16,An)		101						-extend An by 16 bit immed. follow
	//	Address reg indirect w/index - add displacement and value in
	//								register to address register to find
	//								address in which data is present
	//		8-bit displacement		(d8,An,Xn)		110						-followed by 16 bit
	//		base displacement		(bd,An,Xn)		110 					-followed by 16+ bits
	//	Memory Indirect -
	//		postIndexed				([bd,An],Xn,od) 110						-followed by 16+ bits
	//		preIndexed				([bd,An,Xn],od) 110						-followed by 16+ bits
	//	Program Counter Indirect
	//		with displacement		(d16,PC)		111			010			-followed by 16 bits
	//	Program counter indirect with index
	//		8-bit displacement		(d8,PC,Xn)		111			011
	//		base displacement		(bd,PC,Xn)		111			011
	//	Program counter memory indirect
	//		postIndexed				([bd,PC],Xn,od)	111			011			-followed by 16+ bits
	//		preIndexed				([bd,PC,Xn],od) 111			011			-followed by 16+ bits
	//	Absolute Data Addressing
	//		Short					(xxx).W			111			000			-followed by 16 bits
	//		long					(xxx).L			111			000			-followed by 32 bits
	//	Immediate					#<xxx>			111			100			-followed by size# bytes

	//for displacement values - binary command followed by 16 bits which contain
	//half or a quarter of a sign extended displacement
	//if base displacement or an index register are used, the command is followed
	//by more bits which dictate the base displacement

	//8-bit displacement + index:
	// bit place 15,-,-,-,-,-,-,8,7,-,-,-,-,-,-,0
	// bits		 A ,Regis,L,siz,0,--displacement-
	//A = 0 - data register
	//A = 1 - Address register
	//Regis - register number
	//L = 0 - 16 bit register
	//L = 1 - 32 bit register
	//size = index unit of 1, 2, 4 or 8 bytes

	//base displacement + index
	// bit place 15,-,-,-,-,-,-,8,7,-,-,-,-,-,-,0 + more bits of address of size
	// bits		 A ,Regis,L,siz,1,B,X,siz,0,mode-
	//A = 0 - data register
	//A = 1 - Address register
	//Regis - register number
	//L = 0 - 16 bit register
	//L = 1 - 32 bit register
	//size = index unit of 1, 2, 4 or 8 bytes
	//B = 1 - turn off base register
	//X = 1 - turns off index register
	//second size field = form of address bits that follow
	//	01: no address; 10:16-bit address; 11: 32 bit address
	//mode field controls if indirection with pre-indexing or
	//with post-indexing, is used. Indirection with
	//post-indexing allows an indirect reference to an array;
	//indirection with pre-indexing allows choosing from a
	//list of addresses

	//retrieve the effective address mode and number
	//assume that the mode and register bits are
	//adjacent and 3 bits each
	//string EAmode = retrieveBitsFromInstruction(EAstart,3,commandString);
	//string EAregi = retrieveBitsFromInstruction((EAstart-3),3,commandString);

	//clear the effective address
	effectiveAddress = "";
	if (EAmode == "000") {
		//Dn
		//effective address contains data
		effectiveAddress.append("D");
		EAregi = stringBitsToNumber(EAregi);
		effectiveAddress.append(EAregi);
		extensionBytes = 0;
	} else if (EAmode == "001") {
		//An
		//effective address contains data
		effectiveAddress.append("A");
		EAregi = stringBitsToNumber(EAregi);
		effectiveAddress.append(EAregi);
		extensionBytes = 0;
	} else if (EAmode == "010") {
		//(An)
		//effective address points to data
		effectiveAddress.append("(A");
		EAregi = stringBitsToNumber(EAregi);
		effectiveAddress.append(EAregi);
		effectiveAddress.append(")");
		extensionBytes = 0;
	} else if (EAmode == "011") {
		//(An)+
		//effective address points to data, is incremented by size of operand after operation
		effectiveAddress.append("(A");
		EAregi = stringBitsToNumber(EAregi);
		effectiveAddress.append(EAregi);
		effectiveAddress.append(")+");
		extensionBytes = 0;
	} else if (EAmode == "100") {
		//-(An)
		//effective address points to data after decrement of size of operand
		effectiveAddress.append("-(A");
		EAregi = stringBitsToNumber(EAregi);
		effectiveAddress.append(EAregi);
		effectiveAddress.append(")");
		extensionBytes = 0;
	} else if (EAmode == "101") {
		//(d16,An)
		//effective address points to data after adding a sign-extended
		//16-bit displacement
		string displacement = retrieveBitsFromInstruction(
				(lastBitInCommand - 1), 16, commandString);	//gather the 16 bit displacement
		displacement = stringBitsToNumber(displacement);//convert the 16 bit displacement to a decimal
		displacement = stringNumber_to_hex(displacement);//convert the decimal displacement to hex

		effectiveAddress.append("($");		//$ denotes hex value
		effectiveAddress.append(displacement);
		effectiveAddress.append(",A");
		EAregi = stringBitsToNumber(EAregi);
		effectiveAddress.append(EAregi);
		effectiveAddress.append(")");
		extensionBytes = 2;
	} else if (EAmode == "110") {
		//(d8,An,Xn) or (bd,An,Xn)
		//effective address points to data after adding
		//a base displacement or sign extended 8-bit displacement
		//as well as adding a scaled index register

		//first determine if 8-bit or base displacement
		string baseDisplacement = retrieveBitsFromInstruction(
				(lastBitInCommand - 8), 1, commandString);//gather the bit which
		//signals base or 8-bit displacement -> 0 = 8-bit, 1 = base displacement
		string indexRegisterBit = retrieveBitsFromInstruction(
				(lastBitInCommand - 1), 1, commandString);//index register is: 0=data register,1=address register
		string indexRegisterSizeBit = retrieveBitsFromInstruction(
				(lastBitInCommand - 5), 1, commandString);//0=16-bit register,1=32-bit register
		string indexRegisterNumber = retrieveBitsFromInstruction(
				(lastBitInCommand - 2), 3, commandString);//index register number
		indexRegisterNumber = stringBitsToNumber(indexRegisterNumber);
		string indexByteMultiplier = retrieveBitsFromInstruction(
				(lastBitInCommand - 6), 2, commandString);//1,2,4 or 8 byte = index unit

		//effective address register number: convert from binary to decimal
		EAregi = stringBitsToNumber(EAregi);

		//assemble the effective address string
		if (baseDisplacement == "0") {
			effectiveAddress.append("($");
			string displacement = retrieveBitsFromInstruction(
					(lastBitInCommand - 9), 8, commandString);//gather 8-bit displacement
			displacement = stringBitsToNumber(displacement);//convert binary to decimal
			displacement = stringNumber_to_hex(displacement);//convert the decimal displacement to hex
			effectiveAddress.append(displacement);
			effectiveAddress.append(",A");
			effectiveAddress.append(EAregi);
			effectiveAddress.append(",");

			if (indexRegisterBit == "0") {
				effectiveAddress.append("D");		//index is data register
			} else if (indexRegisterBit == "1") {
				effectiveAddress.append("A");		//index is address register
			}
			effectiveAddress.append(indexRegisterNumber);
			effectiveAddress.append(".");
			if (indexRegisterSizeBit == "0") {
				effectiveAddress.append("W*");		//16 bit register
			} else {
				effectiveAddress.append("L*");		//32 bit register
			}
			if (indexByteMultiplier == "00") {
				effectiveAddress.append("1");//number of bytes the index scales by
			} else if (indexByteMultiplier == "01") {
				effectiveAddress.append("2");
			} else if (indexByteMultiplier == "10") {
				effectiveAddress.append("4");
			} else if (indexByteMultiplier == "11") {
				effectiveAddress.append("8");
			}
			effectiveAddress.append(")");
			extensionBytes = 2;	//2 bytes for the 8-bit displacement and index info fields
		} else if (baseDisplacement == "1")		//base displacement mode
				{
			effectiveAddress.append("(");
			string notB = retrieveBitsFromInstruction((lastBitInCommand - 9), 1,
					commandString);		//turns off base (An) register
			string notX = retrieveBitsFromInstruction((lastBitInCommand - 10),
					1, commandString);		//turns off index register
			string addressSize = retrieveBitsFromInstruction(
					(lastBitInCommand - 11), 2, commandString);	//size of base displacement address
			//01 = no address; 10 = 16-bit; 11 = 32 bit
			string baseDisplacementAddress = "";
			if (addressSize == "10") {
				baseDisplacementAddress = retrieveBitsFromInstruction(
						(lastBitInCommand - 17), 16, commandString);//size of base displacement is 16 bits
				baseDisplacementAddress = stringBitsToNumber(
						baseDisplacementAddress);	//convert binary to decimal
				baseDisplacementAddress = stringNumber_to_hex(
						baseDisplacementAddress);		//convert decimal to hex
				extensionBytes = 4;			//2 following words
			} else if (addressSize == "11") {
				baseDisplacementAddress = retrieveBitsFromInstruction(
						(lastBitInCommand - 17), 32, commandString);//size of base displacement is 32 bits
				baseDisplacementAddress = stringBitsToNumber(
						baseDisplacementAddress);	//convert binary to decimal
				baseDisplacementAddress = stringNumber_to_hex(
						baseDisplacementAddress);		//convert decimal to hex
				extensionBytes = 6;			//1 word and one long extension
			} else if (addressSize == "01") {
				extensionBytes = 2;
			}

			if (addressSize != "01") {
				effectiveAddress.append("$");//denote hex address of base displacement
				effectiveAddress.append(baseDisplacementAddress);
				if (notB == "0" || notX == "0") {
					effectiveAddress.append(",");
				}
			}

			if (notB == "0") {
				effectiveAddress.append("A");
				effectiveAddress.append(EAregi);
				if (notX == "0") {
					effectiveAddress.append(",");
				}
			}

			if (notX == "0") {
				if (indexRegisterBit == "0") {
					effectiveAddress.append("D");		//index is data register
				} else if (indexRegisterBit == "1") {
					effectiveAddress.append("A");	//index is address register
				}
				effectiveAddress.append(indexRegisterNumber);
				effectiveAddress.append(".");
				if (indexRegisterSizeBit == "0") {
					effectiveAddress.append("W*");			//16 bit register
				} else {
					effectiveAddress.append("L*");			//32 bit register
				}
				if (indexByteMultiplier == "00") {
					effectiveAddress.append("1");//number of bytes the index scales by
				} else if (indexByteMultiplier == "01") {
					effectiveAddress.append("2");
				} else if (indexByteMultiplier == "10") {
					effectiveAddress.append("4");
				} else if (indexByteMultiplier == "11") {
					effectiveAddress.append("8");
				}
			}
			effectiveAddress.append(")");
		}
	} else if (EAmode == "111") {
		if (EAregi == "000") {
			//(xxx).W
			string displacement = retrieveBitsFromInstruction(
					(lastBitInCommand - 1), 16, commandString);	//gather the 16 bit extension
			displacement = stringBitsToNumber(displacement);//convert the 16 bit extension to a decimal
			displacement = stringNumber_to_hex(displacement);//convert the decimal extension to hex
			effectiveAddress.append("($");
			effectiveAddress.append(displacement);
			effectiveAddress.append(").W");
			extensionBytes = 2;			//followed by this word
		} else if (EAregi == "001") {
			//(xxx).L
			string displacement = retrieveBitsFromInstruction(
					(lastBitInCommand - 1), 32, commandString);	//gather the 32 bit extension
			displacement = stringBitsToNumber(displacement);//convert the 32 bit extension to a decimal
			displacement = stringNumber_to_hex(displacement);//convert the decimal extension to hex
			effectiveAddress.append("($");
			effectiveAddress.append(displacement);
			effectiveAddress.append(").L");
			extensionBytes = 4;			//followed by this long
		} else if (EAregi == "100") {
			//#<data> length determined by size of operation
			string displacement = "";
			if (operationSize == 'B' || operationSize == 'b'
					|| operationSize == 'W' || operationSize == 'w') {
				displacement = retrieveBitsFromInstruction(
						(lastBitInCommand - 1), 16, commandString);	//gather the 16 bit extension
				displacement = stringBitsToNumber(displacement);//convert the 16 bit extension to a decimal
				extensionBytes = 2;
			} else if (operationSize == 'L' || operationSize == 'l') {
				displacement = retrieveBitsFromInstruction(
						(lastBitInCommand - 1), 32, commandString);	//gather the 32 bit extension
				displacement = stringBitsToNumber(displacement);//convert the 32 bit extension to a decimal
				extensionBytes = 4;
			}
			effectiveAddress.append("#");
			effectiveAddress.append(displacement);
		} else if (EAregi == "010") {
			//(d16,PC)
			string displacement = retrieveBitsFromInstruction(
					(lastBitInCommand - 1), 16, commandString);	//gather the 16 bit displacement
			displacement = stringBitsToNumber(displacement);//convert the 16 bit displacement to a decimal
			displacement = stringNumber_to_hex(displacement);//convert the decimal displacement to hex
			effectiveAddress.append("($");
			effectiveAddress.append(displacement);
			effectiveAddress.append(",PC)");
			extensionBytes = 2;
		} else if (EAregi == "011") {
			//(d8,PC,Xn) or (bd,PC,Xn)
			//effective address points to data after adding
			//a base displacement or sign extended 8-bit displacement
			//as well as adding a scaled index register

			//first determine if 8-bit or base displacement
			string baseDisplacement = retrieveBitsFromInstruction(
					(lastBitInCommand - 8), 1, commandString);//gather the bit which
			//signals base or 8-bit displacement -> 0 = 8-bit, 1 = base displacement
			string indexRegisterBit = retrieveBitsFromInstruction(
					(lastBitInCommand - 1), 1, commandString);//index register is: 0=data register,1=address register
			string indexRegisterSizeBit = retrieveBitsFromInstruction(
					(lastBitInCommand - 5), 1, commandString);//0=16-bit register,1=32-bit register
			string indexRegisterNumber = retrieveBitsFromInstruction(
					(lastBitInCommand - 2), 3, commandString);//index register number
			indexRegisterNumber = stringBitsToNumber(indexRegisterNumber);
			string indexByteMultiplier = retrieveBitsFromInstruction(
					(lastBitInCommand - 6), 2, commandString);//1,2,4 or 8 byte = index unit

			//assemble the effective address string
			if (baseDisplacement == "0") {
				effectiveAddress.append("($");
				string displacement = retrieveBitsFromInstruction(
						(lastBitInCommand - 9), 8, commandString);//gather 8-bit displacement
				displacement = stringBitsToNumber(displacement);//convert binary to decimal
				displacement = stringNumber_to_hex(displacement);//convert the decimal displacement to hex
				effectiveAddress.append(displacement);
				effectiveAddress.append(",PC,");

				if (indexRegisterBit == "0") {
					effectiveAddress.append("D");		//index is data register
				} else if (indexRegisterBit == "1") {
					effectiveAddress.append("A");	//index is address register
				}
				effectiveAddress.append(indexRegisterNumber);
				effectiveAddress.append(".");
				if (indexRegisterSizeBit == "0") {
					effectiveAddress.append("W*");			//16 bit register
				} else {
					effectiveAddress.append("L*");			//32 bit register
				}
				if (indexByteMultiplier == "00") {
					effectiveAddress.append("1");//number of bytes the index scales by
				} else if (indexByteMultiplier == "01") {
					effectiveAddress.append("2");
				} else if (indexByteMultiplier == "10") {
					effectiveAddress.append("4");
				} else if (indexByteMultiplier == "11") {
					effectiveAddress.append("8");
				}
				effectiveAddress.append(")");
				extensionBytes = 2;	//2 bytes for the 8-bit displacement and index info fields
			} else if (baseDisplacement == "1")			//base displacement mode
					{
				effectiveAddress.append("(");
				string notB = retrieveBitsFromInstruction(
						(lastBitInCommand - 9), 1, commandString);//turns off base (An) register
				string notX = retrieveBitsFromInstruction(
						(lastBitInCommand - 10), 1, commandString);	//turns off index register
				string addressSize = retrieveBitsFromInstruction(
						(lastBitInCommand - 11), 2, commandString);	//size of base displacement address
				//01 = no address; 10 = 16-bit; 11 = 32 bit
				string baseDisplacementAddress = "";
				if (addressSize == "10") {
					baseDisplacementAddress = retrieveBitsFromInstruction(
							(lastBitInCommand - 17), 16, commandString);//size of base displacement is 16 bits
					baseDisplacementAddress = stringBitsToNumber(
							baseDisplacementAddress);//convert binary to decimal
					baseDisplacementAddress = stringNumber_to_hex(
							baseDisplacementAddress);	//convert decimal to hex
					extensionBytes = 4;				//2 following words
				} else if (addressSize == "11") {
					baseDisplacementAddress = retrieveBitsFromInstruction(
							(lastBitInCommand - 17), 32, commandString);//size of base displacement is 32 bits
					baseDisplacementAddress = stringBitsToNumber(
							baseDisplacementAddress);//convert binary to decimal
					baseDisplacementAddress = stringNumber_to_hex(
							baseDisplacementAddress);	//convert decimal to hex
					extensionBytes = 6;			//1 word and one long extension
				} else if (addressSize == "01") {
					extensionBytes = 2;				//one word follows
				}

				if (addressSize != "01") {
					effectiveAddress.append("$");//denote hex address of base displacement
					effectiveAddress.append(baseDisplacementAddress);
					if (notB == "0" || notX == "0") {
						effectiveAddress.append(",");
					}
				}

				if (notB == "0") {
					effectiveAddress.append("PC");
					if (notX == "0") {
						effectiveAddress.append(",");
					}
				}

				if (notX == "0") {
					if (indexRegisterBit == "0") {
						effectiveAddress.append("D");	//index is data register
					} else if (indexRegisterBit == "1") {
						effectiveAddress.append("A");//index is address register
					}
					effectiveAddress.append(indexRegisterNumber);
					effectiveAddress.append(".");
					if (indexRegisterSizeBit == "0") {
						effectiveAddress.append("W*");		//16 bit register
					} else {
						effectiveAddress.append("L*");		//32 bit register
					}
					if (indexByteMultiplier == "00") {
						effectiveAddress.append("1");//number of bytes the index scales by
					} else if (indexByteMultiplier == "01") {
						effectiveAddress.append("2");
					} else if (indexByteMultiplier == "10") {
						effectiveAddress.append("4");
					} else if (indexByteMultiplier == "11") {
						effectiveAddress.append("8");
					}
				}
				effectiveAddress.append(")");
			}
		}

	}

	return extensionBytes;
}

int assembleEffectiveAddress(string commandString, string &effectiveAddress,
		char operationSize) {
	//same as above, but with EAstart == bit 5 (last 6 bits = EA)
	int value = 0;
	value = assembleEffectiveAddress(commandString, effectiveAddress, 5,
			operationSize, 0);
	return value;
}

int assembleEffectiveAddress(string commandString, string &effectiveAddress,
		int EAstart, char operationSize, int lastBitInCommand) {
	//assume 6 contiguous bits define the effective address with the first 3 = mode and the last 3 = register
	string EAmode = retrieveBitsFromInstruction(EAstart, 3, commandString);
	string EAregi = retrieveBitsFromInstruction((EAstart - 3), 3,
			commandString);
	int value = 0;
	value = assembleEffectiveAddress(commandString, effectiveAddress, EAmode,
			EAregi, operationSize, lastBitInCommand);
	return value;
}

int guessCommand(string commandString, string &outputString, bool &multipleMatch) {
	//input a set of bits, output a string formatted as an assembly instruction
	//if multiple matches are found, multipleMatch is changed to true
	//otherwise it is set false

	int commandLength = 0;//length of command which is output
	multipleMatch = false;//this flag is available in case we make multiple matches

	//these arrays contain the CPU32 instructions and their variations in binary
	char ABCD[16] = 				{ '1', '1', '0', '0', 'x', 'x', 'x', '1', '0', '0', '0', '0', 'x', 'x', 'x', 'x' };//0
	char ADD[16] =  				{ '1', '1', '0', '1', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//1
	char ADDA[16] = 				{ '1', '1', '0', '1', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//2
	char ADDI[16] = 				{ '0', '0', '0', '0', '0', '1', '1', '0', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//3
	char ADDQ[16] = 				{ '0', '1', '0', '1', 'x', 'x', 'x', '0', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//4
	char ADDX[16] = 				{ '1', '1', '0', '1', 'x', 'x', 'x', '1', 'x', 'x', '0', '0', 'x', 'x', 'x', 'x' };//5
	char AND[16] =  			  	{ '1', '1', '0', '0', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//6
	char ANDI[16] = 			  	{ '0', '0', '0', '0', '0', '0', '1', '0', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//7
	char ANDItoCCR[16] = 		  	{ '0', '0', '0', '0', '0', '0', '1', '0', '0', '0', '1', '1', '1', '1', '0', '0' };//8
	char ASL_ASR_sizeSelect[16] = 	{ '1', '1', '1', '0', 'x', 'x', 'x', 'x', 'x', 'x', 'x', '0', '0', 'x', 'x', 'x' };//9
	char ASL_ASR[16] = 				{ '1', '1', '1', '0', '0', '0', '0', 'x', '1', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//10
	char Bcc[16] = 					{ '0', '1', '1', '0', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//11
	char BCHG_bit[16] = 			{ '0', '0', '0', '0', 'x', 'x', 'x', '1', '0', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//12
	char BCHG[16] = 				{ '0', '0', '0', '0', '1', '0', '0', '0', '0', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//13
	char BCLR_bit[16] = 			{ '0', '0', '0', '0', 'x', 'x', 'x', '1', '1', '0', 'x', 'x', 'x', 'x', 'x', 'x' };//14
	char BCLR[16] = 				{ '0', '0', '0', '0', '1', '0', '0', '0', '1', '0', 'x', 'x', 'x', 'x', 'x', 'x' };//15
	char BKPT[16] =     			{ '0', '1', '0', '0', '1', '0', '0', '0', '0', '1', '0', '0', '1', 'x', 'x', 'x' };//16
	char BRA[16] =    				{ '0', '1', '1', '0', '0', '0', '0', '0', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//17
	char BSET_bit[16] = 			{ '0', '0', '0', '0', 'x', 'x', 'x', '1', '1', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//18
	char BSET[16] = 				{ '0', '0', '0', '0', '1', '0', '0', '0', '1', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//19
	char BSR[16] = 					{ '0', '1', '1', '0', '0', '0', '0', '1', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//20
	char BTST_bit[16] =   			{ '0', '0', '0', '0', 'x', 'x', 'x', '1', '0', '0', 'x', 'x', 'x', 'x', 'x', 'x' };//21
	char BTST[16] = 				{ '0', '0', '0', '0', '1', '0', '0', '0', '0', '0', 'x', 'x', 'x', 'x', 'x', 'x' };//22
	char CHK[16] = 					{ '0', '1', '0', '0', 'x', 'x', 'x', 'x', 'x', '0', 'x', 'x', 'x', 'x', 'x', 'x' };//23
	char CHK2[16] = 				{ '0', '0', '0', '0', '0', 'x', 'x', '0', '1', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//24
	char CLR[16] = 					{ '0', '1', '0', '0', '0', '0', '1', '0', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//25
	char CMP[16] = 					{ '1', '0', '1', '1', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//26
	char CMPA[16] = 				{ '1', '0', '1', '1', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//27
	char CMPI[16] = 				{ '0', '0', '0', '0', '1', '1', '0', '0', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//28
	char CMPM[16] = 				{ '1', '0', '1', '1', 'x', 'x', 'x', '1', 'x', 'x', '0', '0', '1', 'x', 'x', 'x' };//29
	char CMP2[16] = 				{ '0', '0', '0', '0', '0', 'x', 'x', '0', '1', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//30
	char DBcc[16] = 				{ '0', '1', '0', '1', 'x', 'x', 'x', 'x', '1', '1', '0', '0', '1', 'x', 'x', 'x' };//31
	char DIVS_DIVSL[16] = 			{ '1', '0', '0', '0', 'x', 'x', 'x', '1', '1', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//32
	char DIVS_DIVSL_follow[16] = 	{ '0', '1', '0', '0', '1', '1', '0', '0', '0', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//33
	char DIVU_DIVUL[16] = 			{ '1', '0', '0', '0', 'x', 'x', 'x', '0', '1', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//34
	char DIVU_DIVUL_follow[16] = 	{ '0', '1', '0', '0', '1', '1', '0', '0', '0', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//35
	char EOR[16] = 					{ '1', '0', '1', '1', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//36
	char EORI[16] = 				{ '0', '0', '0', '0', '1', '0', '1', '0', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//37
	char EORItoCCR[16] = 			{ '0', '0', '0', '0', '1', '0', '1', '0', '0', '0', '1', '1', '1', '1', '0', '0' };//38
	char EXG[16] = 					{ '1', '1', '0', '0', 'x', 'x', 'x', '1', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//39
	char EXT_EXTB[16] = 			{ '0', '1', '0', '0', '1', '0', '0', 'x', 'x', 'x', '0', '0', '0', 'x', 'x', 'x' };//40
	char ILLEGAL[16] = 				{ '0', '1', '0', '0', '1', '0', '1', '0', '1', '1', '1', '1', '1', '1', '0', '0' };//41
	char JMP[16] = 					{ '0', '1', '0', '0', '1', '1', '1', '0', '1', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//42
	char JSR[16] = 					{ '0', '1', '0', '0', '1', '1', '1', '0', '1', '0', 'x', 'x', 'x', 'x', 'x', 'x' };//43
	char LEA[16] = 					{ '0', '1', '0', '0', 'x', 'x', 'x', '1', '1', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//44
	char LINK_word[16] = 			{ '0', '1', '0', '0', '1', '1', '1', '0', '0', '1', '0', '1', '0', 'x', 'x', 'x' };//45
	char LINK_long[16] = 			{ '0', '1', '0', '0', '1', '0', '0', '0', '0', '0', '0', '0', '1', 'x', 'x', 'x' };//46
	char LSL_LSR_sizeSelect[16] = 	{ '1', '1', '1', '0', 'x', 'x', 'x', 'x', 'x', 'x', 'x', '0', '1', 'x', 'x', 'x' };//47
	char LSL_LSR[16] = 				{ '1', '1', '1', '0', '0', '0', '1', 'x', '1', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//48
	char MOVE[16] = 				{ '0', '0', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//49
	char MOVEA[16] = 				{ '0', '0', 'x', 'x', 'x', 'x', 'x', '0', '0', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//50
	char MOVEfromCCR[16] = 			{ '0', '1', '0', '0', '0', '0', '1', '0', '1', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//51
	char MOVEtoCCR[16] = 			{ '0', '1', '0', '0', '0', '1', '0', '0', '1', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//52
	char MOVEfromSR[16] = 			{ '0', '1', '0', '0', '0', '0', '0', '0', '1', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//53
	char MOVEM[16] = 				{ '0', '1', '0', '0', '1', 'x', '0', '0', '1', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//54
	char MOVEP[16] = 				{ '0', '0', '0', '0', 'x', 'x', 'x', 'x', 'x', 'x', '0', '0', '1', 'x', 'x', 'x' };//55
	char MOVEQ[16] = 				{ '0', '1', '1', '1', 'x', 'x', 'x', '0', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//56
	char MULS[16] = 				{ '1', '1', '0', '0', 'x', 'x', 'x', '1', '1', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//57
	char MULS_follow[16] = 			{ '0', '1', '0', '0', '1', '1', '0', '0', '0', '0', 'x', 'x', 'x', 'x', 'x', 'x' };//58
	char MULU[16] = 				{ '1', '1', '0', '0', 'x', 'x', 'x', '0', '1', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//59
	char MULU_follow[16] = 			{ '0', '1', '0', '0', '1', '1', '0', '0', '0', '0', 'x', 'x', 'x', 'x', 'x', 'x' };//60
	char NBCD[16] = 				{ '0', '1', '0', '0', '1', '0', '0', '0', '0', '0', 'x', 'x', 'x', 'x', 'x', 'x' };//61
	char NEG[16] = 					{ '0', '1', '0', '0', '0', '1', '0', '0', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//62
	char NEGX[16] = 				{ '0', '1', '0', '0', '0', '0', '0', '0', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//63
	char NOP[16] = 					{ '0', '1', '0', '0', '1', '1', '1', '0', '0', '1', '1', '1', '0', '0', '0', '1' };//64
	char NOT[16] = 					{ '0', '1', '0', '0', '0', '1', '1', '0', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//65
	char OR[16] = 					{ '1', '0', '0', '0', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//66
	char ORI[16] = 					{ '0', '0', '0', '0', '0', '0', '0', '0', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//67
	char ORItoCCR[16] = 			{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1', '1', '1', '0', '0' };//68
	char PEA[16] = 					{ '0', '1', '0', '0', '1', '0', '0', '0', '0', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//69
	char ROL_ROR_register[16] = 	{ '1', '1', '1', '0', 'x', 'x', 'x', 'x', 'x', 'x', 'x', '1', '1', 'x', 'x', 'x' };//70
	char ROL_ROR_memory[16] = 		{ '1', '1', '1', '0', '0', '1', '1', 'x', '1', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//71
	char ROXL_ROXR_register[16] = 	{ '1', '1', '1', '0', 'x', 'x', 'x', 'x', 'x', 'x', 'x', '1', '0', 'x', 'x', 'x' };//72
	char ROXL_ROXR_memory[16] = 	{ '1', '1', '1', '0', '0', '1', '0', 'x', '1', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//73
	char RTD[16] = 					{ '0', '1', '0', '0', '1', '1', '1', '0', '0', '1', '1', '1', '0', '1', '0', '0' };//74
	char RTR[16] = 					{ '0', '1', '0', '0', '1', '1', '1', '0', '0', '1', '1', '1', '0', '1', '1', '1' };//75
	char RTS[16] = 					{ '0', '1', '0', '0', '1', '1', '1', '0', '0', '1', '1', '1', '0', '1', '0', '1' };//76
	char SBCD[16] = 				{ '1', '0', '0', '0', 'x', 'x', 'x', '1', '0', '0', '0', '0', 'x', 'x', 'x', 'x' };//77
	char Scc[16] = 					{ '0', '1', '0', '1', 'x', 'x', 'x', 'x', '1', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//78
	char SUB[16] = 					{ '1', '0', '0', '1', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//79
	char SUBA[16] = 				{ '1', '0', '0', '1', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//80
	char SUBI[16] = 				{ '0', '0', '0', '0', '0', '1', '0', '0', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//81
	char SUBQ[16] = 				{ '0', '1', '0', '1', 'x', 'x', 'x', '1', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//82
	char SUBX[16] = 				{ '1', '0', '0', '1', 'x', 'x', 'x', '1', 'x', 'x', '0', '0', 'x', 'x', 'x', 'x' };//83
	char SWAP[16] = 				{ '0', '1', '0', '0', '1', '0', '0', '0', '0', '1', '0', '0', '0', 'x', 'x', 'x' };//84
	char TAS[16] = 					{ '0', '1', '0', '0', '1', '0', '1', '0', '1', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//85
	char TRAP[16] = 				{ '0', '1', '0', '0', '1', '1', '1', '0', '0', '1', '0', '0', 'x', 'x', 'x', 'x' };//86
	char TRAPcc[16] = 				{ '0', '1', '0', '1', 'x', 'x', 'x', 'x', '1', '1', '1', '1', '1', 'x', 'x', 'x' };//87
	char TRAPV[16] = 				{ '0', '1', '0', '0', '1', '1', '1', '0', '0', '1', '1', '1', '0', '1', '1', '0' };//88
	char TST[16] = 					{ '0', '1', '0', '0', '1', '0', '1', '0', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//89
	char UNLK[16] = 				{ '0', '1', '0', '0', '1', '1', '1', '0', '0', '1', '0', '1', '1', 'x', 'x', 'x' };//90
	char ANDItoSR[16] = 			{ '0', '0', '0', '0', '0', '0', '1', '0', '0', '1', '1', '1', '1', '1', '0', '0' };//91
	char EORItoSR[16] = 			{ '0', '0', '0', '0', '1', '0', '1', '0', '0', '1', '1', '1', '1', '1', '0', '0' };//92
	char MOVEtoSR[16] = 			{ '0', '1', '0', '0', '0', '1', '1', '0', '1', '1', 'x', 'x', 'x', 'x', 'x', 'x' };//93
	char MOVE_USP[16] = 			{ '0', '1', '0', '0', '1', '1', '1', '0', '0', '1', '1', '0', 'x', 'x', 'x', 'x' };//94
	char MOVEC[16] = 				{ '0', '1', '0', '0', '1', '1', '1', '0', '0', '1', '1', '1', '1', '0', '1', 'x' };//95
	char MOVES[16] = 				{ '0', '0', '0', '0', '1', '1', '1', '0', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x' };//96
	char ORItoSR[16] = 				{ '0', '0', '0', '0', '0', '0', '0', '0', '0', '1', '1', '1', '1', '1', '0', '0' };//97
	char RESET[16] = 				{ '0', '1', '0', '0', '1', '1', '1', '0', '0', '1', '1', '1', '0', '0', '0', '0' };//98
	char RTE[16] = 					{ '0', '1', '0', '0', '1', '1', '1', '0', '0', '1', '1', '1', '0', '0', '1', '1' };//99
	char STOP[16] = 				{ '0', '1', '0', '0', '1', '1', '1', '0', '0', '1', '1', '1', '0', '0', '1', '0' };//100
	char BGND[16] = 				{ '0', '1', '0', '0', '1', '0', '1', '0', '1', '1', '1', '1', '1', '0', '1', '0' };//101
	char LPSTOP[16] = 				{ '1', '1', '1', '1', '1', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' };//102
	char TBLS_TBLSN_table[16] = 	{ '1', '1', '1', '1', '1', '0', '0', '0', '0', '0', 'x', 'x', 'x', 'x', 'x', 'x' };//103
	char TBLS_TBLSN_register[16] = 	{ '1', '1', '1', '1', '1', '0', '0', '0', '0', '0', '0', '0', '0', 'x', 'x', 'x' };//104
	char TBLU_TBLUN_table[16] = 	{ '1', '1', '1', '1', '1', '0', '0', '0', '0', '0', 'x', 'x', 'x', 'x', 'x', 'x' };//105
	char TBLU_TBLUN_register[16] = 	{ '1', '1', '1', '1', '1', '0', '0', '0', '0', '0', '0', '0', '0', 'x', 'x', 'x' };//106

	//comparing these instructions to the memory, pick the most likely instruction
	//from the lookup array of the combined instructions
	//						1                                                      10                                                                    20                                                30                                                                                         40                                                                               50                                                                             60                                                    70                                                                                           80                                                 90                                                                     100                                         105
	char *instrCPU32[107] = { ABCD, ADD, ADDA, ADDI, ADDQ, ADDX, AND, ANDI,
			ANDItoCCR, ASL_ASR_sizeSelect, ASL_ASR, Bcc, BCHG_bit, BCHG,
			BCLR_bit, BCLR, BKPT, BRA, BSET_bit, BSET, BSR, BTST_bit, BTST, CHK,
			CHK2, CLR, CMP, CMPA, CMPI, CMPM, CMP2, DBcc, DIVS_DIVSL,
			DIVS_DIVSL_follow, DIVU_DIVUL, DIVU_DIVUL_follow, EOR, EORI,
			EORItoCCR, EXG, EXT_EXTB, ILLEGAL, JMP, JSR, LEA, LINK_word,
			LINK_long, LSL_LSR_sizeSelect, LSL_LSR, MOVE, MOVEA, MOVEfromCCR,
			MOVEtoCCR, MOVEfromSR, MOVEM, MOVEP, MOVEQ, MULS, MULS_follow, MULU,
			MULU_follow, NBCD, NEG, NEGX, NOP, NOT, OR, ORI, ORItoCCR, PEA,
			ROL_ROR_register, ROL_ROR_memory, ROXL_ROXR_register,
			ROXL_ROXR_memory, RTD, RTR, RTS, SBCD, Scc, SUB, SUBA, SUBI, SUBQ,
			SUBX, SWAP, TAS, TRAP, TRAPcc, TRAPV, TST, UNLK, ANDItoSR, EORItoSR,
			MOVEtoSR, MOVE_USP, MOVEC, MOVES, ORItoSR, RESET, RTE, STOP, BGND,
			LPSTOP, TBLS_TBLSN_table, TBLS_TBLSN_register, TBLU_TBLUN_table,
			TBLU_TBLUN_register };
	string stringinstrCPU32[107] = { "ABCD", "ADD", "ADDA", "ADDI", "ADDQ",
			"ADDX", "AND", "ANDI", "ANDItoCCR", "ASL_ASR_sizeSelect", "ASL_ASR",
			"Bcc", "BCHG_bit", "BCHG", "BCLR_bit", "BCLR", "BKPT", "BRA",
			"BSET_bit", "BSET", "BSR", "BTST_bit", "BTST", "CHK", "CHK2", "CLR",
			"CMP", "CMPA", "CMPI", "CMPM", "CMP2", "DBcc", "DIVS_DIVSL",
			"DIVS_DIVSL_follow", "DIVU_DIVUL", "DIVU_DIVUL_follow", "EOR",
			"EORI", "EORItoCCR", "EXG", "EXT_EXTB", "ILLEGAL", "JMP", "JSR",
			"LEA", "LINK_word", "LINK_long", "LSL_LSR_sizeSelect", "LSL_LSR",
			"MOVE", "MOVEA", "MOVEfromCCR", "MOVEtoCCR", "MOVEfromSR", "MOVEM",
			"MOVEP", "MOVEQ", "MULS", "MULS_follow", "MULU", "MULU_follow",
			"NBCD", "NEG", "NEGX", "NOP", "NOT", "OR", "ORI", "ORItoCCR", "PEA",
			"ROL_ROR_register", "ROL_ROR_memory", "ROXL_ROXR_register",
			"ROXL_ROXR_memory", "RTD", "RTR", "RTS", "SBCD", "Scc", "SUB",
			"SUBA", "SUBI", "SUBQ", "SUBX", "SWAP", "TAS", "TRAP", "TRAPcc",
			"TRAPV", "TST", "UNLK", "ANDItoSR", "EORItoSR", "MOVEtoSR",
			"MOVE_USP", "MOVEC", "MOVES", "ORItoSR", "RESET", "RTE", "STOP",
			"BGND", "LPSTOP", "TBLS_TBLSN_table", "TBLS_TBLSN_register",
			"TBLU_TBLUN_table", "TBLU_TBLUN_register" };

	//in order to aid the choice, a second set of binary instructions may accompany some commands
	char eight_bit_byte[16] = { '0', '0', '0', '0', '0', '0', '0', '0', 'x',
			'x', 'x', 'x', 'x', 'x', 'x', 'x' };
	char chk2_extra[16] = { 'x', 'x', 'x', 'x', '1', '0', '0', '0', '0', '0',
			'0', '0', '0', '0', '0', '0' };
	char cmp2_extra[16] = { 'x', 'x', 'x', 'x', '0', '0', '0', '0', '0', '0',
			'0', '0', '0', '0', '0', '0' };
	char divs_divsl_follow_extra[16] = { '0', 'x', 'x', 'x', '1', 'x', '0', '0',
			'0', '0', '0', '0', '0', 'x', 'x', 'x' };
	char divu_divul_follow_extra[16] = { '0', 'x', 'x', 'x', '0', 'x', '0', '0',
			'0', '0', '0', '0', '0', 'x', 'x', 'x' };
	char muls_follow_extra[16] = { '0', 'x', 'x', 'x', '1', 'x', '0', '0', '0',
			'0', '0', '0', '0', 'x', 'x', 'x' };
	char mulu_follow_extra[16] = { '0', 'x', 'x', 'x', '0', 'x', '0', '0', '0',
			'0', '0', '0', '0', 'x', 'x', 'x' };
	char moves_extra[16] = { 'x', 'x', 'x', 'x', 'x', '0', '0', '0', '0', '0',
			'0', '0', '0', '0', '0', '0' };
	char lpstop_extra[16] = { '0', '0', '0', '0', '0', '0', '0', '1', '1', '1',
			'0', '0', '0', '0', '0', '0' };
	//lpstop has an extra 16 bits beyond this too
	char tbls_tblsn_table_extra[16] = { '0', 'x', 'x', 'x', '1', 'x', '0', '1',
			'x', 'x', '0', '0', '0', '0', '0', '0' };
	char tbls_tblsn_register_extra[16] = { '0', 'x', 'x', 'x', '1', 'x', '0',
			'1', 'x', 'x', '0', '0', '0', 'x', 'x', 'x' };
	char tblu_tblun_table_extra[16] = { '0', 'x', 'x', 'x', '0', 'x', '0', '1',
			'x', 'x', '0', '0', '0', '0', '0', '0' };
	char tblu_tblun_register_extra[16] = { '0', 'x', 'x', 'x', '0', 'x', '0',
			'0', 'x', 'x', '0', '0', '0', 'x', 'x', 'x' };

	//combine the extra bits into a lookup array
	char *definedExtra[13] = { eight_bit_byte, chk2_extra, cmp2_extra,
			divs_divsl_follow_extra, divu_divul_follow_extra, muls_follow_extra,
			mulu_follow_extra, moves_extra, lpstop_extra,
			tbls_tblsn_table_extra, tbls_tblsn_register_extra,
			tblu_tblun_table_extra, tblu_tblun_register_extra };

	//create an array of flags which indicate the type of extra bytes that accompany an instruction, corresponding to that instruction's array index
	//-1 = no extra bits
	//0-12 = lookup array location in definedExtra
	//13 = defined in instruction bits
	//14 = followed by 16-bit word
	//15 = followed by 32-bit long
	//                                 1                         10                         20                          30                        40                            50                           60                          70                            80                            90                          100
	int instruction_extraFlags[107] = { -1, -1, -1, 13, -1, -1, -1, 13, 0, -1,
			-1, 13, -1, 0, -1, 0, -1, 13, -1, 0, 13, -1, 0, -1, 1, -1, -1, -1,
			13, -1, 2, 14, -1, 3, -1, 4, -1, 13, 0, -1, -1, -1, -1, -1, -1, 14,
			15, -1, -1, -1, -1, -1, -1, -1, 14, 14, -1, -1, 5, -1, 6, -1, -1,
			-1, -1, -1, -1, 13, 0, -1, -1, -1, -1, -1, 14, -1, -1, -1, -1, -1,
			-1, 13, -1, -1, -1, -1, -1, 13, -1, -1, -1, 14, 14, -1, -1, 14, 7,
			14, -1, -1, 14, -1, 8, 9, 10, 11, 12 };

	bool matchfound = false;
	string operationCode = commandString.substr(0,4);//first four bits of commandString limit the possible command choices
	/*operation code table
	 0000 bit manipulation/MOVEP/immed late
	 0001 MOVE.B
	 0010 MOVE.L
	 0011 MOVE.W
	 0100 Miscellaneous
	 0101 ADDQ/SUBQ/Scc/DBcc/TRAPcc
	 0110 Bcc/BSR/BRA
	 0111 MOVEQ
	 1000 OR/DIV/SBCD
	 1001 SUB/SUBX
	 1010 (unassigned, Reserved)
	 1011 CMP/EOR
	 1100 AND/MUL/ABCD/EXG
	 1101 ADD/ADDX
	 1110 Shift/rotate/bit field
	 1111 CPU32 extensions
	 */

	int instructionsOfInterest[40] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};//place holder for instruction index
	int numberOfInstructionsToCheck = 0;
	if(operationCode == "0000") {
		//0000 bit manipulation/MOVEP/immed late
		//index command
		instructionsOfInterest[0] = 3;//3     ADDI
		instructionsOfInterest[1] = 7;//7     ANDI
		instructionsOfInterest[2] = 8;//8	  ANDItoCCR
		instructionsOfInterest[3] = 12;//12	BCHG_bit
		instructionsOfInterest[4] = 13;//13	BCHG
		instructionsOfInterest[5] = 14;//14	BCLR_bit
		instructionsOfInterest[6] = 15;//15	BCLR
		instructionsOfInterest[7] = 18;//18	BSET_bit
		instructionsOfInterest[8] = 19;//19	BSET
		instructionsOfInterest[9] = 21;//21	BTST_bit
		instructionsOfInterest[10] = 22;//22	BTST
		instructionsOfInterest[11] = 24;//24	CHK2
		instructionsOfInterest[12] = 28;//28	CMPI
		instructionsOfInterest[13] = 30;//30	CMP2
		instructionsOfInterest[14] = 37;//37	EORI
		instructionsOfInterest[15] = 38;//38	EORItoCCR
		instructionsOfInterest[16] = 55;//55 	MOVEP
		instructionsOfInterest[17] = 67;//67	ORI
		instructionsOfInterest[18] = 68;//68	ORItoCCR
		instructionsOfInterest[19] = 81;//81	SUBI
		instructionsOfInterest[20] = 91;//91	ANDItoSR
		instructionsOfInterest[21] = 92;//92	EORItoSR
		instructionsOfInterest[22] = 96;//96	MOVES
		instructionsOfInterest[23] = 97;//97	ORItoSR
		//24 commands possible
		numberOfInstructionsToCheck = 24;
	}
	else if(operationCode == "0001") {
		//MOVE
		//index command
		instructionsOfInterest[0] = 49;//49    MOVE
		//1 command possible
		numberOfInstructionsToCheck = 1;
	}
	else if(operationCode == "0010" || operationCode == "0011") {
		//MOVE, MOVEA
		//index command
		instructionsOfInterest[0] = 49;//49    MOVE
		instructionsOfInterest[1] = 50;//50	MOVEA
		//2 commands possible (repeat 1 - MOVE)
		numberOfInstructionsToCheck = 2;
	}
	else if(operationCode == "0100") {
		//0100 Misc
		//index command
		instructionsOfInterest[0] = 16;//16	BKPT
		instructionsOfInterest[1] = 23;//23	CHK
		instructionsOfInterest[2] = 25;//25	CLR
		instructionsOfInterest[3] = 33;//33	DIVS_DIVSL_follow
		instructionsOfInterest[4] = 35;//35	DIVU_DIVUL_follow
		instructionsOfInterest[5] = 40;//40	EXT_EXTB
		instructionsOfInterest[6] = 41;//41	ILLEGAL
		instructionsOfInterest[7] = 42;//42	JMP
		instructionsOfInterest[8] = 43;//43	JSR
		instructionsOfInterest[9] = 44;//44	LEA
		instructionsOfInterest[10] = 45;//45	LINK_word
		instructionsOfInterest[11] = 46;//46	LINK_long
		instructionsOfInterest[12] = 51;//51	MOVEfromCCR
		instructionsOfInterest[13] = 52;//52	MOVEtoCCR
		instructionsOfInterest[14] = 53;//53	MOVEfromSR
		instructionsOfInterest[15] = 54;//54	MOVEM
		instructionsOfInterest[16] = 58;//58	MULS_follow
		instructionsOfInterest[17] = 60;//60	MULU_follow
		instructionsOfInterest[18] = 61;//61	NBCD
		instructionsOfInterest[19] = 62;//62	NEG
		instructionsOfInterest[20] = 63;//63	NEGX
		instructionsOfInterest[21] = 64;//64	NOP
		instructionsOfInterest[22] = 65;//65	NOT
		instructionsOfInterest[23] = 69;//69	PEA
		instructionsOfInterest[24] = 74;//74	RTD
		instructionsOfInterest[25] = 75;//75	RTR
		instructionsOfInterest[26] = 76;//76	RTS
		instructionsOfInterest[27] = 84;//84	SWAP
		instructionsOfInterest[28] = 85;//85	TAS
		instructionsOfInterest[29] = 86;//86	TRAP
		instructionsOfInterest[30] = 88;//88	TRAPV
		instructionsOfInterest[31] = 89;//89	TST
		instructionsOfInterest[32] = 90;//90	UNLK
		instructionsOfInterest[33] = 93;//93	MOVEtoSR
		instructionsOfInterest[34] = 94;//94	MOVE_USP
		instructionsOfInterest[35] = 95;//95	MOVEC
		instructionsOfInterest[36] = 98;//98	RESET
		instructionsOfInterest[37] = 99;//99	RTE
		instructionsOfInterest[38] = 100;//100	STOP
		instructionsOfInterest[39] = 101;//101	BGND
		//40 commands possible
		numberOfInstructionsToCheck = 40;
	}
	else if(operationCode == "0101") {
		//0101 ADDQ/SUBQ/Scc/DBcc/TRAPcc
		//index command
		instructionsOfInterest[0] = 4;//4		ADDQ
		instructionsOfInterest[1] = 31;//31	DBcc
		instructionsOfInterest[2] = 78;//78	Scc
		instructionsOfInterest[3] = 82;//82	SUBQ
		instructionsOfInterest[4] = 87;//87	TRAPcc
		//5 command possible
		numberOfInstructionsToCheck = 5;
	}
	else if(operationCode == "0110") {
		//0110 Bcc/BSR/BRA
		//index command
		instructionsOfInterest[0] = 11;//11	Bcc
		instructionsOfInterest[1] = 17;//17	BRA
		instructionsOfInterest[2] = 20;//20	BSR
		//3 commands possible
		numberOfInstructionsToCheck = 3;
	}
	else if(operationCode == "0111") {
		//0111 MOVEQ
		//index command
		instructionsOfInterest[0] = 56;//56	MOVEQ
		//1 command possible
		numberOfInstructionsToCheck = 1;
	}
	else if(operationCode == "1000") {
		//1000 OR/DIV/SBCD
		//index command
		instructionsOfInterest[0] = 32;//32	DIVS_DIVSL
		instructionsOfInterest[1] = 34;//34	DIVU_DIVUL
		instructionsOfInterest[2] = 66;//66	OR
		instructionsOfInterest[3] = 77;//77	SBCD
		//4 commands possible
		numberOfInstructionsToCheck = 4;
	}
	else if(operationCode == "1001") {
		//1001 SUB/SUBX
		//index command
		instructionsOfInterest[0] = 79;//79	SUB
		instructionsOfInterest[1] = 80;//80	SUBA
		instructionsOfInterest[2] = 83;//83	SUBX
		//3 commands possible
		numberOfInstructionsToCheck = 3;
	}
	else if(operationCode == "1010") {
		//1010 (unassigned, Reserved)
	}
	else if(operationCode == "1011") {
		//1011 CMP/EOR
		//index command
		instructionsOfInterest[0] = 26;//26	CMP
		instructionsOfInterest[1] = 27;//27	CMPA
		instructionsOfInterest[2] = 29;//29	CMPM
		instructionsOfInterest[3] = 36;//36	EOR
		//4 commands possible
		numberOfInstructionsToCheck = 4;
	}
	else if(operationCode == "1100") {
		//1100 AND/MUL/ABCD/EXG
		//index command
		instructionsOfInterest[0] = 0;//0		ABCD
		instructionsOfInterest[1] = 6;//6		AND
		instructionsOfInterest[2] = 39;//39	EXG
		instructionsOfInterest[3] = 57;//57	MULS
		instructionsOfInterest[4] = 59;//59	MULU
		//5 commands possible
		numberOfInstructionsToCheck = 5;
	}
	else if(operationCode == "1101") {
		//1101 ADD/ADDX
		//index command
		instructionsOfInterest[0] = 1;//1		ADD
		instructionsOfInterest[1] = 2;//2		ADDA
		instructionsOfInterest[2] = 5;//5		ADDX
		//3 commands possible
		numberOfInstructionsToCheck = 3;
	}
	else if(operationCode == "1110") {
		//1110 Shift/rotate/bit field
		//index command
		instructionsOfInterest[0] = 9;//9		ASL_ASR_sizeSelect
		instructionsOfInterest[1] = 10;//10	ASL_ASR
		instructionsOfInterest[2] = 47;//47	LSL_LSR_sizeSelect
		instructionsOfInterest[3] = 48;//48	LSL_LSR
		instructionsOfInterest[4] = 70;//70	ROL_ROR_register
		instructionsOfInterest[5] = 71;//71	ROL_ROR_memory
		instructionsOfInterest[6] = 72;//72	ROXL_ROXR_register
		instructionsOfInterest[7] = 73;//73	ROXL_ROXR_memory
		//8 command possible
		numberOfInstructionsToCheck = 8;
	}
	else if(operationCode == "1111") {
		//1111 CPU32 extensions
		//index command
		instructionsOfInterest[0] = 102;//102	LPSTOP
		instructionsOfInterest[1] = 103;//103	TBLS_TBLSN_table
		instructionsOfInterest[2] = 104;//104	TBLS_TBLSN_register
		instructionsOfInterest[3] = 105;//105	TBLU_TBLUN_table
		instructionsOfInterest[4] = 106;//106	TBLU_TBLUN_register
		//5 commands possible
		numberOfInstructionsToCheck = 5;
	}

	//loop through all possible instructions after filtering for operation code
	for(int b = 0; b < numberOfInstructionsToCheck; b++) {
		//assemble the instruction code at index "b"
		//first, add the base command of the model instruction, 16 bits which make up the command
		string line2 = "";
		for (int a = 0; a < 16; a++) {
			line2.push_back(instrCPU32[instructionsOfInterest[b]][a]);
		}

		//then, check to see if the model instruction for comparison should have an extra line
		if (instruction_extraFlags[instructionsOfInterest[b]] >= 0 && instruction_extraFlags[instructionsOfInterest[b]] <= 12) {
			//if the extra line is needed, append it
			for (int a = 0; a < 16; a++) {
				int index = instruction_extraFlags[instructionsOfInterest[b]];
				line2.push_back(definedExtra[index][a]);
			}
		}
		//prepare to loop through this model instruction
		int sizeOfComparison = line2.length();

		//count the number of bits from each model instruction which match those
		//of the data file
		int bitMatchCount = 0;
		for (int c = 0; c < sizeOfComparison; c++) //loop through the bits of the model instruction
		{
			//compare the model instruction to the binary data
			if (commandString[c] == line2[c] || line2[c] == 'x') {
				//a single bit match
				bitMatchCount++;

				//if we get a full match between the model instruction and data, interpret as assembly code
				if (bitMatchCount == sizeOfComparison) {
					//outputString = "";//clear the output string placeholder
					string tempStringCommand = "";
					//if the data in commandString matches a command, it outputs a length into commandLength
					//the interpreted Assembly code is appended onto commandString
					int commandLengthTemp = constructInstructionString(stringinstrCPU32[instructionsOfInterest[b]],commandString,tempStringCommand);

					//if we previously found a match for the binary data, and we found another fit, raise another flag
					if(matchfound == true && commandLengthTemp != 0) {
						multipleMatch = true;
						outputString.push_back(' ');
					}

					//raise the flag that we found a match
					if(commandLengthTemp != 0) {
						matchfound = true;
						outputString.append(tempStringCommand);
						commandLength = commandLengthTemp;//will contain the last matched length
					}
				}
			}
		}
		/*if (matchfound == true) {
			//exit the for loop which cycles through the set of instructions for comparison
			break;
		}*/
	}

	//loop through all possible instructions and compare to the input commandString
	/*
	for (int b = 0; b < 107; b++) {
		//assemble the instruction code at index "b"
		//first, add the base command
		string line2 = "";
		for (int a = 0; a < 16; a++) {
			line2.push_back(instrCPU32[b][a]);
		}

		//then, check to see if the code should have an extra line
		if (instruction_extraFlags[b] >= 0 && instruction_extraFlags[b] <= 12) {
			//if the extra line is needed, append it
			for (int a = 0; a < 16; a++) {
				int index = instruction_extraFlags[b];
				line2.push_back(definedExtra[index][a]);
			}
		}

		//prepare to loop through this instruction
		int sizeOfComparison = line2.length();

		//troubleshooting

		//count the number of bits from each instruction which match those
		//of the data file
		int bitMatchCount = 0;
		for (int c = 0; c < sizeOfComparison; c++) //loop through the bits of the instruction
				{
			if (commandString[c] == line2[c] || line2[c] == 'x') {
				//a single bit match
				bitMatchCount++;
				if (bitMatchCount == sizeOfComparison) {
					outputString = "";
					commandLength = constructInstructionString(stringinstrCPU32[b],commandString,outputString);

					if(commandLength != 0) {
						matchfound = true;
					}
				}
			}
		}
		if (matchfound == true) {
			//exit the for loop which cycles through the set of instructions for comparison
			break;
		}
	}*/

	return commandLength;
}

string decimalVersionReader(long address, long &lastFilePosition)
{
	//input address, return value at address or "NAN" if address not allowed
	ifstream myfile("decimalVersion.txt");
	//ofstream testFile("testFile.txt", ios::trunc);
	string value = "NAN";
	string line;
	//string lineAddressString = "";
	long lineAddress = 0;
	long localAddress = 0;
	char delimiter = '\t';
	bool finished = false;
	bool getNextLine = true;
	int lineSize = 0;

	//continue if the file is open
	if (myfile.is_open())
	{
		myfile.seekg(lastFilePosition);
		//as long as we have lines to read in, continue
	    while (finished == false)
	    {

	    	//if we need to read in a new line, do so
	    	if(getNextLine == true)
	    	{
	    		//collect the position before reading in the line
	    		//this position is the beginning of the next line
	    		lastFilePosition += lineSize;
	    		//if no new line is read in, we are at the end of the file
	    		if(!getline(myfile,line))
	    		{
	    			//set to exit the loop
	    			finished = true;
	    		}
	    		getNextLine = false;
	    		lineSize = line.length();
	    		lineAddress = 0;
	    	}

	    	//interpret the line
	    	//if the first character is not a number character, skip the line
	    	if(line[0] < 48 || line[0] > 57)
	    	{
	    		getNextLine = true;
	    		//testFile << "skip line" << '\n';
	    	}

	    	//if the first character is a number, extract the address of the line
	    	//from the first four bytes separated by the delimiter
	    	//the next bytes are the values through which to search, separated and
	    	//terminated by the delimiter
	    	if(getNextLine == false)
	    	{
	    		int delimiterCount = 0;
	    		string byteString = "";
	    		int byteValue = 0;
	    		long addressByteMultiplier = 16777216;
	    		//loop through the line of data
	    		for(int i = 0; i < lineSize; i++)
	    		{
	    			//convert a char to an int
	    			char a = line[i] - 48;
	    			if(line[i] != delimiter)
	    			{
	    				//extract the byteValue
	    	        	int fragment = a;
	    	        	byteValue = byteValue*10 + fragment;
	    			}
	    			else if(delimiterCount < 4 && line[i] == delimiter)
	    			{
	    				//if we reach a delimiter, that ends a byte
	    				//below 4 delimiters, we have address bytes
	    				delimiterCount += 1;
	    				lineAddress += addressByteMultiplier*byteValue;
	    				addressByteMultiplier = addressByteMultiplier/256;
	    				//testFile << " byte: " << byteValue;
	    				byteValue = 0;
	    			}
	    			else if(delimiterCount >= 4 && line[i] == delimiter)
	    			{
	    				//if we reach a delimiter, that ends a byte.
	    				//above 4 delimiters, we have data
	    				delimiterCount += 1;
	    				localAddress = lineAddress + delimiterCount - 5;
	    				//testFile << "addr: " << localAddress << " byte: " << byteValue << '\n';

	    				//if the current byte address matches the requested one, collect it and end
	    				if(localAddress == address)
	    				{
	    				    char stringByte [7];//buffer for the conversion
	    					value = itoa(byteValue,stringByte,10);//convert to string
	    					finished = true;//breaks the while loop
	    					break;//breaks the for loop
	    				}
	    				byteValue = 0;
	    			}
	    		}
    			getNextLine = true;
	    	}
	    }
	}

	myfile.close();
	//testFile.close();
	return value;
}

string decimalVersionReader(long address)
{
	//input address, return value at address or "NAN" if address not allowed
	//always seeks from beginning
	long position = 0;
	string byte = decimalVersionReader(address, position);
	return byte;
}


long decimalVersionFirstAddress()
{
	//outputs the first address in the decimalVersion file
	long firstAddress = 0;
	ifstream myfile("decimalVersion.txt");
	string line;
	long lineAddress = 0;
	char delimiter = '\t';
	bool finished = false;
	bool getNextLine = true;
	int lineSize = 0;

	//continue if the file is open
	if (myfile.is_open())
	{
		//as long as we have lines to read in, continue
	    while (finished == false)
	    {
	    	//if we need to read in a new line, do so
	    	if(getNextLine == true)
	    	{
	    		//if no new line is read in, we are at the end of the file
	    		if(!getline(myfile,line))
	    		{
	    			//set to exit the loop
	    			finished = true;
	    		}
	    		getNextLine = false;
	    		lineSize = line.length();
	    		lineAddress = 0;
	    	}

	    	//interpret the line
	    	//if the first character is not a number character, skip the line
	    	if(line[0] < 48 || line[0] > 57)
	    	{
	    		getNextLine = true;
	    	}

	    	//if the first character is a number, extract the address of the line
	    	//from the first four bytes separated by the delimiter
	    	if(getNextLine == false)
	    	{
	    		int delimiterCount = 0;
	    		string byteString = "";
	    		int byteValue = 0;
	    		long addressByteMultiplier = 16777216;
	    		//loop through the line of data
	    		for(int i = 0; i < lineSize; i++)
	    		{
	    			//convert a char to an int
	    			char a = line[i] - 48;
	    			if(line[i] != delimiter)
	    			{
	    				//extract the byteValue
	    	        	int fragment = a;
	    	        	byteValue = byteValue*10 + fragment;
	    			}
	    			else if(delimiterCount < 4 && line[i] == delimiter)
	    			{
	    				//if we reach a delimiter, that ends a byte
	    				//below 4 delimiters, we have address bytes
	    				delimiterCount += 1;
	    				lineAddress += addressByteMultiplier*byteValue;
	    				addressByteMultiplier = addressByteMultiplier/256;
	    				byteValue = 0;
	    			}
	    			else if(delimiterCount >= 4 && line[i] == delimiter)
	    			{
	    				//if we reach a delimiter, that ends a byte.
	    				//above 4 delimiters, we have data
	    				delimiterCount += 1;
	    				firstAddress = lineAddress;
	    				finished = true;//breaks the while loop
	    				break;//breaks the for loop
	    				byteValue = 0;
	    			}
	    		}
    			getNextLine = true;
	    	}
	    }
	}

	myfile.close();
	return firstAddress;

	/*
	long firstAddress = decimalVersionFirstAddress();
	cout << "firstAddress: " << firstAddress << '\n';
	 */
}

long decimalVersionLastAddress()
{
	//outputs the last address in the decimalVersion file
	long lastAddress = 0;
	ifstream myfile("decimalVersion.txt");
	string line;
	long lineAddress = 0;
	long localAddress = 0;
	char delimiter = '\t';
	bool finished = false;
	bool getNextLine = true;
	int lineSize = 0;

	//continue if the file is open
	if (myfile.is_open())
	{
		//as long as we have lines to read in, continue
	    while (finished == false)
	    {
	    	//if we need to read in a new line, do so
	    	if(getNextLine == true)
	    	{
	    		//if no new line is read in, we are at the end of the file
	    		if(!getline(myfile,line))
	    		{
	    			//set to exit the loop
	    			finished = true;
	    			lastAddress = localAddress;
	    		}
	    		getNextLine = false;
	    		lineSize = line.length();
	    		lineAddress = 0;
	    	}

	    	//interpret the line
	    	//if the first character is not a number character, skip the line
	    	if(line[0] < 48 || line[0] > 57)
	    	{
	    		getNextLine = true;
	    	}

	    	//if the first character is a number, extract the address of the line
	    	//from the first four bytes separated by the delimiter
	    	//the next bytes are the values through which to search, separated and
	    	//terminated by the delimiter
	    	if(getNextLine == false)
	    	{
	    		int delimiterCount = 0;
	    		string byteString = "";
	    		int byteValue = 0;
	    		long addressByteMultiplier = 16777216;
	    		//loop through the line of data
	    		for(int i = 0; i < lineSize; i++)
	    		{
	    			//convert a char to an int
	    			char a = line[i] - 48;
	    			if(line[i] != delimiter)
	    			{
	    				//extract the byteValue
	    	        	int fragment = a;
	    	        	byteValue = byteValue*10 + fragment;
	    			}
	    			else if(delimiterCount < 4 && line[i] == delimiter)
	    			{
	    				//if we reach a delimiter, that ends a byte
	    				//below 4 delimiters, we have address bytes
	    				delimiterCount += 1;
	    				lineAddress += addressByteMultiplier*byteValue;
	    				addressByteMultiplier = addressByteMultiplier/256;
	    				byteValue = 0;
	    			}
	    			else if(delimiterCount >= 4 && line[i] == delimiter)
	    			{
	    				//if we reach a delimiter, that ends a byte.
	    				//above 4 delimiters, we have data
	    				delimiterCount += 1;
	    				localAddress = lineAddress + delimiterCount - 5;
	    				byteValue = 0;
	    			}
	    		}
    			getNextLine = true;
	    	}
	    }
	}
	myfile.close();
	return lastAddress;

	/*
	long lastAddress = decimalVersionLastAddress();
	cout << "lastAddress: " << lastAddress << '\n';
	 */
}

bool numberIsPresentInJumpAddresses(long number) {
	//searches the jump or branch to file for the input number, returns true if present
	//assume that file is formatted such that each line contains a single number

	bool isPresent = false;
	string line = "";

	ifstream myfile("branchOrJumpAddresses.txt");

	if (myfile.is_open())
	{
		//as long as we have lines to read in, continue
	    while (getline(myfile,line)) {
	    	long address = stringDec_to_int(line);
	    	//cout <<"line: " << line << " address: " << address << " comp: " << number << '\n';
	    	if(address == number) {
	    		isPresent = true;
	    	}

	    	if(isPresent == true) {
	    		break;
	    	}
	    }

	}
	myfile.close();
	return isPresent;
}

void numberIsSmallerInJumpAddresses(long number) {
	//searches the jump or branch file and erases any numbers which are smaller than the input number

	string line = "";

	ifstream myfile("branchOrJumpAddresses.txt");
	ofstream temp("temp.txt", ios::trunc);

	if (myfile.is_open())
	{
		//as long as we have lines to read in, continue
	    while (getline(myfile,line)) {
	    	long address = stringDec_to_int(line);
	    	if(address > number) {
	    		temp << line << '\n';
	    	}
	    }
	}
	temp.close();
	myfile.close();
	remove("branchOrJumpAddresses.txt");
	rename("temp.txt","branchOrJumpAddresses.txt");
}

bool  writeVBR_table(string assemblyLine, long currentAddress, string &jumpAddresses)
{
	//input a string which contains the MOVEC to VBR command
	//the function will do one of two things: if the  VBR table
	//is at a lower memory address: re-write the assemblyVersion file up
	//to the location where the vector base register is present, then
	//write in the vector address table while adding those addresses
	//to the jump or branch file, followed by re-writing the assembly
	//code up through the VBR command. if the VBR command is not reproduced
	// output a false flag. If the VBR table is at a higher
	//address that table's addresses are added to the jump or branch file,
	//as are the addresses listed in the table.

	bool returnTrue = true;
	bool overrideResetVector = false;
	//first, we need to figure out what address is being pushed into the VBR
	//so we need to figure out what register stores that value that is being pushed to VBR
	int lengthOfAssemblyLine = assemblyLine.length();
	string fromRegister = "";
	for(int i = 0; i < lengthOfAssemblyLine - 5; i++)
	{
		string command = assemblyLine.substr(i,5);
		if(command == "MOVEC")
		{
			fromRegister = assemblyLine.substr(i+6,2);//2 chars past the MOVEC command is where the register is listed
			break;
		}
		else if(assemblyLine == "write reset VBR")
		{
			overrideResetVector = true;
			break;
		}
	}

	//make sure that this is an address or data register, and that the VBR is not
	//in the from register position.
	if(fromRegister[0] != 'A')
	{
		//this is incorrect formatting, only an address can be put into the VBR as far as I can tell
		returnTrue = false;
	}
	//cout << "from register: " << fromRegister << '\n';

	if(returnTrue == true || overrideResetVector) {
		//next we must figure out what is in this address register
		//start by scanning through the assembly line file to find where the VBR
		//command is at, then working backwards from there looking for the An
		//register
		ifstream readAssembly("assemblyVersion.txt");
		//readAssembly.seekg(0);//make sure we start at the beginnig of the file
		string line = "";
		long targetLine = 0;
		long addressInRegister = 0;
		//assume that the VBR command is the next line to be written, and that the final
		//line in the file is the previous command
		//so set this line as the first target to check
		if(readAssembly.is_open())
		{
			while(getline(readAssembly,line))
			{
				targetLine += 1;
			}
		}

		//cout << "target line " << targetLine << '\n';

		//line now contains the last command issued
		//check to see if a given line pertains to the register of interest
		bool notFound = true;
		while(notFound == true && !overrideResetVector)
		{
			//close, then open the file for reading (resets the read-in position)
			readAssembly.close();
			//ifstream readAssembly("assemblyVersion.txt");
			readAssembly.open("assemblyVersion.txt");
			//readAssembly.seekg(0,ios_base::beg);//reset the seek position to the start of the file
			long currentLine = 0;
			//set the string "line" to the line at targetLine
			if(readAssembly.is_open())
			{
				while(currentLine <= targetLine)
				{
					getline(readAssembly,line);
					currentLine += 1;
				}
			}
			//cout << line << '\n';
			if(stringSearch(fromRegister,line))
			{
				//the line contains the register that gets pushed into VBR
				if(stringSearch("MOVE",line))
				{
					//cout << "current line: " << currentLine << " has MOVE and " << fromRegister << '\n';
					//the command is a move command
					//we must figure out what was put into the register
					//find the position of the MOVE command, look for the first space afterwards
					//include in a new string everything between that space and the first comma
					string pushIntoRegister = "";//place holder to contain the number pushed into the register
					long lineSize = line.length();
					for(int j = 0; j < lineSize-4; j++)
					{
						//find "MOVE"
						string tempString = line.substr(j,4);
						if(tempString == "MOVE")
						{
							//loop past MOVE to find the first space character
							bool begin = false;
							for(int k = j+4; k < lineSize; k++)
							{
								if(begin == true && line[k] != ',')
								{
									pushIntoRegister.push_back(line[k]);
								}
								else if(begin == true && line[k] == ',')
								{
									break;//breaks the loop to find the number pushed into the register
								}
								if(line[k] == ' ')
								{
									begin = true;
								}
							}
							break;//breaks the loop to find "move"
						}
					}
					//now, the string push into register contains our number
					//check it's first character for $ or #
					//cout << "push onto register: " << pushIntoRegister << '\n';
					int pushSize = pushIntoRegister.length();
					string tempString2 = pushIntoRegister.substr(1,pushSize-1);
					if(pushIntoRegister[0] == '$')
					{
						//we have a hex address in tempString2
						string decimal = hexString_to_decimal(tempString2);
						addressInRegister = stringDec_to_int(decimal);
						notFound = false;
					}
					else if(pushIntoRegister[0] == '#')
					{
						//we have a decimal number in tempString2
						addressInRegister = stringDec_to_int(tempString2);
						notFound = false;
					}
					else
					{
						//something went wrong
						//we had a move command which contains our register of interest
						//but the format did not match what we thought
						//look for a different line
						targetLine -= 1;
					}
				}
				else
				{
					//we had the register of interest, but no move command
					//look for a different line
					targetLine -= 1;
				}
			}
			else
			{
				//we did not have the register of interest
				//look for a different line
				targetLine -= 1;
			}

			if(targetLine < 0)
			{
				//we did not find what we were looking for in the file
				//prepare to break the while loop
				notFound = false;
				returnTrue = false;
			}
		}
		readAssembly.close();//close the file we were reading
		//now, addressInRegister should contain the decimal of the address of the VBR
		//cout << fromRegister << " contains: " << addressInRegister << '\n';
		//two conditions may exist: VBR is at a lower address than the current address
		//or VBR may be at a higher address

		//if it is at a higher address, mark that address and the next 4*256 bytes in the
		//jump or branch table. Also add the addresses in that table (each set of 4 bytes is an address)
		//ofstream branchAddresses("branchOrJumpAddresses.txt", ios::app);//store the addresses that the disassembled code branches to
		//branchAddresses.close();
		if(addressInRegister > currentAddress)
		{
			//loop through the addresses and add them to the jump addresses file
			for(int l = 0; l <= 255*4; l++)
			{
				jumpAddresses = addToJumpAddresses(jumpAddresses, addressInRegister+l);

				/*
				//troubleshoot some misbehaving address protection
				if(addressInRegister+l > 0x2018e && addressInRegister+l < 0x20194)
				{
					cout << (int) currentAddress << ' ' << (int) addressInRegister+l << '\n';
				}
				*/

				/*if(!numberIsPresentInJumpAddresses(addressInRegister+l))
				{
					//if the number is not present in the branch or jump file, add it
					branchAddresses.open("branchOrJumpAddresses.txt", ios::app);//re-open for writing
					branchAddresses << addressInRegister+l << '\n';
					branchAddresses.close();
				}*/
			}

			//next, read the addresses in those locations and add those to the branch or jump file
			long seekPosition = 0;
			for(int m = 0; m <= 255*4; m+=4)
			{
				string bitsOfAddress = "";
				string byte0 = decimalVersionReader(addressInRegister+m,seekPosition);
				string byte1 = decimalVersionReader(addressInRegister+m+1,seekPosition);
				string byte2 = decimalVersionReader(addressInRegister+m+2,seekPosition);
				string byte3 = decimalVersionReader(addressInRegister+m+3,seekPosition);

				long factor = 256*256*256;
				long byte0Int = stringDec_to_int(byte0);
				byte0Int = byte0Int*factor;
				factor = factor/256;
				long byte1Int = stringDec_to_int(byte1);
				byte1Int = byte1Int*factor;
				long byte2Int = stringDec_to_int(byte2);
				byte2Int = byte2Int*256;
				long byte3Int = stringDec_to_int(byte3);

				long addressInWhole = byte0Int + byte1Int + byte2Int + byte3Int;

				//check to see if the address is above or below the current address
				//if it is above the current address, then add to the jump/branch file
				if(addressInWhole > currentAddress)
				{
				//check to see if the address is in the jump or branch file already
					jumpAddresses = addToJumpAddresses(jumpAddresses, addressInWhole);

					/*
					//troubleshoot some misbehaving address protection
					if(addressInWhole > 0x2018e && addressInWhole < 0x20194)
					{
						cout << (int) currentAddress << ' ' << (int) addressInWhole << '\n';
					}
					 */

					/*if(!numberIsPresentInJumpAddresses(addressInWhole))
					{
						branchAddresses.open("branchOrJumpAddresses.txt", ios::app);//re-open for writing
						branchAddresses << addressInWhole << '\n';
						branchAddresses.close();
					}*/
				}
				else
				{
					//if the address is below the current address, check that address to see if
					//a command begins there
					bool addressFound = false;
					string storeLine = "";
					readAssembly.open("assemblyVersion.txt");
					while(getline(readAssembly,storeLine))
					{
						string address = storeLine.substr(2,6);
						address = hexString_to_decimal(address);
						long lineAddress = stringDec_to_int(address);
						if(lineAddress == addressInWhole) // there is a line of code at the address referenced by the vector table
						{
							addressFound = true;
							break;//exit the while loop
						}
					}
					readAssembly.close();

					if(addressFound == false)
					{
						returnTrue = false;
					}
				}
			}
		}
		else if(addressInRegister < currentAddress)
		{
			//rewrite the assembly code file up to the address in the register
			ofstream tempFile("tempAssembly.txt", ios::trunc);
			string storeLine = "";
			readAssembly.open("assemblyVersion.txt");
			while(getline(readAssembly,storeLine))
			{
				long lineAddress = 0;
				if(storeLine[0] == '0')
				{
					string address = storeLine.substr(2,6);
					address = hexString_to_decimal(address);
					lineAddress = stringDec_to_int(address);
				}
				if(lineAddress < addressInRegister)
				{
					tempFile << storeLine << '\n';
				}
				else
				{
					break; //end the while loop
				}
			}
			readAssembly.close();

			//then write the hex of the vector table while reading in the addresses referenced

			//if those addresses are higher than the current address and are
			//not in the jump or branch file, add them to it

			//if those addresses are lower than the current address, look for a command at that address

			//next, read the addresses in those locations and add those to the branch or jump file
			long seekPosition = 0;
			for(int m = 0; m <= 255*4; m+=4)
			{
				string bitsOfAddress = "";
				string byte0 = decimalVersionReader(addressInRegister+m,seekPosition);
				string byte1 = decimalVersionReader(addressInRegister+m+1,seekPosition);
				string byte2 = decimalVersionReader(addressInRegister+m+2,seekPosition);
				string byte3 = decimalVersionReader(addressInRegister+m+3,seekPosition);


				char stringByte [14];
				string addressHex = itoa(addressInRegister+m,stringByte,16);
				string hexAddress = "000000";
				int addressLength = addressHex.length();
				for(int i = 0; i < addressLength; i++)
				{
					hexAddress[i + 6 - addressLength] = addressHex[i];
				}

				string byte0Hex = stringNumber_to_hex(byte0);
				string byte1Hex = stringNumber_to_hex(byte1);
				string byte2Hex = stringNumber_to_hex(byte2);
				string byte3Hex = stringNumber_to_hex(byte3);

				char tempByteAdd = '0';
				if(byte0Hex.length() < 2)
				{
					tempByteAdd = byte0Hex[0];
					byte0Hex = '0';
					byte0Hex.push_back(tempByteAdd);
				}
				if(byte1Hex.length() < 2)
				{
					tempByteAdd = byte1Hex[0];
					byte1Hex = '0';
					byte1Hex.push_back(tempByteAdd);
				}
				if(byte2Hex.length() < 2)
				{
					tempByteAdd = byte2Hex[0];
					byte2Hex = '0';
					byte2Hex.push_back(tempByteAdd);
				}
				if(byte3Hex.length() < 2)
				{
					tempByteAdd = byte3Hex[0];
					byte3Hex = '0';
					byte3Hex.push_back(tempByteAdd);
				}
				//exception description
				string exception = "";
				if(m == 0)
				{
					exception = "Reset:initial stack pointer";
				}
				else if(m == 4)
				{
					exception = "Reset:initial program counter";
				}
				else if(m == 8)
				{
					exception = "Bus error or access fault";
				}
				else if(m == 12)
				{
					exception = "Address error";
				}
				else if(m == 16)
				{
					exception = "Illegal instruction";
				}
				else if(m == 20)
				{
					exception = "Zero division of integer";
				}
				else if(m == 24)
				{
					exception = "CHK, CHK2 instructions";
				}
				else if(m == 28)
				{
					exception = "TRAPcc, TRAPV instructions";
				}
				else if(m == 32)
				{
					exception = "Privilege violation";
				}
				else if(m == 36)
				{
					exception = "Trace";
				}
				else if(m == 40)
				{
					exception = "Line 1010 emulator";
				}
				else if(m == 44)
				{
					exception = "Line 1111 emulator";
				}
				else if(m == 48)
				{
					exception = "Hardware breakpoint";
				}
				else if(m == 52)
				{
					exception = "(Reserved, coprocessor protocol violation)";
				}
				else if(m == 56 || m == 60)
				{
					exception = "Format error and uninitialized interrupt";
				}
				else if(m == 96)
				{
					exception = "Spurious interrupt";
				}
				else if(m == 100)
				{
					exception = "Level 1 interrupt autovector";
				}
				else if(m == 104)
				{
					exception = "Level 2 interrupt autovector";
				}
				else if(m == 108)
				{
					exception = "Level 3 interrupt autovector";
				}
				else if(m == 112)
				{
					exception = "Level 4 interrupt autovector";
				}
				else if(m == 116)
				{
					exception = "Level 5 interrupt autovector";
				}
				else if(m == 120)
				{
					exception = "Level 6 interrupt autovector";
				}
				else if(m == 124)
				{
					exception = "Level 7 interrupt autovector";
				}
				else if(m >= 128 && m <= 188)
				{
					int trapNumber = (m - 128)/4;
					char stringByte[8];
					string trapNumString = itoa(trapNumber,stringByte,10);
					exception = "Trap Instruction vector ";
					exception.append(trapNumString);
				}
				else if(m >= 192 && m <= 232)
				{
					exception = "(reserved, coprocessor)";
				}
				else if((m >= 236 && m <= 252) || (m >= 64 && m <= 92))
				{
					exception = "(unassigned, reserved)";
				}
				else if(m >= 256 && m <= 1020)
				{
					exception = "User defined vectors";
				}
				//write that hex address to the assembly file
				tempFile << "0x" << hexAddress << ':' << '\t' << byte0Hex << byte1Hex << byte2Hex << byte3Hex << '\t' << exception << '\n';


				long factor = 256*256*256;
				long byte0Int = stringDec_to_int(byte0);
				byte0Int = byte0Int*factor;
				factor = factor/256;
				long byte1Int = stringDec_to_int(byte1);
				byte1Int = byte1Int*factor;
				long byte2Int = stringDec_to_int(byte2);
				byte2Int = byte2Int*256;
				long byte3Int = stringDec_to_int(byte3);

				long addressInWhole = byte0Int + byte1Int + byte2Int + byte3Int;

				//check to see if the address is above or below the current address
				//if it is above the current address, then add to the jump/branch file
				if(addressInWhole > currentAddress)
				{
					//check to see if the address is in the jump or branch file already
					jumpAddresses = addToJumpAddresses(jumpAddresses, addressInWhole);

					/*
					//troubleshoot some misbehaving address protection
					if(addressInWhole > 0x2018e && addressInWhole < 0x20194)
					{
						cout << (int) currentAddress << ' ' << (int) addressInWhole << '\n';
					}
					*/

					/*if(!numberIsPresentInJumpAddresses(addressInWhole))
					{
						branchAddresses.open("branchOrJumpAddresses.txt", ios::app);//re-open for writing
						branchAddresses << addressInWhole << '\n';
						branchAddresses.close();
					}*/
				}
				else
				{
					//if the address is below the current address, check that address to see if
					//a command begins there
					bool addressFound = false;
					string storeLine = "";
					readAssembly.open("assemblyVersion.txt");
					while(getline(readAssembly,storeLine))
					{
						string address = storeLine.substr(2,6);
						address = hexString_to_decimal(address);
						long lineAddress = stringDec_to_int(address);
						if(lineAddress == addressInWhole) // there is a line of code at the address referenced by the vector table
						{
							addressFound = true;
							break;//exit the while loop
						}
					}
					readAssembly.close();

					if(addressFound == false)
					{
						returnTrue = false;
					}
				}
			}
			//once the table is copied and the addresses are protected, check that the following commands
			//(up to the current address) begin appropriately after the vector table.

			//if commands begin at the correct spot at the end of the vector table, copy those commands
			//to the new assembly file (if the commands don't properly begin, then the whole vector
			//table may have been improperly identified- signal as error)

			storeLine = "";
			readAssembly.open("assemblyVersion.txt");
			while(getline(readAssembly,storeLine))
			{
				long lineAddress = 0;
				if(storeLine[0] == '0')
				{
					string address = storeLine.substr(2,6);
					address = hexString_to_decimal(address);
					lineAddress = stringDec_to_int(address);
				}
				if(lineAddress > addressInRegister+255*4)
				{
					tempFile << storeLine << '\n';
				}
			}
			readAssembly.close();
			tempFile.close();

			//delete the old assembly version file, rename the temp file

			remove("assemblyVersion.txt");
			rename("tempAssembly.txt","assemblyVersion.txt");
		}
	}
	return returnTrue;
}

string addToJumpAddresses(string jumpAddresses, long addressToAdd)
{
	//checks to see if addressToAdd is already present in the
	//jumpAddresses string, if it is not, add it into the string
	//in numerical order
	string newJumpAddresses = "";
	//first, check to see if the addressToAdd is already present
	if(!numberIsPresentInJumpAddresses(jumpAddresses, addressToAdd))
	{
		string tempString = "";
		int tempStringEnd = 0;
		long addressFromList = 0;
		long jumpAddressStringLength = jumpAddresses.length();
		//then convert the input number to a string
		char stringByte [20];//buffer for the conversion
		string value = itoa(addressToAdd,stringByte,10);//convert to string

		for(int i = 0; i < jumpAddressStringLength; i++)
		{
			//read in each progressive address
			if(jumpAddresses[i] == ' ')//denotes beginning of address entry
			{
				for(int j = i+1; j < jumpAddressStringLength; j++)
				{
					if(jumpAddresses[j] != '-')//denoted end of address entry
					{
						tempString.push_back(jumpAddresses[j]);
					}
					else
					{
						tempStringEnd = j;
						break;
					}
				}
				addressFromList = stringDec_to_int(tempString);
				//cout << "address from list dec: " << addressFromList << " address string: " << tempString << '\n';

				if(addressFromList > addressToAdd)
				{
					//add the address to the list before the addressFromList position
					//first, split the jumpAddresses array into two parts
					string tempFirstHalf = jumpAddresses.substr(0,i);
					string tempSecondHalf = jumpAddresses.substr(i,jumpAddressStringLength-i);
					//then combine all the parts, including the start and end flags
					newJumpAddresses.append(tempFirstHalf);
					newJumpAddresses.push_back(' ');
					newJumpAddresses.append(value);
					newJumpAddresses.push_back('-');
					newJumpAddresses.append(tempSecondHalf);
					//cout << "1: " << tempFirstHalf << " 2: " << value << " 3: " << tempSecondHalf << '\n';
					break;
				}
				else
				{
					i = tempStringEnd;
					tempString = "";
				}
			}
		}
		//in case the input number is higher than any address in the list
		//add it to the end
		if(tempString == "")
		{
			newJumpAddresses = jumpAddresses;
			newJumpAddresses.push_back(' ');
			newJumpAddresses.append(value);
			newJumpAddresses.push_back('-');
		}
	}
	else
	{
		//if the number is already present, do nothing
		newJumpAddresses = jumpAddresses;
	}
	return newJumpAddresses;
}

string removeLowerJumpAddresses(string jumpAddresses, long addressThreshold)
{
	//removes from jumpAddresses the addresses below addressThreshold
	string newJumpAddresses = jumpAddresses;
	string tempString = "";
	int tempStringEnd = 0;
	long addressFromList = 0;
	long jumpAddressStringLength = jumpAddresses.length();
	for(int i = 0; i < jumpAddressStringLength; i++)
	{
		//read in each progressive address
		if(jumpAddresses[i] == ' ')//denotes beginning of address entry
		{
			for(int j = i+1; j < jumpAddressStringLength; j++)
			{
				if(jumpAddresses[j] != '-')//denoted end of address entry
				{
					tempString.push_back(jumpAddresses[j]);
				}
				else
				{
					tempStringEnd = j;
					break;
				}
			}
			addressFromList = stringDec_to_int(tempString);
			if(addressFromList > addressThreshold)
			{
				//create a new string
				newJumpAddresses = jumpAddresses.substr(i,jumpAddressStringLength-i);
				break;
			}
			else
			{
				i = tempStringEnd;
				tempString = "";
			}
		}
	}
	return newJumpAddresses;
}

string removeHigherJumpAddresses(string jumpAddresses, long addressThreshold)
{
	//removes from jumpAddresses the addresses above addressThreshold
	string newJumpAddresses = jumpAddresses;
	string tempString = "";
	int tempStringEnd = 0;
	long addressFromList = 0;
	long jumpAddressStringLength = jumpAddresses.length();
	for(int i = 0; i < jumpAddressStringLength; i++)
	{
		//read in each progressive address
		if(jumpAddresses[i] == ' ')//denotes beginning of address entry
		{
			for(int j = i+1; j < jumpAddressStringLength; j++)
			{
				if(jumpAddresses[j] != '-')//denoted end of address entry
				{
					tempString.push_back(jumpAddresses[j]);
				}
				else
				{
					tempStringEnd = j;
					break;
				}
			}
			addressFromList = stringDec_to_int(tempString);
			if(addressFromList > addressThreshold)
			{
				//create a new string
				newJumpAddresses = jumpAddresses.substr(0,i);
				break;
			}
			else
			{
				i = tempStringEnd;
				tempString = "";
			}
		}
	}
	return newJumpAddresses;
}

bool numberIsPresentInJumpAddresses(string jumpAddresses, long address)
{
	//returns true if address is present in jumpAddresses
	bool isPresent = false;
	string tempString = "";
	int tempStringEnd = 0;
	long addressFromList = 0;
	long jumpAddressStringLength = jumpAddresses.length();
	//cout << jumpAddressStringLength << ", " << address << ", " << jumpAddresses << '\n';
	for(int i = 0; i < jumpAddressStringLength; i++)
	{
		//read in each progressive address
		if(jumpAddresses[i] == ' ')//denotes beginning of address entry
		{
			for(int j = i+1; j < jumpAddressStringLength; j++)
			{
				if(jumpAddresses[j] != '-')//denoted end of address entry
				{
					tempString.push_back(jumpAddresses[j]);
				}
				else
				{
					tempStringEnd = j;
					break;
				}
			}
			addressFromList = stringDec_to_int(tempString);
			//cout << addressFromList << ' ';
			if(addressFromList == address)
			{
				isPresent = true;
				break;
			}
			else if(addressFromList > address)
			{
				//the list is ordered in increasing value
				//if we're already higher than the
				//value of interest, it is not present
				break;
			}
			else
			{
				i = tempStringEnd;
				tempString = "";
			}
		}
	}
	//cout << '\n';
	return isPresent;
}
