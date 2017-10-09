#include "Oscillator.h"
double Oscillator::mSampleRate = 44100.0;

/* Changes the oscillator waveform */
void Oscillator::setMode(OscillatorMode mode) {
    mOscillatorMode = mode;
}

/* Changes the oscillator frequency */
void Oscillator::setFrequency(double frequency) {
    mFrequency = frequency;
    updateIncrement();	// Phase increment has to be recalculated
}

/* Updates sample rate */
void Oscillator::setSampleRate(double sampleRate) {
    mSampleRate = sampleRate;
    updateIncrement();	// Phase increment has to be recalculated
}

/* Calculates the new phase increment value as soon as frequency or sample rate is changed */
void Oscillator::updateIncrement() {
    mPhaseIncrement = mFrequency * 2 * mPI / mSampleRate;
}

/* Returns the current waveform value according to the waveform selected */
double Oscillator::nextSample() {
    double value = 0.0;
    switch (mOscillatorMode) {
        case OSCILLATOR_MODE_SINE:					// SINE:
            value = sin(mPhase);					// sine value for the current phase value is calculated.
            break;
        case OSCILLATOR_MODE_SAW:					// SAWTOOTH:
            value = 1.0 - (2.0 * mPhase / twoPI);	// sawtooth value is calculated using the following algorithm:
				// - mPhase goes from 0 upwards, and jumps back to 0 when it reaches 2π;
				// - so (mPhase / 2π) goes from 0 upwards and jumps back to 0 when it reaches 1;
				// - this means that (2 * mPhase / 2π) goes from 0 up and jumps back at 2;
				// - when mPhase is 0, the expression 1 - (2 * mPhase / 2π) is 1:
				//	 while mPhase goes upwards, the expression goes downwards and jumps back to 1 when it reaches -1.
            break;
        case OSCILLATOR_MODE_SQUARE:				// SQUARE:
            if (mPhase <= mPI) {					// if phase falls inside the first half of the period
                value = 1.0;						// wave value is 1,
            } else {								// otherwise (if phase falls in the second half of the period)
                value = -1.0;						// wave value is -1.
            }
            break;
        case OSCILLATOR_MODE_TRIANGLE:				// TRIANGLE:
            value = -1.0 + (2.0 * mPhase / twoPI);	// for each frame of the buffer
            value = 2.0 * (fabs(value) - 0.5);		// the wave is generated (the algorithm is similar to the saw one):
													// fabs() calculates the absolute value (flips negative values to positive ones);
													// subtracting 0.5 centers the waveform around 0; 
													// multiplying by 2 makes the values go from -1 to 1.
            break;
    }
    mPhase += mPhaseIncrement;		// Phase value is updated.
    while (mPhase >= twoPI) {		// If phase is greater than the period
        mPhase -= twoPI;			// phase value is shifted back inside the period interval.
    }
    return value;
}

