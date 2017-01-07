//============================================================================
// Name        : TransferNotes.cpp
// Author      : Obeisance
// Version     :
// Copyright   : Free to use for all
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

void fileIntegrator();
//integrates the notated file with the fixed tagged file
//copies into a new file the lines which begin with '/t'

int main() {

	fileIntegrator();

	return 0;
}

void fileIntegrator()
{
	//integrates the notated file with the fixed tagged file
	//copies into a new file the lines which begin with '/t'

	string annotatedFile = "";
	cout << "-----------------------------------------------" << '\n';
	cout << "----Welcome to the Daft Note Transfer Tool-----" << '\n';
	cout << "-----------------------------------------------" << '\n' << '\n';
	cout << "  This program integrates the notes from one   " << '\n';
	cout << "   file with the data from another file. Line  " << '\n';
	cout << "  by line data from the data file is written   " << '\n';
	cout << " to the merged file until a line starting with " << '\n';
	cout << "  a tab is found in the annotated file. Then,  " << '\n';
	cout << "   the tab marked note is transferred to the   " << '\n';
	cout << "  merged file. Some work is done to maintain   " << '\n';
	cout << "      the hex address near the note.           " << '\n' << '\n';

	cout << "Input name of the annotated file:" << '\n';
	cout << ">>:";
	getline(cin,annotatedFile);
	string dataFile = "";
	cout << "Input name of file which contains data:" << '\n';
	cout << ">>:";
	getline(cin,dataFile);
	string outputFile = "";
	cout << "Input name of file to save to:" << '\n';
	cout << ">>:";
	getline(cin,outputFile);

	//ifstream oldfile("taggedAssemblyVersion - try 2.txt");
	//ifstream newfile("taggedAssemblyVersion.txt");

	//ifstream oldfile("2016-10-30 ECU code from 6-6-06 disassembly and comment.txt");
	//ifstream newfile("labeledFile.txt");

	//ofstream mergedFile("combinedAssemblyFile.txt", ios::trunc);

	ifstream oldfile(annotatedFile.c_str());
	ifstream newfile(dataFile.c_str());
	ofstream mergedFile(outputFile.c_str(), ios::trunc);

	string oldLine = "";
	string oldAdd1 = "";
	string oldLine2 = "";
	string oldAdd2 = "";
	string oldLine3 = "";
	string oldAdd3 = "";
	string oldLine4 = "";
	string oldAdd4 = "";
	string oldLine5 = "";
	string oldAdd5 = "";

	string newLine = "";
	string newAdd1 = "";
	string newLine2 = "";
	string newAdd2 = "";
	string newLine3 = "";
	string newAdd3 = "";
	string newLine4 = "";
	string newAdd4 = "";
	string newLine5 = "";
	string newAdd5 = "";

	string lastOldLine = "";
	string lastOldAddr = "";
	string lastNewAddr = "";

	bool sameAddr = true;
	bool oldAddrLow = false;
	bool newLineWritten = false;

	//long lineCount = 0;

	if (oldfile.is_open())
	{
	  while (getline(oldfile,oldLine))
	  {
		  /*
		  getline(oldfile,oldLine2);
		  getline(oldfile,oldLine3);
		  getline(oldfile,oldLine4);
		  getline(oldfile,oldLine5);

		  getline(newfile,newLine);
		  getline(newfile,newLine2);
		  getline(newfile,newLine3);
		  getline(newfile,newLine4);
		  getline(newfile,newLine5);*/


		  if(oldLine[0] == '0')
		  {
			  //figure out which address it is
			  oldAdd1 = oldLine.substr(0,8);
			  if(oldAddrLow == false)
			  {
				  getline(newfile,newLine);
				  newAdd1 = newLine.substr(0,8);
				  newLineWritten = false;
			  }

			  sameAddr = true;
			  for(int i = 0; i < 8; i++)
			  {
				  if(newAdd1[i] != oldAdd1[i])
				  {
					  if(newAdd1[i] < oldAdd1[i])
					  {
						  oldAddrLow = false;
					  }
					  else
					  {
						  oldAddrLow = true;
					  }
					  sameAddr = false;
					  break;
				  }
			  }

			  //add the new line if it is the same
			  //or a lower address than that in
			  //the original file
			  if((sameAddr || !oldAddrLow) && !newLineWritten)
			  {
				  mergedFile << newLine << '\n';
				  newLineWritten = true;
			  }


			  lastOldLine = oldLine;
			  lastOldAddr = oldAdd1;
			  lastNewAddr = newAdd1;
		  }
		  else
		  {
			  //the line is a comment, so we must make sure it
			  //is being put in the correct place

			  if(sameAddr == true)
			  {
				  //the previous lines had the same address
				  //so output the comment to the file
				  //because it is in the correct location
				  mergedFile << oldLine << '\n';
			  }
			  else
			  {
				  //we're not in the correct place for the comment
				  //read in the next line from the new file
				  //and compare the addresses
				  if(oldAddrLow == false)
				  {
					  while(sameAddr == false && oldAddrLow == false)
					  {
						  getline(newfile,newLine);
						  newAdd1 = newLine.substr(0,8);

						  sameAddr = true;
						  for(int i = 0; i < 8; i++)
						  {
							  if(newAdd1[i] > lastOldAddr[i])
							  {
								  cout << "oldAddrLow" << lastOldAddr << ' ' << newAdd1 << '\n';
								  oldAddrLow = true;
								  sameAddr = false;
								  break;
							  }
							  else if(newAdd1[i] != lastOldAddr[i])
							  {
								  cout << "oldAddrHigh" << lastOldAddr << ' ' << newAdd1 << '\n';
								  sameAddr = false;
								  break;
							  }
						  }
						  //add the new line
						  if(oldAddrLow == false)
						  {
							  mergedFile << newLine << '\n';
						  }
					  }
					  mergedFile << oldLine << '\n';
				  }
				  else if(oldAddrLow == true)
				  {
					  //if the old address is lower than the new
					  //address, just write in the old line comment
					  mergedFile << oldLine << '\n';
					  cout << "oldAddrLow when writing comment" << '\n';
					  break;
				  }
			  }
		  }
		  /*if(oldLine2[0] == '0')
		  {
			  //figure out which address it is
			  oldAdd2 = oldLine2.substr(0,8);
		  }
		  if(oldLine3[0] == '0')
		  {
			  //figure out which address it is
			  oldAdd3 = oldLine3.substr(0,8);
		  }
		  if(oldLine4[0] == '0')
		  {
			  //figure out which address it is
			  oldAdd4 = oldLine4.substr(0,8);
		  }
		  if(oldLine5[0] == '0')
		  {
			  //figure out which address it is
			  oldAdd5 = oldLine5.substr(0,8);
		  }




		  if(oldLine[0] == '0')
		  {

			  mergedFile << newLine << '\n';
		  }
		  else
		  {
			  mergedFile << oldLine << '\n';
		  }*/


	  }
	}

	oldfile.close();
	newfile.close();
	mergedFile.close();
}
