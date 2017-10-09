#include "ProgettoIAS.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmain"
#include "IPlug_include_in_plug_src.h"
#pragma clang diagnostic pop
#include "IControl.h"
#include "IKeyboardControl.h"
#include "resource.h"

#include <math.h>
#include <algorithm>
#include <functional>

const int kNumPrograms = 5;
const double parameterStep = 0.001;	// Filter and ADSR parameters step

// GUI parameters
enum EParams {
	// Volume envelope
	mVolumeEnvAttack = 0,
	mVolumeEnvDecay,
	mVolumeEnvSustain,
	mVolumeEnvRelease,
	// Oscillator
	mOsc1Waveform,
	// Filter
	mFilterMode,
	mFilterCutoff,
	mFilterResonance,
	// Other stuff
	kRecord,
	kGain,

	kNumParams
};

// GUI parameters coordinates
enum ELayout {
	kWidth = GUI_WIDTH,
	kHeight = GUI_HEIGHT,
	
	kKeybX = 1,
	kKeybY = 230,
	
	kGainX = 350,
	kGainY = 177,
	kKnobFrames = 60,

	kRecordX = 190,
	kRecordY = 177,
	kRecordSteps = 2,

	kWaveformX = 24,
	kWaveformY = 38,

	kFilterModeX = 24,
	kFilterModeY = 123,
	kCutoffX = 5,
	kCutoffY = 177,
	kResonanceX = 61,
	kResonanceY = 177,

	kVolAtt = 110,
	kVolDec = 190,
	kVolSus = 270,
	kVolRel = 350,
	kVol = 38
};

/* *** Plugin constructor *** */
ProgettoIAS::ProgettoIAS(IPlugInstanceInfo instanceInfo)
:   IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo),
lastVirtualKeyboardNoteNumber(virtualKeyboardMinimumNoteNumber - 1) {
	TRACE;
	
	// Let's create graphics...
	IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);

	// Background (/resources/img/bg.png)
	pGraphics->AttachBackground(BG_ID, BG_FN);
	
	// Pressed keyboard keys (unpressed keys are part of the background)
	IBitmap whiteKeyImage = pGraphics->LoadIBitmap(WHITE_KEY_ID, WHITE_KEY_FN, 6);
	IBitmap blackKeyImage = pGraphics->LoadIBitmap(BLACK_KEY_ID, BLACK_KEY_FN);

	// IKeyboardControl will take care of all the octaves given the coordinates of the first one
	//                            C#     D#          F#      G#      A#
	int keyCoordinates[12] = { 0, 7, 12, 20, 24, 36, 43, 48, 56, 60, 69, 72 };
	mVirtualKeyboard = new IKeyboardControl(this, kKeybX, kKeybY, virtualKeyboardMinimumNoteNumber, /* octaves: */ 5, &whiteKeyImage, &blackKeyImage, keyCoordinates);
	pGraphics->AttachControl(mVirtualKeyboard);

	// Gain
	//				 args: param name, default val, min, max, step, label
	GetParam(kGain)->InitDouble("Gain", 50., 0., 100.0, 0.01, "%");
	GetParam(kGain)->SetShape(3);
	IBitmap bitmap = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);
	pGraphics->AttachControl(new IKnobMultiControl(this, kGainX, kGainY, kGain, &bitmap));
	
	// ADSR
	GetParam(mVolumeEnvAttack)->InitDouble("Volume Attack", 0.01, 0.01, 10.0, parameterStep);
	GetParam(mVolumeEnvAttack)->SetShape(3);
	pGraphics->AttachControl(new IKnobMultiControl(this, kVolAtt, kVol, mVolumeEnvAttack, &bitmap));
	GetParam(mVolumeEnvDecay)->InitDouble("Decay", 0.5, 0.01, 15.0, parameterStep);
	GetParam(mVolumeEnvDecay)->SetShape(3);
	pGraphics->AttachControl(new IKnobMultiControl(this, kVolDec, kVol, mVolumeEnvDecay, &bitmap));
	GetParam(mVolumeEnvSustain)->InitDouble("Sustain", 0.1, 0.001, 1.0, parameterStep);
	GetParam(mVolumeEnvSustain)->SetShape(2);
	pGraphics->AttachControl(new IKnobMultiControl(this, kVolSus, kVol, mVolumeEnvSustain, &bitmap));
	GetParam(mVolumeEnvRelease)->InitDouble("Release", 1.0, 0.001, 15.0, parameterStep);
	GetParam(mVolumeEnvRelease)->SetShape(3);
	pGraphics->AttachControl(new IKnobMultiControl(this, kVolRel, kVol, mVolumeEnvRelease, &bitmap));
	
	// Filter knobs
	GetParam(mFilterCutoff)->InitDouble("Cutoff", 0.99, 0.01, 0.99, parameterStep);
	GetParam(mFilterCutoff)->SetShape(2);
	pGraphics->AttachControl(new IKnobMultiControl(this, kCutoffX, kCutoffY, mFilterCutoff, &bitmap));
	GetParam(mFilterResonance)->InitDouble("Resonance", 0.01, 0.01, 1.0, parameterStep);
	pGraphics->AttachControl(new IKnobMultiControl(this, kResonanceX, kResonanceY, mFilterResonance, &bitmap));
	// Filter mode
	GetParam(mFilterMode)->InitEnum("Filter Mode", Filter::FILTER_MODE_LOWPASS, Filter::kNumFilterModes);
	IBitmap filtermodeBitmap = pGraphics->LoadIBitmap(FILTERMODE_ID, FILTERMODE_FN, 3);
	pGraphics->AttachControl(new ISwitchControl(this, kFilterModeX, kFilterModeY, mFilterMode, &filtermodeBitmap));
	
	// Osc Waveform
	GetParam(mOsc1Waveform)->InitEnum("Waveform", Oscillator::OSCILLATOR_MODE_SINE, Oscillator::kNumOscillatorModes);
	GetParam(mOsc1Waveform)->SetDisplayText(0, "Sine");
	bitmap = pGraphics->LoadIBitmap(WAVEFORM_ID, WAVEFORM_FN, 4);
	pGraphics->AttachControl(new ISwitchControl(this, kWaveformX, kWaveformY, mOsc1Waveform, &bitmap));
	
	// Record
	GetParam(kRecord)->InitBool("Record", false);
	bitmap = pGraphics->LoadIBitmap(SWITCH_ID, SWITCH_FN, kRecordSteps);
	pGraphics->AttachControl(new ISwitchControl(this, kRecordX, kRecordY, kRecord, &bitmap));
	AttachGraphics(pGraphics);
	// Graphics done!
	
	// No presets right now, maybe in a future release...
	CreatePresets();

	// Signal to slots connection (for info: http://doc.qt.io/qt-5/signalsandslots.html)
	mMIDIReceiver.noteOn.Connect(&voiceManager, &VoiceManager::onNoteOn);
	mMIDIReceiver.noteOff.Connect(&voiceManager, &VoiceManager::onNoteOff);

	// Right channel reverb params
	mReverbRight.SetDecay(0.2631);
	mReverbRight.SetDelay(0.304);

	// MATLAB object used for graphics
	mMatlab = new MATLABGraphics();
}
/* *** End of plugin constructor *** */

/* *** Plugin destructor *** */
ProgettoIAS::~ProgettoIAS() {
	delete mMatlab;
}

/* *** Presets initializer *** */
void ProgettoIAS::CreatePresets() {
}

/*	************************ Audio data generation ************************* */
/* This function is constantly generating samples, even if no key is pressed */
void ProgettoIAS::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames) {
	// Mutex is already locked for us.

	double *leftOutput = outputs[0];	// left output channel
	double *rightOutput = outputs[1];	// right output channel
	double drySignal;					// dry sample (without reverb)

	// We generate MIDI message(s) according to the user input on the keyboard
	processVirtualKeyboard();

	for (int i = 0; i < nFrames; ++i) {						// For each sample in the buffer
		mMIDIReceiver.advance();							// we check the current MIDI msg
		drySignal = voiceManager.nextSample() * mGain;		// and we generate the audio data without reverb...

		leftOutput[i] = mReverbLeft.Process(drySignal);		// ... which is now calculated for the left channel
		rightOutput[i] = mReverbRight.Process(drySignal);	// and the right channel.

		if (mRecordLeft.IsActive() && mRecordRight.IsActive()) {	// Finally, if recording is active 
			mRecordLeft.StoreSamples(leftOutput[i]);				// we store the samples (which will be sent to MATLAB).
			mRecordRight.StoreSamples(rightOutput[i]);
		}
	}
  
	mMIDIReceiver.Flush(nFrames);	// We empty the MIDI msg queue
}

/* ********************************* Reset *********************************** */
/* This function is called whenever the sampling rate of the plugin is changed */
void ProgettoIAS::Reset()
{
	TRACE;
	IMutexLock lock(this);
	double sampleRate = GetSampleRate();
	voiceManager.setSampleRate(sampleRate);
	mMatlab->setSampleRate(GetSampleRate());
}

/* ************ GUI parameters handler ************* */
/* Processes user input (knobs turning, switches...) */
/*           Argument: graphic object ID			 */
void ProgettoIAS::OnParamChange(int paramIdx)
{
	IMutexLock lock(this);

	using std::tr1::placeholders::_1;
	using std::tr1::bind;
	VoiceManager::VoiceChangerFunction changer; // function defined according to the parameter we're changing
	
	switch (paramIdx) {
	// PREMISE: bind() pre-fills the arguments of the function passed as the 1st parameter.
	// _1 means that the function argument in that position is not being pre-filled,
	// but passed in when "changer" gets called.
	// changeAllVoices() calls the function assigned to "changer" on all active voices.
	
	// ADSR attack
	case mVolumeEnvAttack:
		changer = bind(&VoiceManager::setVolumeEnvelopeStageValue, _1,
			EnvelopeGenerator::ENVELOPE_STAGE_ATTACK, GetParam(mVolumeEnvAttack)->Value());
		voiceManager.changeAllVoices(changer);
		break;
	// ADSR decay
	case mVolumeEnvDecay:
		changer = bind(&VoiceManager::setVolumeEnvelopeStageValue, _1, 
			EnvelopeGenerator::ENVELOPE_STAGE_DECAY, GetParam(mVolumeEnvDecay)->Value());
		voiceManager.changeAllVoices(changer);
		break;
	// ADSR sustain
	case mVolumeEnvSustain:
		changer = bind(&VoiceManager::setVolumeEnvelopeStageValue, _1, 
			EnvelopeGenerator::ENVELOPE_STAGE_SUSTAIN, GetParam(mVolumeEnvSustain)->Value());
		voiceManager.changeAllVoices(changer);
		break;
	// ADSR release
	case mVolumeEnvRelease:
		changer = bind(&VoiceManager::setVolumeEnvelopeStageValue, _1, 
			EnvelopeGenerator::ENVELOPE_STAGE_RELEASE, GetParam(mVolumeEnvRelease)->Value());
		voiceManager.changeAllVoices(changer);
		break;
	// Oscillator waveform
	case mOsc1Waveform:
		changer = bind(&VoiceManager::setOscillatorMode, _1, 
			static_cast<Oscillator::OscillatorMode>(GetParam(mOsc1Waveform)->Int()));
		voiceManager.changeAllVoices(changer);
		break;
	// Filter cutoff
	case mFilterCutoff:
		changer = bind(&VoiceManager::setFilterCutoff, _1, GetParam(mFilterCutoff)->Value());
		voiceManager.changeAllVoices(changer);
		break;
	// Filter resonance
	case mFilterResonance:
		changer = bind(&VoiceManager::setFilterResonance, _1, GetParam(mFilterResonance)->Value());
		voiceManager.changeAllVoices(changer);
		break;
	// Filter mode
	case mFilterMode:
		changer = bind(&VoiceManager::setFilterMode, _1, 
			static_cast<Filter::FilterMode>(GetParam(mFilterMode)->Int()));
		voiceManager.changeAllVoices(changer);
		break;
	// Gain (global)
	case kGain:
		mGain = GetParam(kGain)->Value() / 100.;
		break;
	// Record on/off
	case kRecord:
		// Recording has started
		if (GetParam(kRecord)->Bool()) { mRecordLeft.recordOn(); mRecordRight.recordOn(); }
		// Recording has stopped
		else if (!GetParam(kRecord)->Bool() && mRecordLeft.dataAvailable && mRecordRight.dataAvailable ) { 
			mRecordLeft.StopRecording();
			mRecordRight.StopRecording();
			// Sending data to MATLAB's engine
			mMatlab->DrawWaveform(mRecordLeft.GetRecordedData(), mRecordLeft.GetDataLength(), 1);
			mMatlab->DrawWaveform(mRecordRight.GetRecordedData(), mRecordRight.GetDataLength(), 2);
			// Play the recording
			mMatlab->Sound();
		}
		break;
	default:
		break;
	}
}

void ProgettoIAS::ProcessMidiMsg(IMidiMsg* pMsg) {
	mMIDIReceiver.onMessageReceived(pMsg);
	// Telling the graphics engine to redraw the key(s) according to the MIDI msg:
	mVirtualKeyboard->SetDirty();
}

void ProgettoIAS::processVirtualKeyboard() {
	IKeyboardControl* virtualKeyboard = (IKeyboardControl*) mVirtualKeyboard;
	int virtualKeyboardNoteNumber = virtualKeyboard->GetKey() + virtualKeyboardMinimumNoteNumber;
  
	if (lastVirtualKeyboardNoteNumber >= virtualKeyboardMinimumNoteNumber && virtualKeyboardNoteNumber != lastVirtualKeyboardNoteNumber) {
		// The note number has changed from a valid key to something else (valid key or nothing). Release the valid key:
		IMidiMsg midiMessage;
		midiMessage.MakeNoteOffMsg(lastVirtualKeyboardNoteNumber, 0);
		mMIDIReceiver.onMessageReceived(&midiMessage);
	}
  
	if (virtualKeyboardNoteNumber >= virtualKeyboardMinimumNoteNumber && virtualKeyboardNoteNumber != lastVirtualKeyboardNoteNumber) {
		// A valid key is pressed that wasn't pressed the previous call. Send a "note on" message to the MIDI receiver:
		IMidiMsg midiMessage;
		midiMessage.MakeNoteOnMsg(virtualKeyboardNoteNumber, virtualKeyboard->GetVelocity(), 0);
		mMIDIReceiver.onMessageReceived(&midiMessage);
	}
  
	lastVirtualKeyboardNoteNumber = virtualKeyboardNoteNumber;
}