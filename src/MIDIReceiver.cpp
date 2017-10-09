#include "MIDIReceiver.h"

void MIDIReceiver::onMessageReceived(IMidiMsg* midiMessage) {
    IMidiMsg::EStatusMsg status = midiMessage->StatusMsg();
    // We're only interested in Note On/Off messages (not CC, pitch, etc.)
    if (status == IMidiMsg::kNoteOn || status == IMidiMsg::kNoteOff) {
        mMidiQueue.Add(midiMessage);
    }
}

void MIDIReceiver::advance() {
    while (!mMidiQueue.Empty()) {
        IMidiMsg* midiMessage = mMidiQueue.Peek();	// Get the MIDI message
        if (midiMessage->mOffset > mOffset) break;	// Do not process the messages generated after the sample's relative time
        
        IMidiMsg::EStatusMsg status = midiMessage->StatusMsg();
        int noteNumber = midiMessage->NoteNumber();
        int velocity = midiMessage->Velocity();
        // There are only note on/off messages in the queue, see ::OnMessageReceived
		if (status == IMidiMsg::kNoteOn && velocity) {	
            if(mKeyStatus[noteNumber] == false) {
                mKeyStatus[noteNumber] = true;
                mNumKeys += 1;
				// Emit a "note on" signal (see ProgettoIAS's class constructor)
                noteOn(noteNumber, velocity);
            }  
        } else {
            if(mKeyStatus[noteNumber] == true) {
                mKeyStatus[noteNumber] = false;
                mNumKeys -= 1;
				// Emit a "note off" signal
				noteOff(noteNumber, velocity);
            }
        }
        mMidiQueue.Remove(); // Remove the message from the MIDI queue
    }
    mOffset++;
}