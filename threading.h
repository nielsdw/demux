#include <time.h>
#include <pthread.h>
#include <string>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <fstream>


using namespace std;

void *Thread(void *threadarg);

void createThreads(vector<string> inputFilesList, int mismatches, int barcodeLength, vector<string> barcodesList);

bool allDone(int nOT);

void showRunning (int nOT);



struct thread_data {

	string inFilename;
	vector<string> barcodes;
	thread_data(){
			int mismatches = 0;
			int barLength = 0 ;
			int threadID = 0;
	} // necessary for init of thread_data array
	thread_data(int mm, int barln, int thID, string file, vector<string> bc){
		mismatches = mm;
		barLength = barln;
		threadID = thID;
		inFilename = file;
		barcodes = bc;
	}
};
struct thread_return_data {
	int readCounters[1000][1000];
	thread_return_data() {
		for (int i = 0; i < 1000; i++) {
			for(int p = 0; p < 1000; p++) {
				readCounters[i][p] = 0;
			}
		}
	}
};
thread_return_data *demux(int length, int mismatches, string inFilename, int thID, vector<string> barcodesList);
