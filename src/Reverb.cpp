#include "Reverb.h"

Reverb::Reverb(void) {
	// Default values...
	mSampleRate = 44100.;
	mDelayTime = 0.317;
	mDelaySamples = mDelayTime * mSampleRate;
	mDelayIndex = 0;
	mDelayBuffer.resize(mDelaySamples, 0.);	// Resize the delay buffer according to the delay time
											// and fill it with zeroes.
	mDecay = 0.273;
}

Reverb::~Reverb(void) {
}

/* Updates the delay time */
void Reverb::SetDelay(double newVal) {
	mDelayTime = newVal;
	mDelaySamples = mDelayTime * mSampleRate;
	mDelayBuffer.resize(mDelaySamples, 0.);	// Resize the delay buffer according to the new delay time
											// and fill it with zeroes.
	mDelayIndex = 0;						// Go back to the beginning of the buffer
}

/* Applies the reverb to the sample */
double Reverb::Process(double sample) {
	// Calculate the output sample according to the delay line content
	double output = sample - (mDecay * mDelayBuffer[mDelayIndex]);
	// Update the delay line
	mDelayBuffer[mDelayIndex] = sample + (mDecay * mDelayBuffer[mDelayIndex]);
	mDelayIndex = (mDelayIndex + 1) % mDelaySamples; // Circular buffer
	return output;
}