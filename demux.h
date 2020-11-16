#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string.h>
#include <vector>


using namespace std;

const int readLength = 1000;

vector<string> makeBarcodesList(ifstream &inFile);

vector<string > inputFilesList(ifstream &inFile);

bool barMatch(char barcode[], char challenge[], int length, int mismatches);

struct Read {
	string Bases;
	string Scores;
	char readName[readLength], index[readLength],  readNameRepeat[readLength];
	bool isRead;

	Read(ifstream &Finput) {
		if (Finput >> readName) {
			Finput >> index >> Bases >> readNameRepeat >> Scores;
			isRead = true;
		} else {
			isRead = false;
		}
	}
	void readWriter(ofstream &output) {
		output << readName << " " << index << endl << Bases << endl
				<< readNameRepeat << endl << Scores << endl;
	}
};
