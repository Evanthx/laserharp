#include "HarpNoteDetection.h"

const int pluckDelta = 30;

HarpNoteDetection::HarpNoteDetection()
{
}

HarpNoteDetection::~HarpNoteDetection()
{
}

void HarpNoteDetection::checkNotes(int reflectedLightValues[], bool pluckedNotes[]) {

	for (int testString = 0; testString < numberNotes; testString++) {
		pluckedNotes[testString] = false;

		//We are looking at one string. Compare its reading to every other string. Is the difference over the threshhold?
		//Comparing to yourself just returns a zero, so allow it (that's simpler!) Count up the number of comparison
		//points that are "large" (ie, over the delta.)
		int counter = 0;
		for (int comparisonString = 0; comparisonString < numberNotes; comparisonString++) {
		int difference = reflectedLightValues[testString] - reflectedLightValues[comparisonString];
			if (difference > pluckDelta) {
				counter++;
			}
		}

		//If there are at least three strings with a reading much lower than this one - then
		//count this one as plucked.
		//Three means that we have a wide comparison - one string might be too low, there's tons
		//of variations - but this way we account for ambient light as we're comparing the strings
		//to each other, and since there must be at least three we get pretty robust.
		if (counter > 3) {
			pluckedNotes[testString] = true;
		}
	}

	return;
}

bool HarpNoteDetection::getNotes(int& note1, int& note2, bool pluckedNotes[]){

	note1 = -1;
	note2 = -1;
	int numPlucked = 0;
	for (int testString = 0; testString < numberNotes; testString++) {
		if (pluckedNotes[testString]) {
			numPlucked++;
			if (note1 == -1) {
				note1 = testString;
			}
			else {
				note2 = testString;
			}
		}
	}

	if (numPlucked >= 3) {
		//Too many strings! Fail the call.
		return false;
	}

	//Yay! It was all good!
	return true;
}


