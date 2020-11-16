#include "threading.h"
#include "demux.h"

using namespace std;

int numberOfThreads = 0;
bool threadDone[500];
bool threadStarted[500];

thread_return_data *demux(int length, int mismatches, string inFilename, int thID, vector<string> barcodesList);
void *Thread(void *threadarg) {
	thread_return_data *rc;
	thread_data *td;
	td = ( thread_data *) threadarg;
	cout << td->threadID << ":  " << td->inFilename << " " <<  td->barLength << " " << td->mismatches << " started." << endl;
	rc = demux( td->barLength, td->mismatches, td->inFilename, td->threadID, td->barcodes);
	threadDone[td->threadID] = true;
	cout << td->threadID << " done. ";
	showRunning(numberOfThreads);
	pthread_exit(rc);
}

bool allDone(int nOT) {
	int q = 0;
	for (int i = 0; i <= nOT; i++) {
		if (threadDone[i]) q++;
	}
	if (q == nOT) return true; else return false;
}

void showRunning (int nOT) {
	cout << "Threads running: ";
	for (int i = 0; i <= nOT; i++) {
		if (threadStarted[i] && !threadDone[i]) cout << "t:" << i << "\t"; 
	}
	cout << endl;
}

void createThreads(vector<string> inputFilesList, int mismatches, int barcodeLength, vector<string> barcodesList) {
	int maxThreads = 11;
	numberOfThreads = inputFilesList.size();
	pthread_t threads[numberOfThreads];
	thread_data td[numberOfThreads];
	thread_return_data *rc[numberOfThreads];

	for (int i = 0; i < numberOfThreads; i++) {
		td[i] = thread_data(mismatches,barcodeLength,i,inputFilesList[i],barcodesList);
	}

	time_t msec = time(NULL);
	for (int i = 0; i < maxThreads; i++) {
		threadStarted[i] = true;
		pthread_create(&threads[i], NULL, Thread, (void *)&td[i]);
		usleep(100000);
	}
	while (!allDone(numberOfThreads)) {
		for (int i = 0; i < numberOfThreads; i++) {
			if (threadDone[i] && ((i+maxThreads) < numberOfThreads) && !threadStarted[i+maxThreads]) {
				pthread_create(&threads[i+maxThreads], NULL, Thread, (void *)&td[i+maxThreads]);
				threadStarted[i+maxThreads] = true;
			}
		}
		usleep(100000);
	}
	for (int i = 0; i < numberOfThreads; i++) {
		pthread_join(threads[i], (void **)&rc[i]);
		for (unsigned int p = 0; p < barcodesList.size(); p++) {
			cout << inputFilesList[i] << "\t" << barcodesList[p] << "\t" << rc[i]->readCounters[i][p] << endl;
		}
	}


	cout << endl <<"ALL took: " << (time(NULL)) - msec << " seconds." << endl;
	pthread_exit(NULL);
}
