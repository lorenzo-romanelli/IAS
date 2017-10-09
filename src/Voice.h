#pragma once
#include "Oscillator.h"
#include "EnvelopeGenerator.h"
#include "Filter.h"

class Voice {
public:
	friend class VoiceManager;	// this lets the VoiceManager class access Voice class data
	Voice(void) :
		mNoteNumber(-1),
		mVelocity(0),
		isActive(false) {
			// when the voice's ADSR leaves the release stage, the voice is deactivated 
			mVolumeEnvelope.finishedEnvelopeCycle.Connect(this, &Voice::setFree);
	};
	~Voice(void);

	/* Calculates the oscillator frequency based on a MIDI note number */
	inline void setNoteNumber(int noteNumber) {
		mNoteNumber = noteNumber;
		double frequency = 440.0 * pow(2.0, (mNoteNumber - 69.0) / 12.0);
		mOscillator.setFrequency(frequency); // the frequency is sent to the oscillator
	}
	double nextSample();
	void setFree();
	void reset();
private:
	Oscillator mOscillator;
	EnvelopeGenerator mVolumeEnvelope;
	Filter mFilter;
	int mNoteNumber;	// MIDI key number
	int mVelocity;		// MIDI velocity
	bool isActive;
};

