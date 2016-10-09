//============================================================================
// Name        : AssemblyLabeler.cpp
// Author      : Obeisance
// Version     :
// Copyright   : Free to use for all
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

string replaceString(string original, string searchTerm, string replaceTerm);
//searches original for searchTerm, if found will replace with replaceTerm
//otherwise, returns original

string extractAddress(string original, string &remaining);
//extracts an address as indicated by a hex preceded by '($' and terminated by ')'
//returns the rest of the string beyond the identified address

string reLabel(string labelFileName, string originalLine);

int main() {

	string inputString = "";
	cout << "-----------------------------------------------" << '\n';
	cout << "----Welcome to the Daft Motorola 68k Labeler---" << '\n';
	cout << "-----------------------------------------------" << '\n' << '\n';
	cout << "  This program reads in an assembly code file  " << '\n';
	cout << "   as prepared by the daft disassembler and    " << '\n';
	cout << "  looks for addresses in the instructions as   " << '\n';
	cout << "denoted by a ($----) format. When it finds an  " << '\n';
	cout << "address, it will label this address according  " << '\n';
	cout << "to text supplied in an accompanying file. This " << '\n';
	cout << "   label file must be formatted with an '_'    " << '\n';
	cout << "between the hex addresses and the labeling text" << '\n' << '\n';

	cout << "Input name of file to be labeled:" << '\n';
	cout << ">>:";
	getline(cin,inputString);
	string fileWithLabels = "";
	cout << "Input name of file which contains hex addresses" << '\n';
	cout << "            and accompanying labels:" << '\n';
	cout << ">>:";
	string addressFileName = "";
	getline(cin,fileWithLabels);
	string outputFile = "";
	cout << "Input name of file to save to:" << '\n';
	cout << ">>:";
	getline(cin,outputFile);


	ifstream myfile(inputString.c_str());
	ofstream reWrite(outputFile.c_str(), ios::trunc);
	//ofstream troubleshoot("log.txt", ios::trunc);

	//next, open the file and read in a line, preparing to loop through the entire file while doing this
	//also open a file which can have the output pushed into

	string line = "";
	string replacementString = "";

	if (myfile.is_open())
	{
		cout << "Labeling, please wait...";
		while (getline (myfile,line))
		{
			//bool matchFound = false;

			/*ifstream labelFile(fileWithLabels.c_str());//open the label file
			string labelline = "";
			string addressInLine = "";*/
			if(line[0] != '\t')//make sure we're not on a comment line
			{
				line = reLabel(fileWithLabels, line);
				/*
				//search the line for an address
				addressInLine = extractAddress(line);
				int sizeOfAddress = addressInLine.length();

				//if an address is found, loop through the list of interesting addresses to search for a match
				//reWrite << addressInLine << '\n';
				if(sizeOfAddress > 0)
				{

					//loop through the entire label file to
					//check and see if we have a label for this address
					while(getline (labelFile,labelline))
					{
						//read the address from the label file
						//search the line for an address
						int sizeOflabelline = labelline.length();
						string addressInlabel = "";// labelline.substr(1,6);
						int labelStart = 0;
						for(int i = 1; i < sizeOflabelline; i++)
						{
							if(labelline[i] == '_')
							{
								labelStart = i+1;
								break;
							}
							addressInlabel.push_back(labelline[i]);
						}
						string lineAddress = addressInLine.substr(1,addressInlabel.length());
						//troubleshoot << labelline << ", " << addressInlabel << ", " << lineAddress << ", " << addressInLine << '\n';
						matchFound = true;
						//if the two addresses are not the same lenght, we don't have a match
						if(lineAddress.length() != addressInlabel.length())
						{
							matchFound = false;
						}
						else //otherwise, check that the characters match
						{
							for(int j = 0; j < lineAddress.length(); j++)
							{
								if(lineAddress[j] != addressInlabel[j])
								{
									matchFound = false;
								}
							}
						}

						if(matchFound == true)
						{
							//create the replacement string
							string labelString = "";
							for(int i = labelStart; i < sizeOflabelline; i++)
							{
								labelString.push_back(labelline[i]);
							}

							replacementString = addressInLine.substr(0,7);
							replacementString.push_back('_');
							replacementString.append(labelString);
							//reWrite << replaceString << '\n';
							//matchFound = true;
							break;
						}

					}
				}*/
			}

			//output the updated string into the output file
			reWrite << line << '\n';

			/*
			//if a match is found, replace the original address in the string with the identifier
			if(matchFound == true)
			{
				//cout <<addressInLine << '\t' << replaceString << '\n';
				line = replaceString(line, addressInLine, replacementString);

			}*/
			//labelFile.close();
		}

	}

	cout << "finished" << '\n';

	myfile.close();
	//troubleshoot.close();
	reWrite.close();
	return 0;
}


string replaceString(string original, string searchTerm, string replaceTerm)
{
	//searches original for searchTerm, if found will replace with replaceTerm
	//otherwise, returns original
	string editedString = "";

	int stringLength = original.length();
	int searchTermLength = searchTerm.length();

	//cout << "looking for: " << searchTerm << ", size: " << searchTermLength << '\n';

	//loop through the string, searching for "searchTerm"
	int searchIndex = 0;
	int matchingChars = 0;
	int matchPosition = 0;
	bool matchFound = false;
	for(int i = 0; i < stringLength; i++)
	{
		if(original[i] == searchTerm[searchIndex])
		{
			//one character of the string matches our search
			matchingChars += 1;
			searchIndex += 1;//look to see if the next character matches

			//cout << "matched chars: " << matchingChars << '\n';
		}
		else
		{
			searchIndex = 0;
			matchingChars = 0;
		}

		//if we make our first match, then mark the position
		if(matchingChars == 1)
		{
			matchPosition = i;
			//cout << "match found at position: " << matchPosition << '\n';
			//cout << "match begins with: " << original[i] << '\n';
		}

		if(matchingChars == searchTermLength)
		{
			matchFound = true;
			break;//end the for loop
		}
	}

	//if we have a match, then break up the original string and add in the replacement string instead of the original
	if(matchFound == true)
	{
		editedString = original.substr(0, matchPosition);

		//cout << "beginning of orig.: " << editedString << '\n';

		editedString.append(replaceTerm);

		//cout << "add in replaceTerm: " << editedString << '\n';


		//make sure to search the remaining part of the string for the searchTerm
		int positionEndOfSearchTermInOriginalString = matchPosition + searchTermLength;
		string restOfString = original.substr(positionEndOfSearchTermInOriginalString, stringLength - positionEndOfSearchTermInOriginalString);
		string searchRestOfString = replaceString(restOfString, searchTerm, replaceTerm);
		editedString.append(searchRestOfString);
	}
	else
	{
		//if we have no match, output the original string
		editedString = original;
	}
	return editedString;

	/*
	string phrase = "0x00045c:	MOVE.W ($fffa04).L,D0	; source -> destination	1116 + 6	001100000011100100000000111111111111101000000100";
	string phraseSearch = replaceString(phrase, "$fffa04", "$fffa04_clock_synthesizer_control_register_SYNCR");

	cout << phrase << '\n';
	cout << phraseSearch << '\n';
	 */
}

string extractAddress(string original, string &remaining)
{
	//extracts an address as indicated by a hex preceded by '($' and terminated by ')'
	string address = "";
	remaining = "";
	int originalStringLength = original.length();
	int addrStartPosition = -1;
	//loop through the string, search for "($" and ended by ')'
	for(int i = 0; i < originalStringLength; i++)
	{
		if(original[i] == '(' && original[i + 1] == '$')
		{
			addrStartPosition = i + 1;
		}

		if(addrStartPosition >= 0 && original[i] != ')' && original[i] != '(' && original[i] != '_' && original[i] != ',')
		{
			address.push_back(original[i]);
		}
		else if(addrStartPosition >= 0 && original[i] == ')')
		{
			remaining = original.substr(i,original.length()-i);
			break;//end the for loop, we've collected the address
		}
		else if((original[i] == '_' || original[i] == ',') && addrStartPosition >= 0)
		{
			address = "";
			addrStartPosition = -1;
		}
	}
	return address;

	/*
	string phrase = "0x00045c:	MOVE.W ($fffa04).L,D0	; source -> destination	1116 + 6	001100000011100100000000111111111111101000000100";
	string addr = extractAddress(phrase);

	cout << addr << '\n';
	 */
}

string reLabel(string labelFileName, string originalLine)
{
	string originalLineRelabeled = originalLine;//store the string to be output by the function after labelling
	string labelline = "";//store the line from the file of labels
	ifstream labelFile(labelFileName.c_str());//load the file of labels
	bool matchFound = false;
	string replacementString = "";//save the string to replace the identified address from original line


	//search the line for an address
	string notCheckedForLabel = "";//store the part of the original line which has not been checked
	string addressInLine = extractAddress(originalLine,notCheckedForLabel);
	int sizeOfAddress = addressInLine.length();
	int sizeOfNotCheckedForLabel = notCheckedForLabel.length();//=0 if no address found

	//if an address is found, loop through the list of interesting addresses to search for a match
	//reWrite << addressInLine << '\n';
	if(sizeOfAddress > 0)
	{

		//loop through the entire label file to
		//check and see if we have a label for this address
		while(getline (labelFile,labelline))
		{
			//read the address from the label file
			//search the line for an address
			int sizeOflabelline = labelline.length();
			string addressInlabel = "";// labelline.substr(1,6);
			int labelStart = 0;
			for(int i = 1; i < sizeOflabelline; i++)
			{
				if(labelline[i] == '_')
				{
					labelStart = i+1;
					break;
				}
				addressInlabel.push_back(labelline[i]);
			}
			string lineAddress = addressInLine.substr(1,addressInlabel.length());
			//troubleshoot << labelline << ", " << addressInlabel << ", " << lineAddress << ", " << addressInLine << '\n';
			matchFound = true;
			//if the two addresses are not the same length, we don't have a match
			if(lineAddress.length() != addressInlabel.length())
			{
				matchFound = false;
			}
			else //otherwise, check that the characters match
			{
				for(int j = 0; j < lineAddress.length(); j++)
				{
					if(lineAddress[j] != addressInlabel[j])
					{
						matchFound = false;
					}
				}
			}

			if(matchFound == true)
			{
				//create the replacement string
				string labelString = "";
				for(int i = labelStart; i < sizeOflabelline; i++)
				{
					labelString.push_back(labelline[i]);
				}

				replacementString = addressInLine.substr(0,7);
				replacementString.push_back('_');
				replacementString.append(labelString);
				//reWrite << replaceString << '\n';
				//matchFound = true;
				break;
			}

		}
	}
	labelFile.close();

	//if a match is found, replace the original address in the string with the identifier
	if(matchFound == true)
	{
		//cout <<addressInLine << '\t' << replaceString << '\n';
		originalLineRelabeled = replaceString(originalLine, addressInLine, replacementString);
	}

	if(sizeOfNotCheckedForLabel > 0)
	{
		//check for a label in the latter part of the line
		string latterPartLabeled = reLabel(labelFileName, notCheckedForLabel);
		originalLineRelabeled = replaceString(originalLineRelabeled, notCheckedForLabel, latterPartLabeled);
	}

	return originalLineRelabeled;
}
