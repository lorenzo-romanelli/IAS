#pragma once
#include <functional>
#include "Voice.h"

class VoiceManager {
public:
	VoiceManager(void);
	~VoiceManager(void);
	void onNoteOn(int noteNumber, int velocity);
	void onNoteOff(int noteNumber, int velocity);
	double nextSample();
	void setSampleRate(double sampleRate) {
		EnvelopeGenerator::setSampleRate(sampleRate);
		for (int i = 0; i < NumberOfVoices; i++) {
			Voice& voice = voices[i];
			voice.mOscillator.setSampleRate(sampleRate);
		}
	}

	// Calls the function passed as parameter on all the voices
	typedef std::tr1::function<void (Voice&)> VoiceChangerFunction;
	inline void changeAllVoices(VoiceChangerFunction changer) {
		for (int i = 0; i < NumberOfVoices; i++) {
			changer(voices[i]);
		}
	}
	// Set current ADSR's stage value
	static void setVolumeEnvelopeStageValue(Voice& voice, EnvelopeGenerator::EnvelopeStage stage, double value) {
		voice.mVolumeEnvelope.setStageValue(stage, value);
	}
	// Set oscillator waveform
	static void setOscillatorMode(Voice& voice, Oscillator::OscillatorMode mode) {
		voice.mOscillator.setMode(mode);
	}
	// Set filter cutoff
	static void setFilterCutoff(Voice& voice, double cutoff) {
		voice.mFilter.setCutoff(cutoff);
	}
	// Set filter resonance
	static void setFilterResonance(Voice& voice, double resonance) {
		voice.mFilter.setResonance(resonance);
	}
	// Set filter type (lowpass - highpass - bandpass)
	static void setFilterMode(Voice& voice, Filter::FilterMode mode) {
		voice.mFilter.setFilterMode(mode);
	}
private:
	static const int NumberOfVoices = 64; // voices polyphony
	Voice voices[NumberOfVoices];
	Voice* findFreeVoice();
};

