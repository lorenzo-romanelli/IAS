#include "Filter.h"

/*	 Applies the filter to the input sample  */
/*		   Algorithm by Paul Kellet			 */
/* http://www.musicdsp.org/showone.php?id=29 */
double Filter::process(double inputValue) {
	// As we can see from the calculations, this is an IIR filter.
	buf0 += cutoff * (inputValue - buf0 + feedbackAmount * (buf0 - buf1));	// first order filter (-6dB per octave)
	buf1 += cutoff * (buf0 - buf1);	// second order filter (-12dB per octave)
	buf2 += cutoff * (buf1 - buf2);	// third order filter (-18dB per octave)
	buf3 += cutoff * (buf2 - buf3); // fourth order filter (-24dB per octave)
	switch (mode) {
	case Filter::FILTER_MODE_LOWPASS:	// LOWPASS:
		return buf3;					// returns the last filter calculated.
	case Filter::FILTER_MODE_HIGHPASS:	// HIGHPASS:
		return inputValue - buf3;		// returns the value minus the last filter (low frequencies are cut off).
	case Filter::FILTER_MODE_BANDPASS:	// BANDPASS:
		return buf0 - buf3;				// returns what's between the first filter and the last one.
	default:
		return 0.0;
	}
}
