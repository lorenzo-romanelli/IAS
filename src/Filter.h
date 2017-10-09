#pragma once
class Filter
{
public:
	enum FilterMode {				// Filter types:
		FILTER_MODE_LOWPASS = 0,	// - lowpass
		FILTER_MODE_HIGHPASS,		// - highpass
		FILTER_MODE_BANDPASS,		// - bandpass
		kNumFilterModes				// (number of filter types)
	};
	Filter() :
		// Default values...
		cutoff(0.99),
		resonance(0.0),
		mode(FILTER_MODE_LOWPASS),
		buf0(0.0),
		buf1(0.0),
		buf2(0.0),
		buf3(0.0) {
			calculateFeedbackAmount();
	};
	double process(double inputValue);
	/* Updates cutoff value */
	inline void setCutoff(double newCutoff) { 
		cutoff = newCutoff;
		calculateFeedbackAmount(); // feedback has to be recalculated
	}
	/* Updates resonance value */
	inline void setResonance(double newResonance) { 
		resonance = newResonance; 
		calculateFeedbackAmount(); // feedback has to be recalculated
	}
	/* Updates the filter type */
	inline void setFilterMode(FilterMode newMode) { mode = newMode; }
	/* Resets all the filters */
	void reset() {
		buf0 = buf1 = buf2 = buf3 = 0.0;
	}
private:
	double cutoff;			// value used to apply cutoff	 (min 0.01 - max 0.99)
	double resonance;		// value used to apply resonance (min 0.01 - max 1.00)
	FilterMode mode;		// filter type
	double feedbackAmount;	// feedback of the IIR filter
	/* Calculates the amount of IIR filter feedback */
	inline void calculateFeedbackAmount() { feedbackAmount = resonance + resonance / (1.0 - cutoff); }
	// Applying four 1st order filters in series gives us a 4th order filter:
	double buf0;	// First order filter
	double buf1;	// First order filter
	double buf2;	// First order filter
	double buf3;	// First order filter
};

