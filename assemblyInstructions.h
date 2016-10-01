//assemblyInstructions.h
#include <string>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "stringAndNumberHandlers.h"
using namespace std;

int constructInstructionString(string commandArrayString, string commandString, string &outputString);
//input a string with the short form of a command which indicates the correct form
//of the commandString; outputs a changed outputString with the written
//version of the command as well as an int which indicates the total number
//of instruction bytes

string retrieveBitsFromInstruction(int bitPlace, int numBits, string commandString);
//return a string of the numBits number of bits at bitPlace (defined
//in commandString relative to bit assignment
//(15------87------0)('-1'------'-8''-9'------'-16');

int assembleEffectiveAddress(string commandString, string &effectiveAddress, string EAmode, string EAregi, char operationSize, int lastBitInCommand);
//input the bytes of a commandString and a reference string which will be
//changed to the appropriate effective address syntax. Also include char B, W,
// or L to indicate the operation size. Output an int
//which indicates the number of extension bytes beyond the command
//or -1 if the format is incorrect

int assembleEffectiveAddress(string commandString, string &effectiveAddress, char operationSize);
//same as above, but with EAstart == bit 5 (last 6 bits = EA), last bit in command = bit 0 of a 16 bit word.

int assembleEffectiveAddress(string commandString, string &effectiveAddress, int EAstart, char operationSize, int lastBitInCommand);
//same as above, with 6 contiguous bits = EA

int guessCommand(string commandString, string &outputString, bool &multipleMatch);
//input a set of bits, output a string formatted as an assembly instruction;
//if multiple matches are found, multipleMatch is changed to true
//otherwise it is set false

string decimalVersionReader(long address, long &lastFilePosition);
//input address, return value at address or "NAN" if address not allowed
//adjusts the lastFilePosition parameter to allow for fast seeking

string decimalVersionReader(long address);
//input address, return value at address or "NAN" if address not allowed
//always seeks from beginning

long decimalVersionFirstAddress();
//outputs the first address in the decimalVersion file

long decimalVersionLastAddress();
//outputs the last address in the decimalVersion file

string addToJumpAddresses(string jumpAddresses, long addressToAdd);
//checks to see if addressToAdd is already present in the
//jumpAddresses string, if it is not, add it into the string
//in numerical order

string removeLowerJumpAddresses(string jumpAddresses, long addressThreshold);
//removes from jumpAddresses the addresses below addressThreshold

string removeHigherJumpAddresses(string jumpAddresses, long addressThreshold);
//removes from jumpAddresses the addresses above addressThreshold

bool numberIsPresentInJumpAddresses(string jumpAddresses, long address);
//returns true if address is present in jumpAddresses

bool numberIsPresentInJumpAddresses(long number);
//searches the jump or branch to file for the input number, returns true if present

void numberIsSmallerInJumpAddresses(long number);
//searches the jump or branch file and erases any numbers which are smaller than the input number

bool  writeVBR_table(string assemblyLine, long currentAddress, string &jumpAddresses);
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

