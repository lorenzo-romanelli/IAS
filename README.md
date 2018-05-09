# IAS
Polyphonic VST synthesizer

## The project
This VST Synth has been developed as the final project of a course in digital audio signal processing. It's written in C++ and uses Matlab APIs to record/playback audio, as well as plotting its waveform.

It works on Windows 8 (tested, possibly also on Windows 10?) ... and is also MIDI compatible, yay! :)

## About the code
The code contained in this directory is not the complete code.
To compile it you would need:
* Visual Studio (the .dll has been compiled on VS 2012)
* wdl-ol framework by Oli Larkin (https://github.com/olilarkin/wdl-ol)
* Matlab

You can still try it out by importing the .dll ("Synthesis.dll") into some VST host, e.g. Savihost. DAW integration unfortunately hasn't been properly tested yet :(
