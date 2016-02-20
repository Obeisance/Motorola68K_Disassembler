//stringAndNumberHandlers.cpp
#include <string>
#include <stdlib.h>
#include "stringAndNumberHandlers.h"
using namespace std;


//input an integer and output a pseudo binary number
int dec_to_binary(int byteVal)
{
	int binary = 0;
	int mod = byteVal;
	int divide = byteVal;
	int subtract = byteVal;
	int power = 1;

	if(byteVal > 255)
	{
		return -1;
	}

	for(int i = 0; i < 8; i++)
	{
		mod = divide % 2;
		binary += power*mod;
		power = power*10;
		subtract = divide - mod;
		divide = subtract/2;
		//cout << "mod: " << mod <<" binary: " << binary << " power: " << power << " subtract: " << subtract << " divide: " << divide << '\n';
	}
  return binary;
}

string decString_to_binary(string decimal)
{
	//converts a decimal number in a string to a binary number in a string
	string binaryNumber = "";

	//convert the number to an integer
	long long decimalNumber = stringDec_to_int(decimal);

	//cout << "decNumber: " << decimalNumber << '\n';

	//convert the integer to binary
	string binaryNumberShort = "";
	unsigned long mod = decimalNumber;
	unsigned long divide = decimalNumber;
	unsigned long subtract = decimalNumber;
	int bitNumber = 8;
	if(decimalNumber > 16777215)
	{
		bitNumber = 32;
	}
	else if(decimalNumber > 65535)
	{
		bitNumber = 24;
	}
	else if(decimalNumber > 255)
	{
		bitNumber = 16;
	}

	while(divide > 0)
	{
		mod = divide % 2;
		if(mod == 1)
		{
			string temp = binaryNumberShort;
			binaryNumberShort = "";
			binaryNumberShort.push_back('1');
			binaryNumberShort.append(temp);
		}
		else
		{
			string temp = binaryNumberShort;
			binaryNumberShort = "";
			binaryNumberShort.push_back('0');
			binaryNumberShort.append(temp);
		}
		subtract = divide - mod;
		divide = subtract/2;
		//cout << "mod: " << mod <<" binary: " << binaryNumberShort << " subtract: " << subtract << " divide: " << divide << '\n';
	}

	//add extra zeros to make 8 bits
	int binaryNumberLength = binaryNumberShort.length();
	for(int a = 0; a < bitNumber - binaryNumberLength; a++)
	{
		binaryNumber.push_back('0');
	}
	binaryNumber.append(binaryNumberShort);

	return binaryNumber;
}

string stringBitsToNumber(string bits)
{
	//input a string of bits (base2), output a number (base10) in a string
	string outputString = "";
	int sizeBits = bits.length();
	int multiplier = 1;
	int number = 0; //add 48 to get the char
	for(int i = (sizeBits-1); i >= 0; i--)
	{
		if(bits[i] == '1')
		{
			number += multiplier;
		}
		multiplier = multiplier*2;
	}
	char stringByte [17];
	outputString = itoa(number,stringByte,10);

	return outputString;
}

int stringBitsToNumberInt(string bits)
{
	//input a string of bits (base2), output a number (base10)
	int sizeBits = bits.length();
	int multiplier = 1;
	int number = 0; //add 48 to get the char
	for(int i = (sizeBits-1); i >= 0; i--)
	{
		if(bits[i] == '1')
		{
			number += multiplier;
		}
		multiplier = multiplier*2;
	}

	return number;
}

string stringNumber_to_hex(string number)
{
	//input a base10 number in a string, output a string with the equivalent hex number
	string hexNumber = "";

    size_t len = number.length();
    char stringByte [2*len];//temporarily stores the output number
    int byteValue = 0;//temporarily store the number in the string


    //loop through the number's characters, convert to integer
    for (size_t i = 0; i < len; ++i)
    {
        const char a = number[i] - 48;
        if (number[i] <= 57 && number[i] >= 48)//don't convert symbols, punctuation or letters
        {
        	int fragment = a;
        	byteValue = byteValue*10 + fragment;
        }
    }
	//convert to hex
	hexNumber = itoa(byteValue,stringByte,16);

	return hexNumber;
}

string hexString_to_decimal(string hex)
{
	//input a string value hex value, convert to decimal string
	string decimal = "";
	int hexConvert[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	int hexConvertlower[16] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

	//take the hex number and get an int in decimal
	int stringLength = hex.length();
	long long number = 0;
	long long multiplier = 1;
	for(int a = stringLength - 1; a >= 0 ; a--)
	{
		for(int b = 0; b < 16; b++)
		{
			if(hex[a] == hexConvert[b] || hex[a] == hexConvertlower[b])
			{
				number += multiplier*b;
			}
		}
		multiplier = multiplier*16;
		//cout << "number: " << number << '\n';
	}

	//cout << "number: " << number << '\n';

	//take the decimal number and convert it to a string
	int temp = 0;
	long long divide = number;
	while(divide > 0)
	{
		long long preDivide = divide;
		divide = divide/10;
		temp = preDivide - divide*10;
		string tempString = decimal;
		decimal = "";
		decimal.push_back(hexConvert[temp]);
		decimal.append(tempString);
	}

	if(number == 0)
	{
		decimal = '0';
	}

	//cout << "hex: " << hex << ", number " << number << ", decimal "<< decimal << '\n';
	return decimal;
}

long long stringDec_to_int(string number)
{
	//input an integer in a string, convert to a number
	int size = number.length();
	long long integer = 0;
	long long multiple = 1;
	for(int i = size - 1; i >= 0; i--)
	{
		integer += multiple*(number[i] - 48);
		multiple = multiple*10;
	}
	return integer;
}

string makeAddress(long addressNumber, int addressLength)
{
	//input a decimal address, split it into strings of bytes separated by tab values
	//addressNumber is the address to be converted into addressLength number of bytes
	string addressString = "";
	int address[addressLength];
	unsigned long mod = addressNumber;
	unsigned long divide = addressNumber;
	unsigned long subtract = addressNumber;
	for(int i = addressLength-1; i >= 0; i--)
	{
		mod = divide % 256;
		address[i] = mod;
		subtract = divide - mod;
	    divide = subtract/256;
	}
	for(int j = 0; j < addressLength; j++)
	{
	    char stringByte [16];
	    string decimal = itoa(address[j],stringByte,10);
		addressString.append(decimal);
		addressString.push_back('\t');
	}

	return addressString;
}

string decAddr_to_hexAddr(long addr)
{
	//input an address decimal, output a string address in hex
	  char stringByte [14];
	  string addressHex = itoa(addr,stringByte,16);
	  string hexAddress = "000000";
	  int addressLength = addressHex.length();
	  for(int i = 0; i < addressLength; i++)
	  {
		  hexAddress[i + 6 - addressLength] = addressHex[i];
	  }
	  string hexBegin = "0x";
	  hexBegin.append(hexAddress);
	  return hexBegin;
}

string binaryLLong_to_string(long long binaryInt)
{
	//input a long long binary int and convert to string
	string binaryString = "";
	string temp = "";

	//split the int into 4 parts of 8 bits each
	long long firstHalf = binaryInt/10000000000000000;
	long long secondHalf = binaryInt - (firstHalf*10000000000000000);

	//cout << "firstHalf: " << firstHalf << " secondHalf: " << secondHalf << '\n';

	long firstHalfOfFirstHalf = firstHalf/100000000;
	long secondHalfOfFirstHalf = firstHalf - (firstHalfOfFirstHalf*100000000);

	long firstHalfOfSecondHalf = secondHalf/100000000;
	long secondHalfOfSecondHalf = secondHalf - (firstHalfOfSecondHalf*100000000);

	//cout << "1: " << firstHalfOfFirstHalf << " 2: " << secondHalfOfFirstHalf << " 3: " << firstHalfOfSecondHalf << " 4: " << secondHalfOfSecondHalf << '\n';

	char stringByte [32];
	if(firstHalfOfFirstHalf > 0)
	{
		temp = itoa(firstHalfOfFirstHalf,stringByte,10);
		binaryString.append(temp);
	}
	if(secondHalfOfFirstHalf > 0)
	{
		temp = itoa(secondHalfOfFirstHalf,stringByte,10);
		binaryString.append(temp);
	}
	if(firstHalfOfSecondHalf > 0)
	{
		temp = itoa(firstHalfOfSecondHalf,stringByte,10);
		binaryString.append(temp);
	}
	if(secondHalfOfSecondHalf > 0)
	{
		temp = itoa(secondHalfOfSecondHalf,stringByte,10);
		binaryString.append(temp);
	}

	return binaryString;
}


string binaryString_to_hexString(string binary)
{
	//input a binary string, output a hex string

	//make sure that the binary string is composed of multiples of 4, else extend
	//it to the nearest group of 4

	int sizeOfBinary = binary.length();
	int binaryMod = sizeOfBinary % 4;
	string fixedBinaryString = "";
	if (binaryMod != 0)
	{
		int needExtraZeros = 4 - binaryMod;
		for(int i = 0; i < needExtraZeros; i++)
		{
			fixedBinaryString.push_back('0');
		}
	}
	fixedBinaryString.append(binary);
	//now we have ensured that the string has an appropriate length

	//next, split into groups of 4 bits and interpret as hex
	int numberOfGroups = (fixedBinaryString.length())/4;
	char hexConvert[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	string hexString = "";
	for(int j = 0; j < numberOfGroups; j++)
	{
		string tempString = fixedBinaryString.substr(j*4,4);
		int index = stringBitsToNumberInt(tempString);
		hexString.push_back(hexConvert[index]);
	}
	return hexString;
}

string binaryString_to_charString(string binary)
{
	//input a binary string, output a string converted to char

	//make sure that the binary string is composed of multiples of 8, else extend
	//it to the nearest group of 8

	int sizeOfBinary = binary.length();
	int binaryMod = sizeOfBinary % 8;
	string fixedBinaryString = "";
	if (binaryMod != 0)
	{
		int needExtraZeros = 8 - binaryMod;
		for(int i = 0; i < needExtraZeros; i++)
		{
			fixedBinaryString.push_back('0');
		}
	}
	fixedBinaryString.append(binary);
	//now we have ensured that the string has an appropriate length

	//next, split into groups of 8 bits and interpret as a char
	int numberOfGroups = (fixedBinaryString.length())/8;
	string charString = "";
	for(int j = 0; j < numberOfGroups; j++)
	{
		string tempString = fixedBinaryString.substr(j*8,8);
		int index = stringBitsToNumberInt(tempString);
		if(index > 31) {
			//don't copy the formatting characters over -_-
			charString.push_back(index);//necessarily interprets index as char
		}
	}
	return charString;

	/*
	 * 	string test = binaryString_to_charString("0100100001100101011011000110110001101111001000000101011101101111011100100110110001100100");
	cout << test << '\n';
	 */
}

bool stringSearch(string searchTerm, string searchString)
{
	//looks for searchTerm in searchString- returns true if found
	int sizeOfSearchString = searchString.length();
	int sizeOfSearchTerm = searchTerm.length();
	bool isPresent = false;
	for(int i = 0; i < sizeOfSearchString - sizeOfSearchTerm; i++)
	{
		string tempString = searchString.substr(i,sizeOfSearchTerm);
		if(tempString == searchTerm) {
			isPresent = true;
			break;
		}
	}
	return isPresent;
}
