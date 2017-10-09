#pragma once
#include <vector>
#include "engine.h"

class MATLABGraphics {
public:
	MATLABGraphics(void);
	void DrawWaveform(double *waveform, int samples, int subplot);
	void setSampleRate(double sampleRate);
	void Sound();
private:
	double mSampleRate;			// needed for playing back the recording
	long long int mSamples;		// length of the audio data
	Engine* mEp;				// instance of MATLAB engine
	mxArray *mOutputData;		// MATLAB-readable data array (mono signal)
	mxArray *mFs;				// MATLAB-readable variable for sample rate
	mxArray *mStereoOutput;		// MATLAB-readable stereo signal
};

