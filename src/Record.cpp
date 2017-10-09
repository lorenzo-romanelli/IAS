#include "Record.h"

Record::Record() {
	// Default values...
	dataAvailable = false;				// don't store any data!
	isCollecting = false;				// recording in not active
	mCurrentFrame = 0;
	mLastFrame = 0,
	mWindows = 1;								// let's start with 1 second of samples
	waveOutput.reserve(DATA_WINDOW * mWindows);	// here we reserve memory space for 44100 samples
}

/* Prepares data for recording */
void Record::Prepare() {
	mCurrentFrame = 0;	// we start at the beginning of our output vector
	mLastFrame = 0;		// no frames are stored
	mWindows = 1;		// reset the number of data windows to 1
	std::vector<double>().swap(waveOutput);		// empty the vector from previous recording
	waveOutput.reserve(DATA_WINDOW);			// reserve memory space for 44100 samples

	isCollecting = true;						// recording active, now data can be stored
}

/* Tells the record class to start recording */
void Record::recordOn() {
	dataAvailable = true;	// OK, data can be stored
	Prepare();				
}

/* Stores a sample in the output vector */
void Record::StoreSamples(double sample) {
	if (mCurrentFrame >= waveOutput.capacity()) {		// If the index of the sample we're going to store exceeds vector dimensions
		mWindows++;										
		waveOutput.reserve(DATA_WINDOW * (mWindows));	// reserve memory space for 1 more sec of data.
	}
	waveOutput.push_back(sample);	// Store the sample after the previous one.
	mLastFrame = mCurrentFrame;	
	mCurrentFrame++;
}

/* Tells the record class to stop recording */
void Record::StopRecording() {
	isCollecting = false;			// Don't accept data anymore
	waveOutput.resize(mLastFrame);	// Shrink the vector to the actual number of samples stored
}