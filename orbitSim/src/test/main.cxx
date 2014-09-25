#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>

#include "orbitSim/functions.h"
#include "orbitSim/OrbSim.h"


std::ifstream refFile;
std::ifstream compareFile;
std::ofstream testOut;


using namespace std;
bool FileCheck(const std::string& filename);
bool CompareOutput(std::string refString, std::string compareString, int lineNumber, ofstream &testOut, int * statearray);
string DiffOut(std::string refString, std::string compareString);

// Main test executable.  Compares output files to reference files line by lin
int main () {
  std::string refline;
  std::string compline;

  testOut.open("gtorbsimTest.out", ios::app);

  if(FileCheck("TakoAttitude.out") == 1){
		if(FileCheck("TakoAttRef") != 1){
		  std::cout << "No TakoAttRef Found!  Exiting!";
			return 0;
		}
		int i=1;
		//int linenum = 700;
		int linestate[700];
		refFile.open ("TakoAttRef", ios::out);
		compareFile.open ("TakoAttitude.out", ios::out);
		cout << "Checking TakoAttitude.out" << "\n";
		while((std::getline(refFile,refline) && std::getline(compareFile,compline)))
		{
		  if(CompareOutput(refline,compline,i,testOut,linestate)==1){
		    //cout << "Difference found.  Line number: " << i << "\n";
		  }
			i++;
		}
  } else if(FileCheck("ASFLAttitude.out") == 1){
		if(FileCheck("ASFLAttRef") != 1){
		  std::cout << "No ASFLAttRef Found!  Exiting!";
			return 0;
		}
		int i=1;
		//int linenum = 700;
		int linestate[700];
		refFile.open ("ASFLAttRef", ios::out);
		compareFile.open ("ASFLAttitude.out", ios::out);
		cout << "Checking ASFLAttitude.out" << "\n";
		while((std::getline(refFile,refline) && std::getline(compareFile,compline)))
		{
		  CompareOutput(refline,compline,i,testOut,linestate);
                  if(CompareOutput(refline,compline,i,testOut,linestate)==1){
                    //cout << "Difference found.  Line number: " << i << "\n";
                  }
			i++;
		}
	}
	refFile.close();
	compareFile.close();
	testOut.close();
	cout << "Test Completed." << "\n";
	return 0;
}

bool FileCheck(const std::string& filename) {
	ifstream ifile(filename.c_str());
	return ifile;
}

bool CompareOutput(std::string refString, std::string compareString, int lineNumber, ofstream &testOut, int * statearray)
{
        //testOut.open("gtorbsimTest.out", ios::in | ios::app);
	bool status = 0;
	if (refString.compare(compareString) != 0) {
		//testOut << "Compare Failure!  Line number: " << lineNumber << "\n";
		cout << "Difference Found.  Line number: " << lineNumber << "\n";
		statearray[lineNumber] = 1;
		//testOut << DiffOut(refString, compareString);
		status = 1;
	} else {
		statearray[lineNumber] = 0;
		//testOut << "No differences found.  Line number: " << lineNumber << "\n";
	}	
	return status;
}

string DiffOut(std::string refString, std::string compareString)
{
        std::string testoutput;
	std::string referenceString;
	std::string compString;

	char * comphold;
        char * refhold;

	char comp[bufsz];
	strcpy (comp, compareString.c_str());
	comphold = strtok (comp, "=,");

	char ref[bufsz];
	strcpy (ref, refString.c_str());
	refhold = strtok (ref, "=,");

	while(refhold != NULL)
	{
	//if (match_str(comphold, refhold) != 1){
	//      referenceString = refhold;
	//	compString = comphold;
	//	testoutput = "Failure at Reference Value: " + referenceString + "Comparison Value: " + compString + "\n";
	//	break;
	//}
	refhold = strtok(ref, "=,"); 
	comphold = strtok(comp, "=,");
	}
	return testoutput;
}


