#pragma once
#include <vector>

class Reverb
{
public:
	Reverb(void);
	~Reverb(void);
	double Process(double sample);
	void SetDelay(double newVal);
	/* Update decay factor */
	inline void SetDecay(double newVal) { mDecay = newVal; }
private:
	double mDelayTime;	// delay time in seconds
	int mDelaySamples;	// delay time in samples
	int mDelayIndex;	// current position inside the delay buffer
	double mSampleRate;	// samples per second
	double mDecay;		// decay factor
	std::vector<double> mDelayBuffer;	// dynamic delay buffer
};

