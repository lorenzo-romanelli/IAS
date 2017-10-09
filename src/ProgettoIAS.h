#ifndef __SYNTHESIS__
#define __SYNTHESIS__

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextra-tokens"
#include "IPlug_include_in_plug_hdr.h"
#pragma clang diagnostic pop

#include "MIDIReceiver.h"
#include "MATLABGraphics.h"
#include "Record.h"
#include "Reverb.h"
#include "VoiceManager.h"

class ProgettoIAS : public IPlug {
public:
	ProgettoIAS(IPlugInstanceInfo instanceInfo);
	~ProgettoIAS();
	
	void Reset();
	void OnParamChange(int paramIdx);
	void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);
	// to receive MIDI messages:
	void ProcessMidiMsg(IMidiMsg* pMsg);
  
	// Needed for the GUI keyboard:
	// Should return non-zero if one or more keys are playing.
	inline int GetNumKeys() const { return mMIDIReceiver.getNumKeys(); };
	// Should return true if the specified key is playing.
	inline bool GetKeyStatus(int key) const { return mMIDIReceiver.getKeyStatus(key); };
	static const int virtualKeyboardMinimumNoteNumber = 48;
	int lastVirtualKeyboardNoteNumber;
  
private:
	double mGain;
	void CreatePresets();
	MIDIReceiver mMIDIReceiver;
	IControl* mVirtualKeyboard;
	MATLABGraphics* mMatlab;
	Record mRecordLeft;
	Record mRecordRight;
	Reverb mReverbLeft;
	Reverb mReverbRight;
	VoiceManager voiceManager;
	void processVirtualKeyboard();
};

#endif
