#include "Voice.h"

Voice::~Voice(void) {}

/* Calculates the single voice current sample */
double Voice::nextSample() {
	if (!isActive) return 0.0;
	// The voice is active:
	double oscillatorOutput = mOscillator.nextSample();			// calculate the oscillator's current value,
	double volumeEnvelopeValue = mVolumeEnvelope.nextSample();	// calculate the ADSR current value,
	return mFilter.process(										// apply the filter
		oscillatorOutput * volumeEnvelopeValue					// to the oscillator's output scaled by both the ADSR factor
		* mVelocity / 127.0);									// and the key velocity factor.
}

/* Sets the voice as not active */
void Voice::setFree() {
	isActive = false;
}

/* Resets all the values of the voice */
void Voice::reset() {
	mNoteNumber = -1;
	mVelocity = 0;
	mOscillator.reset();
	mVolumeEnvelope.reset();
	mFilter.reset();
}