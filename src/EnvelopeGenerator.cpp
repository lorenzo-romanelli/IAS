#include <algorithm>
#include "EnvelopeGenerator.h"
double EnvelopeGenerator::sampleRate = 44100.0;

/* Calculates the current stage value */
double EnvelopeGenerator::nextSample() {
	if (currentStage != ENVELOPE_STAGE_OFF && currentStage != ENVELOPE_STAGE_SUSTAIN) {
		// Current stage is attack, decay or release:
		if (currentSampleIndex == nextStageSampleIndex) {
			// We have reached a new envelope stage: 
			EnvelopeStage newStage = static_cast<EnvelopeStage>((currentStage + 1) % kNumEnvelopeStages);
			enterStage(newStage);
		}
		currentLevel *= multiplier;	// the current level calculated is multiplied by the exponential decay
		currentSampleIndex++;
	}
	return currentLevel;
}

/*					  Calculates the exponential decay					 */
/* based on Christian Schoenebeck's Fast Exponential Envelope Generator: */ 
/*				http://www.musicdsp.org/showone.php?id=189				 */
void EnvelopeGenerator::calculateMultiplier(double startLevel, double endLevel, unsigned long long lengthInSamples) {
	multiplier = 1.0 + (log(endLevel) - log(startLevel)) / (lengthInSamples);
}

/* Changes the current envelope values according to the new ADSR stage */
void EnvelopeGenerator::enterStage(EnvelopeStage newStage) {
	if (currentStage == newStage) return;
	if (currentStage == ENVELOPE_STAGE_OFF) {
		beganEnvelopeCycle();		// emits a signal that the envelope calculation has begun
	}
	if (newStage == ENVELOPE_STAGE_OFF) {
		finishedEnvelopeCycle();	// emits a signal that the envelope calculation has stopped
	}
	currentStage = newStage;
	currentSampleIndex = 0;
	if (currentStage == ENVELOPE_STAGE_OFF || currentStage == ENVELOPE_STAGE_SUSTAIN) {
		// When we're in the sustain stage (or the envelope is not active) 
		// we don't know when the user will release (or press) a key.
		nextStageSampleIndex = 0;
	} else {
		// When in attack, decay or release stage, the sample index of the next stage is calculated
		nextStageSampleIndex = stageValue[currentStage] * sampleRate;
	}
	switch (newStage) {
	case EnvelopeGenerator::ENVELOPE_STAGE_OFF:			// Envelope not active:
		// ADSR doesn't affect anything
		currentLevel = 0.0;
		multiplier = 1.0;
		break;
	case EnvelopeGenerator::ENVELOPE_STAGE_ATTACK:		// Envelope in ATTACK stage:
		currentLevel = minimumLevel; 
		// we go from minimum level (0.0001) to maximum level (1)
		calculateMultiplier(currentLevel, 1.0, nextStageSampleIndex);
		break;
	case EnvelopeGenerator::ENVELOPE_STAGE_DECAY:		// Envelope in DECAY stage:
		currentLevel = 1.0;	
		// we go from maximum level to sustain level (or to minimum level if sustain = 0)
		calculateMultiplier(currentLevel, std::max(stageValue[ENVELOPE_STAGE_SUSTAIN], minimumLevel), nextStageSampleIndex);
		break;
	case EnvelopeGenerator::ENVELOPE_STAGE_SUSTAIN:		// Envelope in SUSTAIN stage:
		// we just stay on sustain level
		currentLevel = stageValue[ENVELOPE_STAGE_SUSTAIN]; 
		multiplier = 1.0;
		break;
	case EnvelopeGenerator::ENVELOPE_STAGE_RELEASE:		// Envelope in RELEASE stage:
		// we go from sustain level to minimum level
		calculateMultiplier(currentLevel, minimumLevel, nextStageSampleIndex);
		break;
	default:
		break;
	}
}

/* When a stage parameter is changed from the GUI, new envelope values are calculated */
void EnvelopeGenerator::setStageValue(EnvelopeStage stage, double value) {
	stageValue[stage] = value;	// Saves the value of the respective stage
	if (stage == currentStage) {
		if (currentStage == ENVELOPE_STAGE_ATTACK || currentStage == ENVELOPE_STAGE_DECAY || currentStage == ENVELOPE_STAGE_RELEASE) {
			double nextLevelValue;
			// We're in attack, decay or sustain stage,
			// the level of the next stage is calculated:
			switch (currentStage) {
			case EnvelopeGenerator::ENVELOPE_STAGE_ATTACK:
				nextLevelValue = 1.0;
				break;
			case EnvelopeGenerator::ENVELOPE_STAGE_DECAY:
				nextLevelValue = std::max(stageValue[ENVELOPE_STAGE_SUSTAIN], minimumLevel);
				break;
			case EnvelopeGenerator::ENVELOPE_STAGE_RELEASE:
				nextLevelValue = minimumLevel;
				break;
			default:
				break;
			}
			// We calculate how far we are into the stage calculations...
			double currentStageProcess = (currentSampleIndex + 0.0) / nextStageSampleIndex;
			// ... and how much there is left
			double remainingStageProcess = 1.0 - currentStageProcess;
			unsigned long long samplesUntilNextStage = remainingStageProcess * value * sampleRate;
			nextStageSampleIndex = currentSampleIndex + samplesUntilNextStage;
			calculateMultiplier(currentLevel, nextLevelValue, samplesUntilNextStage);
		}
		else if (currentStage == ENVELOPE_STAGE_SUSTAIN) {
			currentLevel = value;
		}
	}

	// What if sustain value is changed while we're still into decay stage?
	if (currentStage == ENVELOPE_STAGE_DECAY && stage == ENVELOPE_STAGE_SUSTAIN) {
		unsigned long long samplesUntilNextStage = nextStageSampleIndex - currentSampleIndex;
		calculateMultiplier(currentLevel,
			std::max(stageValue[ENVELOPE_STAGE_SUSTAIN], minimumLevel),
			samplesUntilNextStage);
	}
}

/* Updates sample rate */
void EnvelopeGenerator::setSampleRate(double newSampleRate) {
	sampleRate = newSampleRate;
}
