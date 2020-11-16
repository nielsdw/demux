#include "demux.h"
#include "threading.h"

using namespace std;

vector<string> makeBarcodesList(ifstream &inFile){
	vector<string > barcodesList;
	string barcode;
	while (inFile >> barcode) {
		barcodesList.push_back(barcode) ;
	}
	return barcodesList;

}

vector<string > inputFilesList(ifstream &inFile){
	vector<string > inputFilesList;
	string inputFile;
	while (inFile >> inputFile) {
		inputFilesList.push_back(inputFile);
	}
	return inputFilesList;
}

bool barMatch(char barcode[], char challenge[], int length, int mismatches) {
	int matches = 0;
	for (int i = 0; i < length; i++) {

		if (barcode[i] == challenge[i])
			matches++;
	}
	return (bool) ((matches + mismatches) >= length);
}

thread_return_data *demux( int length, int mismatches, string inFilename, int thID, vector<string> barcodesList) {
	thread_return_data *rc = new thread_return_data();

	Read *aRead;
	ifstream inFile(inFilename.c_str());
	ofstream *output = new ofstream[barcodesList.size()];

	for (unsigned int i = 0; i < barcodesList.size(); i++) {
		string outFileName = "test2/"+inFilename+" "+barcodesList[i]+".fastq";
		output[i].open(outFileName.c_str());
	}
	string outUndet = "test2/"+inFilename+" Undetermined.fastq";
	ofstream Undet(outUndet.c_str());

	int barMatches = 0;
	int barMatchNo = 0;
	while ((aRead = (new Read(inFile)))->isRead) {
		for (unsigned int i = 0; i < barcodesList.size(); i++) {
			if (barMatch((char *)aRead->Bases.c_str(), (char *)barcodesList[i].c_str(), length, mismatches )) {
				barMatches++;
				barMatchNo = i;
			}
		}
		if (barMatches == 1) {
			strcat(aRead->index,(char *) barcodesList[barMatchNo].c_str());
			aRead->Bases = aRead->Bases.substr(length, aRead->Bases.size());
			aRead->Scores = aRead->Scores.substr(length, aRead->Bases.size());
			aRead->readWriter(output[barMatchNo]);
			rc->readCounters[thID][barMatchNo]++;
		}
		else if (barMatches == 0) {
			aRead->readWriter(Undet);
		}
		else if (barMatches > 1) {
			cout << "Barcode collision" << endl;
			return rc;
		}
		barMatches = 0;
		barMatchNo = 0;

		delete aRead;
	}
	for (unsigned int i = 0; i < barcodesList.size(); i++) {
		output[i].close();
	}
	return rc;
}


int main(int argc, char *argv[]) {
	if (argc != 4) {
		cout << "Invalid argument count: Must be: demux fileslist barcodeslist mismatches" << endl;
		return 1;
	}
	ifstream filesListFile(argv[1]);
	ifstream barcodesListFile(argv[2]);
	int mismatches = atoi(argv[3]);
	int barcodeLength = 6;
	vector<string> inputFiles = inputFilesList(filesListFile);
	vector<string> barcodesList;
	barcodesList = makeBarcodesList(barcodesListFile);
	createThreads(inputFiles, mismatches, barcodeLength, barcodesList);
	pthread_exit(NULL);
	return 0;
}
