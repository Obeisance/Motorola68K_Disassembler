//stringAndNumberHandlers.h
#include <string>
#include <stdlib.h>
using namespace std;


int dec_to_binary(int byteVal);
//input an integer and output a pseudo binary number

string decString_to_binary(string decimal);
//converts a decimal number in a string to a binary number in a string

string stringBitsToNumber(string bits);
//input a string of bits(base 2), output a number(base10) in a string

int stringBitsToNumberInt(string bits);
//input a string of bits(base 2), output a number(base10)

string stringNumber_to_hex(string number);
//input a base10 number in a string, output a string with the equivalent hex number

string hexString_to_decimal(string hex);
//input a string value hex value, convert to decimal string

long long stringDec_to_int(string number);
//input an integer in a string, convert to a number

string makeAddress(long addressNumber, int addressLength);
//input a decimal address, split it into strings of bytes separated by tab values
//addressNumber is the address to be converted into addressLength number of bytes

string decAddr_to_hexAddr(long addr);
//input an address decimal, output a string address in hex

string binaryLLong_to_string(long long binaryInt);
//input a long long binary int and convert to string

string binaryString_to_hexString(string binary);
//input a binary string, output a hex string

string binaryString_to_charString(string binary);
//input a binary string, output a string converted to char

bool stringSearch(string searchTerm, string searchString);
//looks for searchTerm in searchString- returns true if found
