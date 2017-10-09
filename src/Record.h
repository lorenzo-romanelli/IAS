#pragma once
#include <vector>
#include "GallantSignal.h"

class Record
{
public:
	bool dataAvailable;	// prevents the plugin from sending data when recording is not active
	Record(void);
	void Prepare();
	void recordOn();
	void StopRecording();
	void StoreSamples(double sample);
	/* Returns a pointer to the beginning of stored data */
	inline double* GetRecordedData() { return &waveOutput[0]; };
	/* Returns the number of samples stored */
	inline unsigned long long GetDataLength() { return mLastFrame; };
	/* Checks if recording is active */
	inline bool IsActive() { return isCollecting; };
private:
	static const int DATA_WINDOW = 44100;	// length of one data window (default: 1 sec)
	bool isCollecting;						// true when recording is active
	unsigned long long mCurrentFrame;		// sample that has to be stored
	unsigned long long mLastFrame;			// number of samples stored
	int mWindows;							// number of windows used to store data
	std::vector<double> waveOutput;			// dynamic vector where we store samples
};