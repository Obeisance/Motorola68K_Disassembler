//============================================================================
// Name        : MotorolaDisassembler.cpp
// Author      : Obeisance
// Version     :
// Copyright   : Free to use for all
// Description : disassemble the instruction code of a Motorola 68330 - CPU32
//============================================================================

#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <math.h>
#include "assemblyInstructions.h"
#include "stringAndNumberHandlers.h"
using namespace std;


void convert_s_record(ifstream &srec);
//takes in a motorola s record file, and writes a file "decimal version" formatted
//the way I first wrote the interpreter

//int jumpCommand(string command);
long jumpCommand(string command, long address);
//input a command, and output an integer which contains the number
//of displacement bytes to the next command

string extractAddress(string original);
//extracts an address as indicated by a hex preceded by '($' and terminated by ')'

long long convertAddress(string assembly);
//input some assembly code, extract an address which is not part of a branch or LEA routine
//outputs the address as a decimal

bool numberIsPresentInDataAddresses(long number);
//searches the possible data addresses to file for the input number, returns true if present

void numberIsSmallerInDataAddresses(long number);
//searches the data address file and erases any numbers which are smaller than the input number

int main() {

	  //string line;
	  //string convert;

	  //convert the s record to a decimal file

	  //file name of the S record to be disassembled
	  //ifstream s_record("tutorial1.S68");
	  //ifstream s_record("tutorial2.S68");
	  //ifstream s_record("tutorial4.S68");
	  ifstream s_record("6-16-06 ECU Read");
	  convert_s_record(s_record);
	  s_record.close();

	  //prepare a bunch of storage variables
	  string binaryInstruction = "";//used to store bits of data
	  long lastAddressInBinaryInstruction = -1;//track the bits we are looking at
	  long firstAddressInBinaryInstruction = 0;

	  ofstream writeAssembly("assemblyVersion.txt", ios::trunc);//store our disassembled code
	  ofstream branchAddresses("branchOrJumpAddresses.txt", ios::trunc);//store the addresses that the disassembled code branches to
	  ofstream possDataAddresses("DataAddresses.txt", ios::trunc);//store the addresses that the disassembled code accesses -> data
	  bool notFinished = true;//a flag to indicate we're done
	  bool multipleMatchedCommands = false;//a flag to indicate that a piece of binary data matches more than one assembly machine instruction
	  int bytesToShift = 2;//track the number of bytes to the next instruction
	  string SP = "";//stack pointer location
	  //int stackPointer = 0;
	  //string SI = "";
	  bool matchfound = false;//short term storage indicating we found a match between a command and binary in the file
	  long startAddress = decimalVersionFirstAddress();//first address in the binary file
	  long endAddress = decimalVersionLastAddress();//last address in the binary file
	  long seekPosition = 0;//store where we last accessed the binary file

	  //prepare for the first address to start looking for commands
	  long addressOfInterest = 0;
	  if(startAddress == 0)
	  {
		  //addresses 4-7 contain the address of the first instruction
		  long addressMultiplier = 16777216;
		  for(int i = 4; i < 8; i++)
		  {
			  string byteValString = decimalVersionReader(i);//extract the byte from the file as a string
			  int byteVal = stringDec_to_int(byteValString);//convert the string Decimal to an int
			  addressOfInterest += byteVal*addressMultiplier;//add to the address of interest
			  addressMultiplier = addressMultiplier/256;
		  }
	  }
	  else
	  {
		  addressOfInterest = startAddress;//otherwise, start looking for instructions at the first address in the file
	  }


	  //loop until we decide that all instructions have been interpreted
	  while(notFinished == true)
	  {
		  //assemble the set of binary values which are to be interpreted as an instruction
		  //we want 128 bits (16 bytes, for now)
		  //binaryInstruction = ""; //->only use when not using "if(addressOfInterest + i > lastAddressInBinaryInstruction)" to fully build command string each time
		  int addressesInBinaryInstruction = 16;
		  for(int i = 0; i < addressesInBinaryInstruction; i++)
		  {
			  if(addressOfInterest + i > lastAddressInBinaryInstruction)
			  {
				  string decimalNumberString = decimalVersionReader(addressOfInterest + i, seekPosition);//extract the byte from the file as a string

				  if(decimalNumberString != "NAN")
				  {
					  string binaryNumber = decString_to_binary(decimalNumberString);//convert to binary string
					  binaryInstruction.append(binaryNumber);//append to the set of bytes of interest
				  }
				  else //if(addressOfInterest + 1 + addressesInBinaryInstruction <= endAddress)
				  {
					  //in case we get a NAN that is not at the end of the file, keep reading in addresses
					  //by extending the number of addresses added in the loop
					  //addressesInBinaryInstruction += 1;
					  binaryInstruction.append("11111111");//append to the set of bytes of interest a command that means nothing
					  seekPosition = 0;
				  }
			  }
		  }
		  //instructionAddresses << "firstAddr: " << firstAddressInBinaryInstruction << ", lastAddr: " << lastAddressInBinaryInstruction << '\n';
		  //instructionAddresses << "Addr: " << addressOfInterest << ", bits: " << binaryInstruction << '\n';
		  firstAddressInBinaryInstruction = addressOfInterest;
		  lastAddressInBinaryInstruction = addressOfInterest + addressesInBinaryInstruction - 1;

		  //if we try to use bits beyond the end of the file, we are done (this artificially
		  //ends the search with 16 bytes of data at the end of the file having not been
		  //searched
		  //also, if binary data has more than one assembly instruction match, we should stop
		  if(addressOfInterest >= endAddress || multipleMatchedCommands == true)
		  {
			  notFinished = false;
		  }

		  //if we have extracted the 128 bits to check, compare
		  //to all of the possible instruction files
		  if(binaryInstruction.length() == 128)
		  {
			  //check the instruction to see if it matches the basic format of a command
			  string assemblySyntax = "";
			  bytesToShift = guessCommand(binaryInstruction,assemblySyntax,multipleMatchedCommands);

			  //if the match was a jump command figure out where the jump directs to
			  long displacementBytes = jumpCommand(assemblySyntax, addressOfInterest);
			  //cout<<bytesToShift << ' ' << assemblySyntax << '\n';
			  //write the instruction to the assembly code file, write jump address targets to branchAddress file
			  if(bytesToShift != 0)
			  {
				  //first, lets make sure that we are not writing an instruction which
				  //overlaps with a jump or possible data
				  bool safeJump = true;
				  //possDataAddresses.close();
				  branchAddresses.close();//cannot access the file if it is still in the write buffer
				  //loop through the addresses which make up the present instruction
				  //compare them to data, jump or branch locations and flag if data or a jump or branch
				  //indicates a landing point within the instruction
				  /*if(numberIsPresentInDataAddresses(addressOfInterest)) {
					  safeJump = false;
					  writeAssembly << "possible data address " << assemblySyntax << ' ';
				  }*/
				  for(int z = 1; z < bytesToShift; z++) {
					  if(numberIsPresentInJumpAddresses(addressOfInterest+z)) {
						  safeJump = false;
						  //cout << "Reject addr: " << addressOfInterest << ", assembly: " << assemblySyntax << '\n';
						  string hexAddressOfReject = decAddr_to_hexAddr(addressOfInterest);
						  writeAssembly << hexAddressOfReject << ':' << '\t' << "reject due to branch conflict: " << assemblySyntax << ' ';
						  break;
					  }
					  /*else if(numberIsPresentInDataAddresses(addressOfInterest+z)) {
						  safeJump = false;
						  //writeAssembly << "possible data address ";
						  break;
					  }*/
				  }
				  //extract an effective address from data
				  //long potentialDataAddress = convertAddress(assemblySyntax);
				  //numberIsSmallerInDataAddresses(addressOfInterest);//remove any numbers from the file which are smaller than our current position in order to keep the file short
				  //bool dataAddressPresent = numberIsPresentInDataAddresses(potentialDataAddress);//don't add same address twice
				  branchAddresses.open("branchOrJumpAddresses.txt", ios::app);//re-open for writing
				  //ofstream possDataAddresses("DataAddresses.txt", ios::app);


				  //if(potentialDataAddress > addressOfInterest && !dataAddressPresent && potentialDataAddress < endAddress) {
				//	  possDataAddresses << potentialDataAddress << '\n';
				  //}

				  //if the locations are all clear, then proceed to write to the assembly file
				  if(safeJump == true) {

					  //check for the possibility that the new line is one where a VBR is set
					  if(stringSearch("VBR", assemblySyntax))
					  {
						  //cout << '\n' << "found VBR at: " << addressOfInterest << '\n';
						  writeAssembly.close();//close the file to store our disassembled code so it saves current progress
						  branchAddresses.close();
						  if(writeVBR_table(assemblySyntax, addressOfInterest))
						  {

						  }
						  writeAssembly.open("assemblyVersion.txt", ios::app);//reopen the file to store our disassembled code
						  branchAddresses.open("branchOrJumpAddresses.txt", ios::app);//re-open for writing
					  }

					  //then proceed to write to the assembly file
					  string hexAddress = decAddr_to_hexAddr(addressOfInterest);
					  writeAssembly << hexAddress << ':' << '\t';
					  writeAssembly << assemblySyntax;
					  if(displacementBytes != 0)
					  {
						  long jump = addressOfInterest + displacementBytes;
						  string jumpAddr = decAddr_to_hexAddr(jump);
						  writeAssembly << " displacement: " << displacementBytes << " points to: " << jumpAddr;
						  //branchAddresses << "Addr: " << addressOfInterest << " jump to: " << jumpAddr << " or: " << jump << '\n';
						  branchAddresses.close();
						  bool alreadyPresent = numberIsPresentInJumpAddresses(jump);//is this address already present in the file?
						  numberIsSmallerInJumpAddresses(addressOfInterest);//remove any numbers from the file which are smaller than our current position in order to keep the file short
						  branchAddresses.open("branchOrJumpAddresses.txt", ios::app);
						  if(displacementBytes > 0 && !alreadyPresent) {
							  branchAddresses << jump << '\n';
						  }
					  }
					  matchfound = true;
				  }
			  }

			  //if a match was found, move the address of interest by the number of bytes in the instruction
			  if(matchfound == true)
			  {
				  matchfound = false;
				  string instructionBits = binaryInstruction.substr(0, 8*bytesToShift);
				  string instructionBitsHex = binaryString_to_hexString(instructionBits);
				  string instructionBitsChar = binaryString_to_charString(instructionBits);
				  writeAssembly << '\t' << addressOfInterest << " + " << bytesToShift << '\t' << instructionBits << '\t' << instructionBitsHex << '\t' << instructionBitsChar << '\n';
				  addressOfInterest += bytesToShift;

				  //then, shift the binary instruction which we are trying
				  //to match by (8 bits)*number of addresses of the
				  //matched instruction
				  int sizeOfBinaryInstructionMatch = binaryInstruction.length();

				  //since we have a match, if we are shifting by fewer bytes than are stored in our comparison array of bits, shift that array
				  if(bytesToShift*8 <= sizeOfBinaryInstructionMatch)
				  {
					  binaryInstruction = binaryInstruction.substr(8*bytesToShift, sizeOfBinaryInstructionMatch - 8*bytesToShift);
					  firstAddressInBinaryInstruction += bytesToShift;
				  }
				  else
				  {
					  //otherwise, clear that array to begin refilling it because our instruction was too long.
					  binaryInstruction = "";
				  }

				  //writeAssembly << binaryInstruction << '\n';
			  }
			  else
			  {
				  //if we do not have an instruction match, write this to the assembly file
				  //begin by finding the address where we failed to match
				  char stringByte [14];
				  string addressHex = itoa(addressOfInterest,stringByte,16);
				  string hexAddress = "000000";
				  int addressLength = addressHex.length();
				  for(int i = 0; i < addressLength; i++)
				  {
					  hexAddress[i + 6 - addressLength] = addressHex[i];
				  }
				  //write that hex address to the assembly file
				  writeAssembly << "0x" << hexAddress << ':' << '\t';

				  //print the bits which we could not match and their decimal address
				  string shortBinaryInstruction = binaryInstruction.substr(0,16);
				  string shortBinaryInstructionHex = binaryString_to_hexString(shortBinaryInstruction);
				  string shortBinaryInstructionChar = binaryString_to_charString(shortBinaryInstruction);
				  writeAssembly << '\t' << addressOfInterest << ": no match " << shortBinaryInstruction << '\t' << shortBinaryInstructionHex << '\t' << shortBinaryInstructionChar << '\n';
				  //cout << addressOfInterest << ": no match " << '\n';

				  //look two bytes later to find the next instruction
				  addressOfInterest += 2;

				  //then, shift the binary instruction which we are trying
				  //to match by 8 bits
				  int sizeOfBinaryInstructionNoMatch = binaryInstruction.length();
				  binaryInstruction = binaryInstruction.substr(8*2, sizeOfBinaryInstructionNoMatch - 8*2);
				  firstAddressInBinaryInstruction += 2;
			  }
		  }
		  else
		  {
			  //cout << "not enough bits" << '\n';
			  //if we don't have 32 bits for comparison, get the next
			  //line of data and be sure to get 16 bits
			  //writeAssembly << '\n';
			  //getNextLine = true;

			  //if we decided to step backwards in memory, reset the file read in variables
			  if(addressOfInterest < firstAddressInBinaryInstruction)
			  {
				  //cout << "here";
				  //return to the beginning of the file and reset the instruction counters
				  /*readbin.seekg(0, readbin.beg);
				  address = -8; //(-4 for the first read, and another -4 for the first data line)
				  binaryInstruction = "";
				  firstAddressInBinaryInstruction = 0;*/
			  }
			  else if(addressOfInterest > firstAddressInBinaryInstruction)
			  {
				  //otherwise, if we step the address too far forwards, clear the stored bits
				  binaryInstruction = "";
			  }
		  }




	  }

	  writeAssembly.close();
	  branchAddresses.close();
	  possDataAddresses.close();
	  remove("branchOrJumpAddresses.txt");
	  remove("DataAddresses.txt");
	return 0;
}


void convert_s_record(ifstream &srec)
{
	//takes in a motorola s record file, and writes a file "decimal version" formatted
	//the way I first wrote the interpreter
	ofstream writedec("decimalVersion.txt", ios::trunc);
	string line;
	string addressHex = "";
	string s_record_type = "";
	bool firstLine = true;
	long address = 0;

	if (srec.is_open())
	{
	  while (getline (srec,line))
	  {
		  if((line[0] == 'S' || line[0] =='s') && line[1] != '0' && firstLine == true)
		  {
			  //the first line output to writedec should not be data
			  writedec << "Interpreted file";// << '\n';
			  firstLine = false;//we've put something in the first line of the file
		  }

		  if(line[1] != '0' && line[1] != '4' && line[1] != '5' && line[1] != '6' && line[1] != '7' && line[1] != '8' && line[1] != '9')
		  {
			  s_record_type = line[1];
			  //the s record type indicates the format of the instruction
			  //for instance,  the number of bits used to write the address of the line
			  //16 bit address - s1, s5, s9
			  //24 bit address - s2, s6, s8
			  //32 bit address - s3, s7
			  //address field = '0000' - s0
			  int addressLength = 0;
			  if(s_record_type == "1" || s_record_type == "5" || s_record_type == "9")
			  {
				  addressLength = 4;
			  }
			  else if(s_record_type == "2" || s_record_type == "6" || s_record_type == "8")
			  {
				  addressLength = 6;
			  }
			  else if(s_record_type == "3" || s_record_type == "7")
			  {
				  addressLength = 8;
			  }

			  //the next two bits are the byte count of the line
			  int instructionLength = 0;
			  string hexNumberOfBytes = "";
			  hexNumberOfBytes.push_back(line[2]);
			  hexNumberOfBytes.push_back(line[3]);
			  hexNumberOfBytes = hexString_to_decimal(hexNumberOfBytes); //convert to decimal string
			  instructionLength = stringDec_to_int(hexNumberOfBytes); //convert from string to int
			  //cout << "instruction length dec string: " << hexNumberOfBytes << ", instruction length dec: " << instructionLength << '\n';
			  instructionLength -= (addressLength/2 + 1);//the byte count includes the checksum and address, which we're not interested in
			  //cout << "addressLength: " << addressLength << ", instruction length dec: " << instructionLength << '\n';


			  //the next handful of bytes contain the address of the line
			  long addressPoint = 0;
			  int mult = 1;
			  for(int c = 1; c < addressLength/2; c++)
			  {
				  mult = mult*256;
			  }
			  for(int a = 0; a < addressLength; a = a + 2)
			  {
				  string tempAddressNumber = "";
				  tempAddressNumber.push_back(line[a+4]);
				  tempAddressNumber.push_back(line[a+5]);
				  tempAddressNumber = hexString_to_decimal(tempAddressNumber);
				  //writedec << tempAddressNumber << '\t';
				  int temp = stringDec_to_int(tempAddressNumber); //convert from string to int
				  addressPoint += temp*mult;
				  mult = mult/256;
			  }
			  //cout << "line address: " << addressPoint << ", address: " << address << '\n';
			  //make sure our accounting of address is correct
			  /*if(addressPoint < address)
			  {
				  //somehow some s records duplicate data from line to line
				  instructionLength = 0;
			  }*/
			  if(addressPoint != address)// && address == 0)
			  {
				  //adjust the first value of address to match that of the s record file
				  address = addressPoint;
			  }



			  //then loop through the data
			  int startPoint = addressLength + 4;//ex. S1 24 0000 data
			  for(int b = startPoint; b < 2*instructionLength + startPoint; b = b + 2)
			  {
				  //if four addresses have been written
				  if(address % 4 == 0)
				  {
					  //switch to a new line
					  writedec << '\n';
					  //write the address of this starting point
					  string newAddress = makeAddress(address, 4);
					  writedec << newAddress;
				  }
				  string data = "";
				  data.push_back(line[b]);
				  data.push_back(line[b+1]);
				  data = hexString_to_decimal(data);
				  writedec << data << '\t';

				  address += 1;//address of next data point
			  }
		  }
		  else
		  {
			  if(firstLine == true)
			  {
				  writedec << line;// << '\n';
				  firstLine = false;
			  }
		  }
	  }
	}
	writedec.close();

	//test code
	/*
	 *
	  //file name of the S record to be disassembled
	  ifstream s_record("tutorial1.S68");
	  convert_s_record(s_record);
	  s_record.close();
	 */
}


//int jumpCommand(string command)
long jumpCommand(string command, long address)
{
	//input a command, and output an integer which contains the number
	//of displacement bytes to the next command

	//commands of interest: JMP, BRA, BSR, JSR, RTD, RTR, RTS
	//Bcc, DBcc, BRA and BSR jump to PC + 2 + twos compliment of $displacement
	//JMP and JSR jump to <ea>
	//RTD, RTS, and RTR retrieve the next PC from the (SP)

	//possible commands of interest: Bcc, DBcc, Scc
	//each one may use a different address mode to move the program around
	long displacementBytes = 0;
	string shortCommand = "";

	//extract the short command
	int commandSize = command.length();
	int shortCommandLength = 0;
	for(int i = 0; i < commandSize; i++)
	{
		if(command[i] == ' ' || command[i] == '.')
		{
			break;
		}
		shortCommandLength ++;
	}
	shortCommand = command.substr(0, shortCommandLength);

	//compare the short command to the set of jump instruction commands
	if(shortCommand == "BRA" || shortCommand == "BSR" ||  (shortCommand[0] == 'D' && shortCommand[1] == 'B') || shortCommand == "BCC(HI)" || shortCommand == "BCS(LO)" || shortCommand == "BEQ" || shortCommand == "BGE" || shortCommand == "BGT" || shortCommand == "BHI" || shortCommand == "BLE" || shortCommand == "BLS" || shortCommand == "BLT" || shortCommand == "BMI" || shortCommand == "BNE" || shortCommand == "BPL" || shortCommand == "BVC" || shortCommand == "BVS")
	{
		//Bcc and DBcc also use this displacement mechanism
		//extract the byte displacement from the command

		//collect displacement, a hex value string
		int startPlace = 0;
		int endPlace = 0;
		for(int i = 0; i < commandSize; i++)
		{
			if(command[i] == '$')
			{
				startPlace = i+1;
			}
			else if((command[i] == ' ' || command[i] == '\t') && startPlace != 0)
			{
				endPlace = i;
				break;
			}
		}
		string displacementNumber = command.substr(startPlace, (endPlace - startPlace));

		//cout << "dispHex: " << displacementNumber << '\t';

		//convert the displacement number into an integer
		displacementNumber = hexString_to_decimal(displacementNumber);//output string decimal

		//cout << "dispDec: " << displacementNumber << '\t';

		//convert this integer to binary, invert and add one to get twos compliment, the displacement
		string stringByteHex = decString_to_binary(displacementNumber); //use a string to invert the binary version

		//cout << "bits: " << stringByteHex << '\t';

		//extend to nearest number of bits 8, 16 or 24 or 32
		int sizeOfStringByteHex = stringByteHex.length();
		int lengthOfBits = 0;
		if(sizeOfStringByteHex <= 8)
		{
			lengthOfBits = 8;
		}
		else if(sizeOfStringByteHex <= 16)
		{
			lengthOfBits = 16;
		}
		else if(sizeOfStringByteHex <= 24)
		{
			lengthOfBits = 24;
		}
		else if(sizeOfStringByteHex <= 32)
		{
			lengthOfBits = 32;
		}

		string stringBinary = "";
		for(int i = 0; i < lengthOfBits - sizeOfStringByteHex; i++)
		{
			stringBinary.push_back('0');
		}
		stringBinary.append(stringByteHex);

		int posOrNeg = 1;
		if(stringBinary[0] == '1')//if the first bit is 1, it is a negative number
		{
			posOrNeg = -1;
		}
		//invert bits if the number is negative
		if(posOrNeg == -1)
		{
			int lengthOfStringBinary = stringBinary.length();
			for(int j = 0; j < lengthOfStringBinary; j++)
			{
				if(stringBinary[j] == '0')
				{
					stringBinary[j] = '1';
				}
				else
				{
					stringBinary[j] = '0';
				}
			}
		}

		//cout << "Inverted bits: " << stringBinary << '\t';

		//convert back to int
		displacementNumber = stringBitsToNumber(stringBinary);//output string decimal
		displacementBytes = stringDec_to_int(displacementNumber);//output integer decimal

		//cout << "number: " << displacementNumber << '\t';

		if(posOrNeg == -1)
		{
			//add one to finish the twos compliment
			displacementBytes += 1;
			displacementBytes = displacementBytes*posOrNeg;
		}

		//cout << "(number+1)*-1: " << displacementBytes << '\t';

		//then add 2 to get the byte displacement to the next command
		displacementBytes += 2;
	}
	else if(shortCommand == "JMP" || shortCommand == "JSR")
	{
		string addressInInstruction = extractAddress(command);
		//make sure not to indicate a jump if no hex address is found
		if(addressInInstruction.length() > 0) {
			addressInInstruction = hexString_to_decimal(addressInInstruction);
			long jumpAddress = stringDec_to_int(addressInInstruction);
			displacementBytes = jumpAddress - address;
		}
	}
	//else if(shortCommand == "RTS" || shortCommand == "RTR" || shortCommand == "RTD")
	//{
		//return from subroutine
		//displacementBytes = -1;//flag to go back to stored address
	//}

	return displacementBytes;
}


string extractAddress(string original)
{
	//extracts an address as indicated by a hex preceded by '($' and terminated by ')'
	string address = "";
	int originalStringLength = original.length();
	int addrStartPosition = -1;
	//loop through the string, search for "($" and ended by ')'
	for(int i = 0; i < originalStringLength; i++)
	{
		if(original[i] == '(' && original[i + 1] == '$')
		{
			addrStartPosition = i + 1;
		}

		if(addrStartPosition >= 0 && original[i] != ')' && original[i] != '(')
		{
			address.push_back(original[i]);
		}
		else if(original[i] == ')')
		{
			break;//end the for loop, we've collected the address
		}
	}
	return address;

	/*
	string phrase = "0x00045c:	MOVE.W ($fffa04).L,D0	; source -> destination	1116 + 6	001100000011100100000000111111111111101000000100";
	string addr = extractAddress(phrase);

	cout << addr << '\n';
	 */
}

bool numberIsPresentInDataAddresses(long number) {
	//searches the possible data addresses to file for the input number, returns true if present
	bool isPresent = false;
	string line = "";

	ifstream myfile("DataAddresses.txt");

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

long long convertAddress(string assembly) {
	//input some assembly code, extract an address which is not part of a branch or LEA routine
	//outputs the address as a decimal
	string effAddr = extractAddress(assembly);
	int lengthEA = effAddr.length();
	string tempEA = "";
	for(int i = 1; i < lengthEA; i++) {
		tempEA.push_back(effAddr[i]);
		if(effAddr[i] == ',') {
			tempEA = "";
			break;
		}
	}
	//cout << tempEA << '\n';
	string shortCommand = "";

	//extract the short command
	int commandSize = assembly.length();
	int shortCommandLength = 0;
	for(int i = 0; i < commandSize; i++)
	{
		if(assembly[i] == ' ' || assembly[i] == '.')
		{
			break;
		}
		shortCommandLength ++;
	}
	shortCommand = assembly.substr(0, shortCommandLength);

	long long dataAddressDecimal = 0;
	//if the short command is a jump, branch or load address, it may not be data
	if(shortCommand != "LEA" && shortCommand != "JSR" && shortCommand != "JMP") {
		tempEA = hexString_to_decimal(tempEA);
		dataAddressDecimal = stringDec_to_int(tempEA);
	}
	return dataAddressDecimal;
}

void numberIsSmallerInDataAddresses(long number) {
	//searches the data address file and erases any numbers which are smaller than the input number
	string line = "";

	ifstream myfile("DataAddresses.txt");
	ofstream temp("temp1.txt", ios::trunc);

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
	remove("DataAddresses.txt");
	rename("temp1.txt","DataAddresses.txt");
}
