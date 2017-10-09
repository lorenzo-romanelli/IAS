#ifndef __Synthesis__Oscillator__
#define __Synthesis__Oscillator__

#include <math.h>

class Oscillator {
public:
	enum OscillatorMode {			// Waveforms:
		OSCILLATOR_MODE_SINE,		// - sine
		OSCILLATOR_MODE_SAW,		// - saw
		OSCILLATOR_MODE_SQUARE,		// - square
		OSCILLATOR_MODE_TRIANGLE,	// - triangle
		kNumOscillatorModes			// (number of waveforms)
	};
    void setMode(OscillatorMode mode);
    void setFrequency(double frequency);
    void setSampleRate(double sampleRate);
    double nextSample();
	void reset() { mPhase = 0.0; }
    Oscillator() :
		// Default values
		mOscillatorMode(OSCILLATOR_MODE_SINE),
		mPI(2*acos(0.0)),
		twoPI(2 * mPI),
		mFrequency(440.0),
		mPhase(0.0) { updateIncrement(); };
private:
    OscillatorMode mOscillatorMode;
    const double mPI;			// π = 2 * arccos(0)
	const double twoPI;			// 2 * π
    double mFrequency;			// wave frequency (Hz)
    double mPhase;				// wave phase
    static double mSampleRate;	// samples per second
    double mPhaseIncrement;		// frequency * 2 * π / samplerate
    void updateIncrement();
};

#endif /* defined(__Synthesis__Oscillator__) */
