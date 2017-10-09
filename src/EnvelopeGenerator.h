#ifndef __Synthesis__EnvelopeGenerator__
#define __Synthesis__EnvelopeGenerator__

#include <cmath>
#include "GallantSignal.h"
using Gallant::Signal0;

class EnvelopeGenerator {
public:
	enum EnvelopeStage {			// ADSR stages:
		ENVELOPE_STAGE_OFF = 0,		// - not active
		ENVELOPE_STAGE_ATTACK,		// - attack
		ENVELOPE_STAGE_DECAY,		// - decay
		ENVELOPE_STAGE_SUSTAIN,		// - sustain
		ENVELOPE_STAGE_RELEASE,		// - release
		kNumEnvelopeStages			// (number of stages)
	};
	Signal0<> beganEnvelopeCycle;		// signal for when the envelope has begun
	Signal0<> finishedEnvelopeCycle;	// signal for when the envelope has stopped
	void enterStage(EnvelopeStage newStage);
	double nextSample();
	static void setSampleRate(double newSampleRate);
	void setStageValue(EnvelopeStage stage, double value);
	/* Returns the current ADSR stage */
	inline EnvelopeStage getCurrentStage() const { return currentStage; };
	const double minimumLevel;	// envelope calculations don't work with an amplitude of zero
	/* Resets all the values of the ADSR */
	void reset() {
		currentStage = ENVELOPE_STAGE_OFF;
		currentLevel = minimumLevel;
		multiplier = 1.0;
		currentSampleIndex = 0;
		nextStageSampleIndex = 0;
	}
	
	EnvelopeGenerator() :
		minimumLevel(0.0001),
		currentStage(ENVELOPE_STAGE_OFF),
		currentLevel(minimumLevel),
		multiplier(1.0),
		currentSampleIndex(0),
		nextStageSampleIndex(0) {
			// Default values...
			stageValue[ENVELOPE_STAGE_OFF] = 0.0;
			stageValue[ENVELOPE_STAGE_ATTACK] = 0.01;
			stageValue[ENVELOPE_STAGE_DECAY] = 0.5;
			stageValue[ENVELOPE_STAGE_SUSTAIN] = 0.1;
			stageValue[ENVELOPE_STAGE_RELEASE] = 1.0;
	};
private:
	EnvelopeStage currentStage;	// current envelope stage
	double currentLevel;		// current calculated value of the ADSR
	double multiplier;			// responsible for the exponential decay
	static double sampleRate;	
	double stageValue[kNumEnvelopeStages];	// current values for each stage of the ADSR
	void calculateMultiplier(double startLevel, double endLevel, unsigned long long lengthInSamples);
	unsigned long long currentSampleIndex;		// current sample processed
	unsigned long long nextStageSampleIndex;	// sample at which the envelope stage will change
};

#endif
