#include "MATLABGraphics.h"
#include <Windows.h>

MATLABGraphics::MATLABGraphics(void) {
	mOutputData = NULL;
	mEp = engOpen(NULL);	// open one instance of MATLAB's engine
	if (!mEp) {				// if open fails end the program (drastic...)
		MessageBox ((HWND)NULL, (LPSTR)"Can't start MATLAB engine", 
			(LPSTR) "Engwindemo.c", MB_OK);
		exit(-1);
	}
	setSampleRate(44100.);	// tell MATLAB's engine to store the sampling rate
}

/* Adds (or updates) the sampling rate variable into MATLAB's engine */
void MATLABGraphics::setSampleRate(double sampleRate) {
	mSampleRate = sampleRate;
	mFs = mxCreateDoubleMatrix(1, 1, mxREAL);								// Create a MATLAB matrix of size 1*1 
	memcpy((char *) mxGetPr(mFs), (char *) &mSampleRate, sizeof(double));	// Assign to the above matrix the sample rate value 
	engPutVariable(mEp, "Fs", mFs);											// Put (or update) the variable into MATLAB's engine (naming it "Fs")
}

/* Plots the waveform(s) */
void MATLABGraphics::DrawWaveform(double *waveform, int samples, int subplot) {
	mSamples = samples;
	mOutputData = mxCreateDoubleMatrix(samples, 1, mxREAL);		// Create a MATLAB matrix of size samples*1
	memcpy((char *) mxGetPr(mOutputData), (char *) waveform, samples * sizeof(double));	// Assign to the above matrix the waveform array
	if (subplot == 1) {										// Subplot left channel
		engPutVariable(mEp, "signalLeft", mOutputData);
		engEvalString(mEp, "subplot(2,1,1);");
		engEvalString(mEp, "plot(signalLeft);");
	}
	else {													// Subplot right channel
		engPutVariable(mEp, "signalRight", mOutputData);
		engEvalString(mEp, "subplot(2,1,2);");
		engEvalString(mEp, "plot(signalRight);");
	}
	mxDestroyArray(mOutputData);
}

/* Plays back the data recorded */
void MATLABGraphics::Sound() {
	mStereoOutput = mxCreateDoubleMatrix(mSamples, 2, mxREAL);	// Create a MATLAB matrix of size samples*2 (stereo signal)
	engPutVariable(mEp, "signalStereo", mStereoOutput);	
	engEvalString(mEp, "signalStereo(:, 1) = signalLeft;");		// Store left channel
	engEvalString(mEp, "signalStereo(:, 2) = signalRight;");	// Store right channel
	engEvalString(mEp, "sound(signalStereo, Fs);");				// Play the stereo signal
	mxDestroyArray(mStereoOutput);
}